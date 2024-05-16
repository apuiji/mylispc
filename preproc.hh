#pragma once

#include<map>
#include<ostream>
#include<vector>
#include"mylispc.hh"
#include"zlt/myset.hh"

namespace zlt::mylispc {
  struct Macro {
    using Params = std::vector<const std::string *>;
    using ItParam = Params::const_iterator;
    Pos pos;
    Params params;
    UNodes body;
    Macro() = default;
  };

  struct PreprocContext {
    std::ostream &err;
    Symbols &symbols;
    Pos pos;
    PosStack posk;
    std::map<const std::string *, Macro> macros;
    PreprocContext(std::ostream &err, Symbols &symbols) noexcept: out(out), err(err), symbols(symbols) {}
  };

  void preproc(std::ostream &dest, PreprocContext &ctx, UNode &src);

  static inline void preproc(std::ostream &dest, PreprocContext &ctx, UNodes &src) {
    auto it = src.cbegin();
    auto end = src.cend();
    for (; it != end; it = src.erase(it)) {
      preproc(dest, ctx, *it);
    }
  }
}
