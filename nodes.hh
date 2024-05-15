#pragma once

#include"mylispc.hh"

namespace zlt::mylispc {
  struct EOLAtom final: Node {};

  struct NumberAtom final: Node {
    const std::string *raw;
    double value;
    NumberAtom(const std::string *raw, double value) noexcept: raw(raw), value(value) {}
  };

  struct StringAtom final: Node {
    const std::string *value;
    StringAtom(const std::string *value) noexcept: value(value) {}
  };

  struct IDAtom final: Node {
    const std::string *name;
    IDAtom(const std::string *name) noexcept: name(name) {}
  };

  struct TokenAtom final: Node {
    int token;
    TokenAtom(int token) noexcept: token(token) {}
  };

  struct List final: Node {
    UNodes items;
    List(UNodes &&items = {}) noexcept: items(std::move(items)) {}
  };

  template<int T>
  static inline bool isTokenAtom(const UNode &src) noexcept {
    auto a = dynamic_cast<const TokenAtom *>(src.get());
    return a && a->token == T;
  }
}
