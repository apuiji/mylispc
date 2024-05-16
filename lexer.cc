#include<algorithm>
#include<cctype>
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
      reportBad(ctx.err, bad::UNRECOGNIZED_SYMBOL);
      throw Bad();
    }
    string_view raw(it, it1 - it);
    int t = token::ofRaw(numval, ctx.err, raw);
    return { t, it1 };
  }

  bool isRawChar(char c) noexcept {
    return !strchr("\"'();", c) && !isspace(c);
  }

  static size_t esch(int &dest, It it, It end) noexcept;

  Prod lexerStr(string &strval, stringstream &ss, Context &ctx, int quot, It it, It end) {
    if (it == end) [[unlikely]] {
      reportBad(ctx.err, bad::UNTERMINATED_STRING);
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
}
