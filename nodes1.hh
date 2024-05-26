#pragma once

#include<array>
#include"nodes.hh"
#include"token.hh"

namespace zlt::mylispc {
  struct Calling {
    UNode callee;
    UNodes args;
    Calling(UNode &&callee, UNodes &&args) noexcept: callee(std::move(callee)), args(std::move(args)) {}
  };

  struct Call final: Node, Calling {
    Call(const Pos *pos, Calling &&calling) noexcept: Node(pos), Calling(std::move(calling)) {}
  };

  struct Callee final: Node {
    using Node::Node;
  };

  struct Defer final: Node {
    UNode value;
    Defer(const Pos *pos, UNode &&value) noexcept: Node(pos), value(std::move(value)) {}
  };

  struct Forward final: Node, Calling {
    Forward(const Pos *pos, Calling &&calling) noexcept: Node(pos), Calling(std::move(calling)) {}
  };

  struct Guard final: Node {
    UNode value;
    Guard(const Pos *pos, UNode &&value) noexcept: Node(pos), value(std::move(value)) {}
  };

  struct Function final: Node {
    using Defs = std::set<const std::string *>;
    using Params = std::vector<const std::string *>;
    Defs defs;
    Params params;
    UNodes body;
    using Node::Node;
    Function(const Pos *pos, Defs &&defs, Params &&params, UNodes &&body) noexcept:
    Node(pos), defs(std::move(defs)), params(std::move(params)), body(std::move(body)) {}
  };

  using ID = IDAtom;

  struct If final: Node {
    UNode cond;
    UNode then;
    UNode elze;
    If(const Pos *pos, UNode &&cond, UNode &&then, UNode &&elze) noexcept:
    Node(pos), cond(std::move(cond)), then(std::move(then)), elze(std::move(elze)) {}
  };

  struct Null final: Node {
    using Node::Node;
  };

  struct Number final: Node {
    double value;
    Number(const Pos *pos, double value) noexcept: Node(pos), value(value) {}
  };

  struct Return final: Node {
    UNode value;
    Return(const Pos *pos, UNode &&value) noexcept: Node(pos), value(std::move(value)) {}
  };

  struct SetID final: Node {
    const std::string *name;
    UNode value;
    SetID(const Pos *pos, const std::string *name, UNode &&value) noexcept: Node(pos), name(name), value(std::move(value)) {}
  };

  struct Throw final: Node {
    UNode value;
    Throw(const Pos *pos, UNode &&value) noexcept: Node(pos), value(std::move(value)) {}
  };

  struct Try final: Node, Calling {
    Try(const Pos *pos, Calling &&calling) noexcept: Node(pos), Calling(std::move(calling)) {}
  };

  // operations begin
  template<int N>
  struct Operation: Node {
    std::array<UNode, N> items;
    Operation(const Pos *pos, std::array<UNode, N> &&items) noexcept: Node(pos), items(std::move(items)) {}
  };

  template<>
  struct Operation<1>: Node {
    UNode item;
    Operation(const Pos *pos, UNode &&item) noexcept: Node(pos), item(std::move(item)) {}
  };

  template<>
  struct Operation<-1>: Node {
    UNodes items;
    Operation(const Pos *pos, UNodes &&items) noexcept: Node(pos), items(std::move(items)) {}
  };

  template<int N, int Op>
  struct Operation1 final: Operation<N> {
    using Operation<N>::Operation;
  };

  // arithmetical operations begin
  using ArithAddOper = Operation1<-1, "+"_token>;
  using ArithSubOper = Operation1<-1, "-"_token>;
  using ArithMulOper = Operation1<-1, "*"_token>;
  using ArithDivOper = Operation1<-1, "/"_token>;
  using ArithModOper = Operation1<-1, "%"_token>;
  using ArithPowOper = Operation1<-1, "**"_token>;
  // arithmetical operations end
  // logical operations begin
  using LogicAndOper = Operation1<-1, "&&"_token>;
  using LogicOrOper = Operation1<-1, "||"_token>;
  using LogicNotOper = Operation1<1, "!"_token>;
  using LogicXorOper = Operation1<-1, "^^"_token>;
  // logical operations end
  // bitwise operations begin
  using BitwsAndOper = Operation1<-1, "&"_token>;
  using BitwsOrOper = Operation1<-1, "|"_token>;
  using BitwsNotOper = Operation1<1, "~"_token>;
  using BitwsXorOper = Operation1<-1, "^"_token>;
  using LshOper = Operation1<-1, "<<"_token>;
  using RshOper = Operation1<-1, ">>"_token>;
  using UshOper = Operation1<-1, ">>>"_token>;
  // bitwise operations end
  // compare operations begin
  using CmpEqOper = Operation1<2, "=="_token>;
  using CmpLtOper = Operation1<2, "<"_token>;
  using CmpGtOper = Operation1<2, ">"_token>;
  using CmpLteqOper = Operation1<2, "<="_token>;
  using CmpGteqOper = Operation1<2, ">="_token>;
  using CompareOper = Operation1<2, "<=>"_token>;
  // compare operations end
  using GetMembOper = Operation1<-1, "."_token>;
  using LengthOper = Operation1<1, "length"_token>;
  using NegativeOper = Operation1<1, "-"_token>;
  using PositiveOper = Operation1<1, "+"_token>;
  using SequenceOper = Operation1<-1, ","_token>;

  struct SetMembOper final: Operation<3> {
    using Operation<3>::Operation;
  };
  // operations end

  static inline UNode number(double value, const Pos *pos = nullptr) {
    return UNode(new Number(pos, value));
  }

  static inline UNode nvll(const Pos *pos = nullptr) {
    return UNode(new Null(pos));
  }

  static inline UNode trve(const Pos *pos = nullptr) {
    return number(1, pos);
  }

  static inline UNode fa1se(const Pos *pos = nullptr) {
    return nvll(pos);
  }

  static inline UNode boo1(bool b, const Pos *pos = nullptr) {
    return b ? trve(pos) : fa1se(pos);
  }

  static inline UNode callee(const Pos *pos = nullptr) {
    return UNode(new Callee(pos));
  }
}
