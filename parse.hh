#pragma once

#include"mylispc.hh"

namespace zlt::mylispc {
  struct ParseContext {
    FILE *err;
    Set<String> &symbols;
    Set<Pos> &poss;
    Pos pos;
  };

  static inline ParseContext makeParseContext(FILE *err, Set<String> &symbols, Set<Pos> &poss, const Pos &pos) noexcept {
    return (ParseContext) { .err = err, .symbols = symbols, .poss = poss, .pos = pos };
  }

  void hit(FILE *src) noexcept;

  /// @param[out] numval when token is NUMBER
  /// @param[out] strval when token is STRING
  /// @return token
  /// @throw bad::Fatal
  int lexer(double &numval, String &strval, const String *&raw, ParseContext &ctx, FILE *src);

  int lexer(ParseContext &ctx, FILE *src);

  /// @param[out] dest Node pointer
  /// @throw bad::Fatal
  void parse(void *&dest, ParseContext &ctx, FILE *src);
}
