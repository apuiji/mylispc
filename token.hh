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

  template<size_t N>
  struct Symbol {
    int value;
    consteval Symbol(const char (&s)[N]):
    value(
      strEnumValue(
        s,
        X,
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

  std::string_view raw(int token) noexcept;

  /// @return 0-not a number, 1-is number, -1-out of range
  int isNumber(double &dest, std::string_view src);

  /// @return token
  int fromRaw(std::string_view src) noexcept;
}

namespace zlt::mylispc {
  template<token::Symbol symbol>
  consteval int operator "" _token() {
    static_assert(symbol.value >= 0);
    return symbol.value;
  }
}
