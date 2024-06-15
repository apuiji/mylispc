#pragma once

#include"mylispc.hh"
#include"zlt/map.hh"

namespace zlt::mylispc {
  void clone(void *&dest, const Pos *upPos, const NumberAtom &src);
  void clone(void *&dest, const Pos *upPos, const StringAtom &src);
  void clone(void *&dest, const Pos *upPos, const IDAtom &src);
  void clone(void *&dest, const Pos *upPos, const TokenAtom &src);
  void clone(void *&dest, const Pos *upPos, const List &src);
  void clone(void *&dest, const Pos *upPos, const void *src);
  void clones(void *&dest, const Pos *upPos, const void *src);

  struct Macro {
    const Pos *pos;
    const String **params;
    size_t paramc;
    void *body;
  };

  struct PreprocContext {
    FILE *err;
    Set<String> &symbols;
    Set<Pos> &poss;
    Map<const String *, Macro> &macros;
    FILE *idout;
  };

  static inline PreprocContext makePreprocContext(
    FILE *err, Set<String> &symbols, Set<Pos> &poss, Map<const String *, Macro> &macros, FILE *idout) noexcept {
    return (PreprocContext) { .err = err, .symbols = symbols, .poss = poss, .macros = macros, .idout = idout };
  }

  void preproc(void *&dest, PreprocContext &ctx, const Pos *upPos, void *&src);

  struct ExpandContext {
    FILE *err;
    Map<const String *, const void *> map;
  };

  static inline ExpandContext makeExpandContext(FILE *err) noexcept {
    return (ExpandContext) { .err = err };
  }

  void expand(void *&dest, ExpandContext &ctx, const Macro &macro, const void *src);
}
