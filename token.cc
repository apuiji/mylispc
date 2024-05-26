#include<cmath>
#include<regex>
#include"token.hh"

using namespace std;

namespace zlt::mylispc::token {
  std::string_view raw(int token) noexcept {
    #define ifToken(r) \
    if (token == r##_token) { \
      return r; \
    }
    // keywords begin
    ifToken("callee");
    ifToken("def");
    ifToken("defer");
    ifToken("forward");
    ifToken("guard");
    ifToken("if");
    ifToken("length");
    ifToken("return");
    ifToken("throw");
    ifToken("try");
    // keywords end
    // preproc operations begin
    ifToken("#");
    ifToken("##");
    ifToken("#def");
    ifToken("#if");
    ifToken("#include");
    ifToken("#movedef");
    ifToken("#undef");
    // preproc operations end
    // mark operations begin
    ifToken("$poppos");
    ifToken("$pos");
    ifToken("$pushpos");
    // mark operations end
    ifToken("!");
    ifToken("%");
    ifToken("&&");
    ifToken("&");
    ifToken("(");
    ifToken(")");
    ifToken("**");
    ifToken("*");
    ifToken("+");
    ifToken(",");
    ifToken("-");
    ifToken(".");
    ifToken("/");
    ifToken("<<");
    ifToken("<=>");
    ifToken("<=");
    ifToken("<");
    ifToken("==");
    ifToken("=");
    ifToken(">=");
    ifToken(">>>");
    ifToken(">>");
    ifToken(">");
    ifToken("@");
    ifToken("^^");
    ifToken("^");
    ifToken("||");
    ifToken("|");
    ifToken("~");
    #undef ifToken
    return {};
  }

  static const regex re2i("([+-]?)0[Bb]([01]+)");
  static const regex re4i("([+-]?)0[Qq]([0-3]+)");
  static const regex re8i("([+-]?)0[Oo]([0-7]+)");
  static const regex re16i("([+-]?)0[Xx]([[:xdigit:]]+)");

  static bool isBaseInt(double &dest, const regex &re, size_t base, string_view src);
  static bool isDecimal(double &dest, string_view src);

  int isNumber(double &dest, string_view src) {
    try {
      bool b =
        isBaseInt(dest, re2i, 2, src) ||
        isBaseInt(dest, re4i, 4, src) ||
        isBaseInt(dest, re8i, 8, src) ||
        isBaseInt(dest, re16i, 16, src) ||
        isDecimal(dest, src);
      return b ? 1 : 0;
    } catch (invalid_argument) {
      return 0;
    } catch (out_of_range) {
      dest = NAN;
      return -1;
    }
  }

  bool isBaseInt(double &dest, const regex &re, size_t base, string_view src) {
    cmatch m;
    if (!regex_match(src.data(), src.data() + src.size(), m, re)) {
      return false;
    }
    dest = stoi(m.str(1) + m.str(2), nullptr, base);
    return true;
  }

  bool isDecimal(double &dest, string_view src) {
    size_t n;
    dest = stod(string(src), &n);
    return n == src.size();
  }

  int fromRaw(double &numval, string_view src) noexcept {
    #define ifRaw(raw) \
    if (src == raw) { \
      return raw##_token; \
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
