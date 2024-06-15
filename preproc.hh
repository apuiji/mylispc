#pragma once

#include"mylispc.hh"
#include"zlt/map.hh"

namespace zlt::mylispc {
  void clone(void *&dest, const NumberAtom &src);
  void clone(void *&dest, const StringAtom &src);
  void clone(void *&dest, const IDAtom &src);
  void clone(void *&dest, const TokenAtom &src);
  void clone(void *&dest, const List &src);
  void clone(void *&dest, const void *src);
  void clones(void *&dest, const void *src);

  struct Macro {
    const String **params;
    size_t paramc;
    void *body;
  };

  struct PreprocContext {
    FILE *err;
    Set<String> &symbols;
    Set<Pos> &poss;
    Map<const String *, Macro> &macros;
  };

  static inline PreprocContext makePreprocContext(
    FILE *err, Set<String> &symbols, Set<Pos> &poss, Map<const String *, Macro> &macros) noexcept {
    return (PreprocContext) { .err = err, .symbols = symbols, .poss = poss, .macros = macros };
  }

  void preproc(void *&dest, PreprocContext &ctx, const Pos *upPos, void *&src);
  void preprocs(void *&dest, PreprocContext &ctx, const Pos *upPos, void *&src);

  struct ExpandContext {
    FILE *err;
    std::map<const std::string *, UNodes::const_iterator> map;
  };

  void expand(UNodes &dest, ExpandContext &ctx, const Macro &macro, UNodes::const_iterator it, UNodes::const_iterator end);
}
