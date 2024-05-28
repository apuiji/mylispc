#pragma once

#include<map>
#include<vector>
#include"mylispc.hh"
#include"zlt/myset.hh"

namespace zlt::mylispc {
  struct ParseContext {
    std::ostream &err;
    Symbols &symbols;
    PosSet &poss;
    Pos pos;
    ParseContext(std::ostream &err, Symbols &symbols, PosSet &poss, const Pos &pos) noexcept:
    err(err), symbols(symbols), poss(poss), pos(pos) {}
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

  int rawToken(double &numval, const Pos *pos, std::string_view src);

  void parse(UNodes &dest, ParseContext &ctx, const char *it, const char *end);
}
