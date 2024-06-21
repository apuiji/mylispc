#pragma once

#include"mylispc.hh"
#include"zlt/map.hh"

namespace zlt::mylispc::preproc {
  void write(FILE *dest, const Pos &pos) noexcept;

  static inline void write(FILE *dest, const Pos *pos) noexcept {
    write(dest, *pos);
  }

  void write(FILE *dest, const EOLAtom &src) noexcept;
  void write(FILE *dest, const NumberAtom &src) noexcept;
  void write(FILE *dest, const StringAtom &src) noexcept;
  void write(FILE *dest, const IDAtom &src) noexcept;
  void write(FILE *dest, const TokenAtom &src) noexcept;
  void write(FILE *dest, const List &src) noexcept;
  void write(FILE *dest, const void *src) noexcept;
  void writes(FILE *dest, const void *src) noexcept;

  struct Macro {
    const Pos *pos;
    const String **params;
    size_t paramc;
    void *body;
  };

  static inline Macro makeMacro(const Pos *pos, const String **params, size_t paramc, void *body) noexcept {
    return (Macro) { .pos = pos, .params = params, .paramc = paramc, .body = body };
  }

  using Macros = Map<const String *, Macro>;

  /// @throw bad::Fatal
  const Macro *addMacro(Macros &dest, Macro &&macro);

  struct PreprocContext {
    FILE *err;
    Set<String> &symbols;
    Set<Pos> &poss;
    Macros &macros;
  };

  static inline PreprocContext makePreprocContext(FILE *err, Set<String> &symbols, Set<Pos> &poss, Macros &macros) noexcept {
    return (PreprocContext) { .err = err, .symbols = symbols, .poss = poss, .macros = macros };
  }

  void preproc(FILE *dest, PreprocContext &ctx, const Pos *upPos, void *&src);

  struct ExpandContext {
    FILE *err;
    Map<const String *, const void *> map;
  };

  static inline ExpandContext makeExpandContext(FILE *err) noexcept {
    return (ExpandContext) { .err = err };
  }

  void expand(FILE *dest, ExpandContext &ctx, const Macro &macro, const void *src);
}
