#pragma once

#include<map>
#include<ostream>
#include<vector>
#include"mylispc.hh"
#include"zlt/myset.hh"

namespace zlt::mylispc {
  void clone(UNode &dest, const NumberAtom &src);
  void clone(UNode &dest, const StringAtom &src);
  void clone(UNode &dest, const IDAtom &src);
  void clone(UNode &dest, const TokenAtom &src);
  void clone(UNode &dest, const List &src);
  void clone(UNode &dest, const UNode &src);
  void clone(UNodes &dest, UNodes::const_iterator it, UNodes::const_iterator end);

  struct Macro {
    using Params = std::vector<const std::string *>;
    using ItParam = Params::const_iterator;
    Params params;
    UNodes body;
    Macro() = default;
  };

  using Macros = std::map<const std::string *, Macro>;

  struct PreprocContext {
    std::ostream &err;
    Symbols &symbols;
    PosSet &poss;
    Macros &macros;
    PreprocContext(std::ostream &err, Symbols &symbols, PosSet &poss, Macros &macros) noexcept:
    err(err), symbols(symbols), poss(poss), macros(macros) {}
  };

  void preproc(UNodes &dest, PreprocContext &ctx, const UNode &src);

  static inline void preproc(UNodes &dest, PreprocContext &ctx, UNodes::const_iterator it, UNodes::const_iterator end) {
    for (; it != end; ++it) {
      preproc(dest, ctx, *it);
    }
  }

  struct ExpandContext {
    std::map<const std::string *, UNodes::const_iterator> map;
  };

  void expand(UNodes &dest, ExpandContext &ctx, const Macro &macro, UNodes::const_iterator it, UNodes::const_iterator end);
}
