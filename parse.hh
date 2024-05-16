#pragma once

#include<map>
#include<vector>
#include"mylispc.hh"
#include"zlt/myset.hh"

namespace zlt::mylispc {
  struct ParseContext {
    std::ostream &err;
    Symbols &symbols;
    Pos pos;
    PosStack &posk;
    ParseContext(std::ostream &err, Symbols &symbols, PosStack &posk) noexcept: err(err), symbols(symbols), posk(posk) {}
  };

  const char *hit(const char *it, const char *end) noexcept;

  /// @param[out] numval when token is NUMBER
  /// @param[out] strval when token is STRING
  /// @return [token, end]
  std::pair<int, const char *> lexer(double &numval, std::string &strval, ParseContext &ctx, const char *it, const char *end);

  static inline std::pair<int, const char *> lexer(ParseContext &ctx, const char *it, const char *end) {
    double d;
    std::string s;
    return lexer(d, s, ctx, it, end);
  }

  void parse(UNodes &dest, ParseContext &ctx, const char *it, const char *end);
}
