#include<regex>
#include"mylispc.hh"
#include"token.hh"

using namespace std;

namespace zlt::mylispc::token {
  static bool isNumber(double &dest, const char *start, string_view raw);

  int ofRaw(double &numval, const char *start, string_view raw) {
    if (isNumber(numval, start, raw)) {
      return NUMBER;
    }
    #define ifRaw(s) \
    if (raw == s) { \
      return s##_token; \
    }
    ifRaw("callee");
    ifRaw("def");
    ifRaw("defer");
    ifRaw("forward");
    ifRaw("if");
    ifRaw("length");
    ifRaw("return");
    ifRaw("throw");
    ifRaw("try");
    ifRaw("yield");
    ifRaw("!");
    ifRaw("#");
    ifRaw("##");
    ifRaw("#def");
    ifRaw("#if");
    ifRaw("#include");
    ifRaw("#undef");
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
    return ID;
  }

  static const regex re2i("([+-]?)0[Bb]([01]+)");
  static const regex re4i("([+-]?)0[Qq]([0-3]+)");
  static const regex re8i("([+-]?)0[Oo]([0-7]+)");
  static const regex re16i("([+-]?)0[Xx]([[:xdigit:]]+)");
  static const regex re10d("[+-]?(?:\\d+\\.\\d*|\\.?\\d+)([Ee][+-]?\\d+)?");

  static bool isBaseInt(double &dest, const regex &re, size_t base, string_view raw);
  static bool isDecimal(double &dest, string_view raw);

  bool isNumber(double &dest, const char *start, string_view raw) {
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
      throw Bad(bad::NUMBER_LITERAL_OOR, start);
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
    cmatch m;
    if (!regex_match(raw.data(), raw.data() + raw.size(), m, re10d)) {
      return false;
    }
    dest = stod(m.str());
    return true;
  }
}
