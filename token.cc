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
}
