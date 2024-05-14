#pragma once

#include"mylispc.hh"

namespace zlt::mylispc::token {
  enum {
    E0F,
    EOL,
    ID,
    NUMBER,
    STRING,
    X
  };

  int ofRaw(double &numval, Context &ctx, std::string_view raw);

  consteval int symbol(std::string_view raw, auto ...s) {
    int i = X;
    ((++i, raw == s) || ... || (i = -1));
    return i;
  }

  template<size_t N>
  struct Symbol {
    int value;
    consteval Symbol(const char (&s)[N]):
    value(
      symbol(
        s,
        // keywords begin
        "callee",
        "def",
        "defer",
        "forward",
        "guard",
        "if",
        "length",
        "return",
        "throw",
        "try",
        // keywords end
        // preproc operations begin
        "#",
        "##",
        "#def",
        "#if",
        "#include",
        "#movedef",
        "#undef",
        // preproc operations end
        // mark operations begin
        "$poppos",
        "$pos",
        "$pushpos",
        // mark operations end
        "!",
        "%",
        "&&",
        "&",
        "(",
        ")",
        "**",
        "*",
        "+",
        ",",
        "-",
        ".",
        "/",
        "<<",
        "<=>",
        "<=",
        "<",
        "==",
        "=",
        ">=",
        ">>>",
        ">>",
        ">",
        "@",
        "^^",
        "^",
        "||",
        "|",
        "~")) {}
  };
}

namespace zlt::mylispc {
  template<token::Symbol symbol>
  consteval int operator "" _token() {
    static_assert(symbol.value >= 0);
    return symbol.value;
  }
}
