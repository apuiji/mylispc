#pragma once

#include"mylispc.hh"

namespace zlt::mylispc {
  struct TransContext {
    std::ostream &err;
    std::vector<std::set<const std::string *>> defsk;
    Pos pos;
    std::vector<Pos> posk;
    TransContext(std::ostream &err) noexcept: err(err) {}
  };

  void trans(UNodes &dest, TransContext &ctx, UNode &src);

  static inline void trans(UNodes &dest, TransContext &ctx, UNodes &src) {
    for (; src.size(); src.pop_front()) {
      trans(dest, ctx, src.front());
    }
  }
}
