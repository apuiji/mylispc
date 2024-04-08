#pragma once

#include<string_view>
#include<type_traits>

namespace zlt::mylispc::token {
  enum {
    CHAR,
    E0F,
    ID,
    NUMBER,
    STRING,
    X
  };

  int ofRaw(double &numval, const char *start, std::string_view raw);

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
        "callee",
        "def",
        "defer",
        "forward",
        "if",
        "length",
        "return",
        "throw",
        "try",
        "yield",
        "!",
        "#",
        "##",
        "#def",
        "#if",
        "#include",
        "#undef",
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
