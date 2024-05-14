#include<cmath>
#include<regex>
#include"mylispc.hh"
#include"token.hh"

using namespace std;

namespace zlt::mylispc::token {
  static bool isNumber(double &dest, Context &ctx, string_view raw);

  int ofRaw(double &numval, Context &ctx, string_view raw) {
    if (isNumber(numval, ctx, raw)) {
      return NUMBER;
    }
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
    return ID;
  }

  static const regex re2i("([+-]?)0[Bb]([01]+)");
  static const regex re4i("([+-]?)0[Qq]([0-3]+)");
  static const regex re8i("([+-]?)0[Oo]([0-7]+)");
  static const regex re16i("([+-]?)0[Xx]([[:xdigit:]]+)");
  static const regex re10d("[+-]?(?:\\d+\\.\\d*|\\.?\\d+)([Ee][+-]?\\d+)?");

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
      reportBad(ctx, bad::NUMBER_LITERAL_OOR);
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
    cmatch m;
    if (!regex_match(raw.data(), raw.data() + raw.size(), m, re10d)) {
      return false;
    }
    dest = stod(m.str());
    return true;
  }
}
