#include<algorithm>
#include<cctype>
#include<cmath>
#include<regex>
#include<sstream>
#include"parse.hh"
#include"token.hh"

using namespace std;

namespace zlt::mylispc {
  using Context = ParseContext;
  using It = const char *;

  It hit(It it, It end) noexcept {
    if (it == end) [[unlikely]] {
      return end;
    }
    if (*it == '\n') {
      return it;
    }
    if (isspace(*it)) {
      return hit(it + 1, end);
    }
    if (*it == ';') {
      return find(it + 1, end, '\n');
    }
    return it;
  }

  using Prod = pair<int, It>;

  static Prod lexerStr(string &strval, stringstream &ss, Context &ctx, int quot, It it, It end);
  static bool isRawChar(char c) noexcept;
  static bool isNumber(double &dest, Context &ctx, string_view raw);
  static int rawToken(string_view raw) noexcept;

  Prod lexer(double &numval, string &strval, Context &ctx, It it, It end) {
    if (it == end) [[unlikely]] {
      return { token::E0F, end };
    }
    if (*it == '\n') {
      return { token::EOL, it + 1 };
    }
    if (*it == '(') {
      return { "("_token, it + 1 };
    }
    if (*it == ')') {
      return { ")"_token, it + 1 };
    }
    if (*it == '"' || *it == '\'') {
      stringstream ss;
      return lexerStr(strval, ss, ctx, *it, it + 1, end);
    }
    It it1 = find_if_not(it, end, isRawChar);
    if (it1 == it) {
      reportBad(ctx.err, bad::UNRECOGNIZED_SYMBOL, ctx.pos, ctx.posk);
      throw Bad();
    }
    string_view raw(it, it1 - it);
    if (isNumber(numval, ctx, raw)) {
      return { token::NUMBER, it1 };
    }
    return { rawToken(raw), it1 };
  }

  bool isRawChar(char c) noexcept {
    return !strchr("\"'();", c) && !isspace(c);
  }

  static size_t esch(int &dest, It it, It end) noexcept;

  Prod lexerStr(string &strval, stringstream &ss, Context &ctx, int quot, It it, It end) {
    if (it == end) [[unlikely]] {
      reportBad(ctx.err, bad::UNTERMINATED_STRING, ctx.pos, ctx.posk);
      throw Bad();
    }
    if (It it1 = find_if(it, end, [quot] (char c) { return c == '\\' || c == quot; }); it1 != it) {
      ss.write(it, it1 - it);
      return lexerStr(strval, ss, ctx, quot, it1, end);
    }
    if (*it == '\\') {
      int c;
      size_t n = esch(c, it + 1, end);
      ss.put(c);
      return lexerStr(strval, ss, ctx, quot, it + 1 + n, end);
    }
    strval = ss.str();
    return { token::STRING, it + 1 };
  }

  static size_t esch8(int &dest, It it, It end, size_t limit) noexcept;
  static size_t esch16(int &dest, It it, It end) noexcept;

  size_t esch(int &dest, It it, It end) noexcept {
    if (it == end) {
      dest = '\\';
      return 0;
    }
    if (*it == '"' || *it == '\'' || *it == '\\') {
      dest = *it;
      return 1;
    }
    if (*it == 'n') {
      dest = '\n';
      return 1;
    }
    if (*it == 'r') {
      dest = '\r';
      return 1;
    }
    if (*it == 't') {
      dest = '\t';
      return 1;
    }
    if (*it >= '0' && *it <= '3') {
      return esch8(dest, it, end, 3);
    }
    if (*it >= '4' && *it <= '7') {
      return esch8(dest, it, end, 2);
    }
    if (*it == 'x') {
      return esch16(dest, it, end);
    }
    dest = '\\';
    return 0;
  }

  size_t esch8(int &dest, It it, It end, size_t limit) noexcept {
    dest = 0;
    int n = 0;
    for (; it != end && n < limit && *it >= '0' && *it <= '7'; ++it, ++n) {
      dest = (dest << 3) | (*it - '0');
    }
    return n;
  }

  size_t esch16(int &dest, It it, It end) noexcept {
    if (it + 2 < end && isxdigit(it[1]) && isxdigit(it[2])) {
      dest = stoi(string(it + 1, it + 3), nullptr, 16);
      return 3;
    }
    dest = '\\';
    return 0;
  }

  static const regex re2i("([+-]?)0[Bb]([01]+)");
  static const regex re4i("([+-]?)0[Qq]([0-3]+)");
  static const regex re8i("([+-]?)0[Oo]([0-7]+)");
  static const regex re16i("([+-]?)0[Xx]([[:xdigit:]]+)");

  static bool isBaseInt(double &dest, const regex &re, size_t base, string_view raw);
  static bool isDecimal(double &dest, string_view raw);

  bool isNumber(double &dest, Context &ctx, string_view raw) {
    try {
      return
        isBaseInt(dest, re2i, 2, raw) ||
        isBaseInt(dest, re4i, 4, raw) ||
        isBaseInt(dest, re8i, 8, raw) ||
        isBaseInt(dest, re16i, 16, raw) ||
        isDecimal(dest, raw);
    } catch (invalid_argument) {
      return false;
    } catch (out_of_range) {
      reportBad(ctx.err, bad::NUMBER_LITERAL_OOR, ctx.pos, ctx.posk);
      dest = NAN;
      return true;
    }
  }

  bool isBaseInt(double &dest, const regex &re, size_t base, string_view raw) {
    cmatch m;
    if (!regex_match(raw.data(), raw.data() + raw.size(), m, re)) {
      return false;
    }
    dest = stoi(m.str(1) + m.str(2), nullptr, base);
    return true;
  }

  bool isDecimal(double &dest, string_view raw) {
    size_t n;
    dest = stod(string(raw), &n);
    return n == raw.size();
  }

  int rawToken(string_view raw) noexcept {
    #define ifRaw(s) \
    if (raw == s) { \
      return s##_token; \
    }
    // keywords begin
    ifRaw("callee");
    ifRaw("def");
    ifRaw("defer");
    ifRaw("forward");
    ifRaw("guard");
    ifRaw("if");
    ifRaw("length");
    ifRaw("return");
    ifRaw("throw");
    ifRaw("try");
    // keywords end
    // preproc operations begin
    ifRaw("#");
    ifRaw("##");
    ifRaw("#def");
    ifRaw("#if");
    ifRaw("#include");
    ifRaw("#movedef");
    ifRaw("#undef");
    // preproc operations end
    // mark operations begin
    ifRaw("$poppos");
    ifRaw("$pos");
    ifRaw("$pushpos");
    // mark operations end
    ifRaw("!");
    ifRaw("%");
    ifRaw("&&");
    ifRaw("&");
    ifRaw("(");
    ifRaw(")");
    ifRaw("**");
    ifRaw("*");
    ifRaw("+");
    ifRaw(",");
    ifRaw("-");
    ifRaw(".");
    ifRaw("/");
    ifRaw("<<");
    ifRaw("<=>");
    ifRaw("<=");
    ifRaw("<");
    ifRaw("==");
    ifRaw("=");
    ifRaw(">=");
    ifRaw(">>>");
    ifRaw(">>");
    ifRaw(">");
    ifRaw("@");
    ifRaw("^^");
    ifRaw("^");
    ifRaw("||");
    ifRaw("|");
    ifRaw("~");
    #undef ifRaw
    return token::ID;
  }
}
