#pragma once

#include"nodes1.hh"

namespace zlt::mylispc {
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
    using Defs = std::vector<const std::string *>;
    using HighDefs = std::set<const std::string *>;
    using ClosureDefs = std::map<const std::string *, Reference>;
    size_t paramn;
    Defs defs;
    HighDefs highDefs;
    ClosureDefs closureDefs;
    bool hasGuard;
    UNodes body;
    using Node::Node;
  };

  struct GetHighRef final: Node {
    Reference ref;
    GetHighRef(const char *start, const Reference &ref) noexcept: Node(start), ref(ref) {}
  };

  struct GetRef final: Node {
    Reference ref;
    GetRef(const char *start, const Reference &ref) noexcept: Node(start), ref(ref) {}
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
