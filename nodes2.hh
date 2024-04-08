#pragma once

#include"nodes1.hh"

namespace zlt::mylispc {
  struct Argument final: Node {
    size_t index;
    Argument(size_t index) noexcept: index(index) {}
  };

  struct ClearArgs final: Node {
    ClearArgs() noexcept: Node() {}
  };

  struct Reference {
    enum {
      CLOSURE_SCOPE,
      GLOBAL_SCOPE,
      LOCAL_SCOPE
    };
    int scope;
    const std::string *name;
    Reference() = default;
    Reference(int scope, const std::string *name) noexcept: scope(scope), name(name) {}
  };

  struct Function1 final: Node {
    using Defs = Function::Defs;
    using ClosureDefs = std::map<const std::string *, Reference>;
    Defs highDefs;
    ClosureDefs closureDefs;
    UNodes body;
    Function1(const char *start, Defs &&highDefs, ClosureDefs &&closureDefs, UNodes &&body) noexcept:
    Node(start), highDefs(std::move(highDefs)), closureDefs(std::move(closureDefs)), body(std::move(body)) {}
  };

  struct GetHighRef final: Node {
    Reference ref;
    GetHighRef(const char *start, const Reference &ref) noexcept: Node(start), ref(ref) {}
  };

  struct GetRef final: Node {
    Reference ref;
    GetRef(const char *start, const Reference &ref) noexcept: Node(start), ref(ref) {}
  };

  struct MakeHighRef final: Node {
    using Node::Node;
  };

  struct SetHighRef final: Node {
    Reference ref;
    UNode value;
    SetHighRef(const char *start, const Reference &ref, UNode &&value) noexcept: Node(start), ref(ref), value(std::move(value))
    {}
  };

  struct SetRef final: Node {
    Reference ref;
    UNode value;
    SetRef(const char *start, const Reference &ref, UNode &&value) noexcept: Node(start), ref(ref), value(std::move(value)) {}
  };
}
