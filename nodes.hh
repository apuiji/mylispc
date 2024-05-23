#pragma once

#include"mylispc.hh"
#include"token.hh"

namespace zlt::mylispc {
  struct EOLAtom final: Node {};

  struct RawAtom: virtual Node {
    virtual std::string_view raw() const noexcept = 0;
  };

  struct NumberAtom final: RawAtom {
    const std::string *rawval;
    double value;
    NumberAtom(const std::string *rawval, double value) noexcept: rawval(rawval), value(value) {}
    std::string_view raw() const noexcept override {
      return *rawval;
    }
  };

  struct StringAtom final: Node {
    const std::string *value;
    StringAtom(const std::string *value) noexcept: value(value) {}
  };

  struct IDAtom final: RawAtom {
    const std::string *name;
    IDAtom(const std::string *name) noexcept: name(name) {}
    std::string_view raw() const noexcept override {
      return *name;
    }
  };

  struct TokenAtom final: RawAtom {
    int token;
    TokenAtom(int token) noexcept: token(token) {}
    std::string_view raw() const noexcept override {
      return mylispc::token::raw(token);
    }
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
