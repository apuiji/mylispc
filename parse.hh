#pragma once

#include"mylispc.hh"

namespace zlt::mylispc {
  struct ParseContext {
    FILE *err;
    Set<String> &symbols;
    Set<Pos> &poss;
    Pos pos;
    FILE *strout;
    size_t strsize;
  };

  static inline ParseContext makeParseContext(FILE *err, Set<String> &symbols, Set<Pos> &poss, const Pos &pos) noexcept {
    return (ParseContext) { .err = err, .symbols = symbols, .poss = poss, .pos = pos };
  }

  const char *hit(Pos &pos, const char *it, const char *end) noexcept;

  /// @param[out] numval when token is NUMBER
  /// @param[out] strval when token is STRING
  /// @return [token, end]
  const char *lexer(int &tokval, double &numval, String &strval, ParseContext &ctx, const char *it, const char *end);

  const char *lexer(int &tokval, ParseContext &ctx, const char *it, const char *end);

  void parse(void *&dest, ParseContext &ctx, const char *it, const char *end);
}
