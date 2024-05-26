#pragma once

#include"mylispc.hh"
#include"token.hh"

namespace zlt::mylispc {
  struct RawAtom: virtual Node {
    using Node::Node;
    virtual std::string_view raw() const noexcept = 0;
  };

  struct NumberAtom final: RawAtom {
    const std::string *rawval;
    double value;
    NumberAtom(const Pos *pos, const std::string *rawval, double value) noexcept: RawAtom(pos), rawval(rawval), value(value) {}
    std::string_view raw() const noexcept override {
      return *rawval;
    }
  };

  struct StringAtom final: Node {
    const std::string *value;
    StringAtom(const Pos *pos, const std::string *value) noexcept: Node(pos), value(value) {}
  };

  struct IDAtom final: RawAtom {
    const std::string *name;
    IDAtom(const Pos *pos, const std::string *name) noexcept: RawAtom(pos), name(name) {}
    std::string_view raw() const noexcept override {
      return *name;
    }
  };

  struct TokenAtom final: RawAtom {
    int token;
    TokenAtom(const Pos *pos, int token) noexcept: RawAtom(pos), token(token) {}
    std::string_view raw() const noexcept override {
      return mylispc::token::raw(token);
    }
  };

  struct List final: Node {
    UNodes items;
    List(const Pos *pos, UNodes &&items = {}) noexcept: Node(pos), items(std::move(items)) {}
  };

  template<int T>
  static inline bool isTokenAtom(const UNode &src) noexcept {
    auto a = dynamic_cast<const TokenAtom *>(src.get());
    return a && a->token == T;
  }
}
