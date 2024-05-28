#pragma once

#include"mylispc.hh"

namespace zlt::mylispc {
  struct TransContext {
    std::ostream &err;
    TransContext(std::ostream &err) noexcept: err(err) {}
  };

  void trans(TransContext &ctx, std::set<const std::string *> &defs, UNode &src);

  static inline void trans(TransContext &ctx, std::set<const std::string *> &defs, UNodes::iterator it, UNodes::iterator end) {
    for (; it != end; ++it) {
      trans(ctx, defs, *it);
    }
  }

  void optimize(UNode &src);

  static inline void optimize(UNodes::iterator it, UNodes::iterator end) {
    for (; it != end; ++it) {
      optimize(*it);
    }
  }

  /// @return unterminated
  bool optimizeBody(UNodes &dest, UNodes &src);
}
