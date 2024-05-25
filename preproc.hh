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

  using Macros = std::map<const std::string *, Macro>;

  struct PreprocContext {
    std::ostream &err;
    Symbols &symbols;
    Macros &macros;
    Pos pos;
    PosStack &posk;
    PreprocContext(std::ostream &err, Symbols &symbols, Macros &macros, const Pos &pos, PosStack &posk) noexcept:
    err(err), symbols(symbols), macros(macros), pos(pos), posk(posk) {}
  };

  void preproc(std::ostream &dest, PreprocContext &ctx, UNode &src);

  static inline void preproc(std::ostream &dest, PreprocContext &ctx, UNodes &src) {
    for (; src.size(); src.pop_front()) {
      preproc(dest, ctx, src.front());
    }
  }

  struct ExpandContext {
    Pos pos;
    std::map<const std::string *, std::pair<Pos, UNodes::const_iterator>> map;
    ExpandContext(const Pos &pos) noexcept: pos(pos) {}
  };

  void expand(std::ostream &dest, ExpandContext &ctx, const Macro &macro, UNodes &src);

  namespace preproc_output {
    void outputStr(std::ostream &dest, const unsigned char *it, const unsigned char *end);

    static inline void outputStr(std::ostream &dest, std::string_view src) {
      outputStr(dest, (const unsigned char *) src.data(), (const unsigned char *) src.data() + src.size());
    }

    static inline void outputStr1(std::ostream &dest, std::string_view src) {
      dest.put('\'');
      outputStr(dest, src);
      dest.put('\'');
    }

    void outputPos(std::ostream &dest, const Pos &pos);
    void outputStr1(std::ostream &dest, const Pos &pos);
  }
}
