#include<iterator>
#include"myutils/xyz.hh"
#include"nodes1.hh"
#include"token.hh"

using namespace std;

namespace zlt::mylispc {
  using Defs = set<const string *>;
  using It = UNodes::iterator;

  static void trans(bool global, Defs &defs, UNode &src);

  static inline void trans(bool global, Defs &defs, It it, It end) {
    for (; it != end; ++it) {
      trans(global, defs, *it);
    }
  }

  void trans(UNodes &src) {
    Defs _;
    trans(true, _, src.begin(), src.end());
    src.push_back({});
    src.back().reset(new GlobalReturn(nullptr, nvll()));
  }

  static void transList(UNode &dest, bool global, Defs &defs, const char *start, It it, It end);

  void trans(bool global, Defs &defs, UNode &src) {
    if (auto a = dynamic_cast<const NumberAtom *>(src.get()); a) {
      src.reset(new Number(a->start, a->value));
      return;
    }
    if (auto a = dynamic_cast<const IDAtom *>(src.get()); a) {
      src.reset(new ID(a->start, a->name));
      return;
    }
    if (Dynamicastable<CharAtom, StringAtom> {}(*src)) {
      return;
    }
    if (auto a = dynamic_cast<List *>(src.get()); a) {
      transList(src, global, defs, a->start, a->items.begin(), a->items.end());
      return;
    }
    auto &a = static_cast<TokenAtom &>(*src);
    if (a.token == "callee"_token) {
      if (global) {
        src.reset(new Null(a.start));
      } else {
        src.reset(new Callee(a.start));
      }
      return;
    }
    throw Bad(bad::UNEXPECTED_TOKEN, src->start);
  }

  // aa begin
  static inline UNodes transItems(bool global, Defs &defs, It it, It end) {
    trans(global, defs, it, end);
    return UNodes(move_iterator(it), move_iterator(end));
  }

  static UNode transItem(bool global, Defs &defs, It it, It end) {
    auto a = transItems(global, defs, it, end);
    if (a.empty()) [[unlikely]] {
      return nvll();
    }
    if (a.size() == 1) {
      return std::move(a.front());
    }
    auto start = a.front()->start;
    return UNode(new SequenceOper(start, std::move(a)));
  }

  static void transItemN(UNode *dest, size_t n, bool global, Defs &defs, It it, It end) {
    for (; n > 1 && it != end; ++dest, --n, ++it) {
      trans(global, defs, *it);
      *dest = std::move(*it);
    }
    for (; n > 1; ++dest, --n) {
      *dest = nvll();
    }
    *dest = transItem(global, defs, it, end);
  }

  static Calling transCalling(bool global, Defs &defs, It it, It end) {
    if (it == end) [[unlikely]] {
      return Calling(nvll(), {});
    }
    trans(global, defs, *it);
    auto callee = std::move(*it);
    return Calling(std::move(callee), transItems(global, defs, ++it, end));
  }

  template<class T>
  static inline void transUnaryOper(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    auto a = transItem(global, defs, it, end);
    dest.reset(new T(start, std::move(a)));
  }

  template<class T, size_t N>
  static inline void transXnaryOper(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    array<UNode, N> a;
    transItemN(a.data(), N, global, defs, it, end);
    dest.reset(new T(start, std::move(a)));
  }

  template<class T>
  static inline UNodes &transMultiOper(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    auto a = transItems(global, defs, it, end);
    auto b = new T(start, std::move(a));
    dest.reset(b);
    return b->items;
  }
  // aa end

  template<int>
  void trans(UNode &dest, bool global, Defs &defs, const char *start, It it, It end);

  #define declTrans(T) \
  template<> \
  void trans<T##_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end)

  declTrans("def");
  declTrans("defer");
  declTrans("forward");
  declTrans("if");
  declTrans("length");
  declTrans("return");
  declTrans("throw");
  declTrans("try");
  declTrans("yield");
  declTrans("!");
  declTrans("%");
  declTrans("&&");
  declTrans("&");
  declTrans("**");
  declTrans("*");
  declTrans("+");
  declTrans(",");
  declTrans("-");
  declTrans(".");
  declTrans("/");
  declTrans("<<");
  declTrans("<=>");
  declTrans("<=");
  declTrans("<");
  declTrans("==");
  declTrans("=");
  declTrans(">=");
  declTrans(">>>");
  declTrans(">>");
  declTrans(">");
  declTrans("@");
  declTrans("^^");
  declTrans("^");
  declTrans("||");
  declTrans("|");
  declTrans("~");

  #undef declTrans

  void transList(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    if (it == end) [[unlikely]] {
      dest = nvll(start);
      return;
    }
    auto ta = dynamic_cast<const TokenAtom *>(it->get());
    int t = ta ? ta->token : -1;
    #define ifToken(T) \
    if (t == T##_token) { \
      trans<T##_token>(dest, global, defs, start, ++it, end); \
      return; \
    }
    ifToken("def");
    ifToken("defer");
    ifToken("forward");
    ifToken("if");
    ifToken("length");
    ifToken("return");
    ifToken("throw");
    ifToken("try");
    ifToken("yield");
    ifToken("!");
    ifToken("%");
    ifToken("&&");
    ifToken("&");
    ifToken("**");
    ifToken("*");
    ifToken("+");
    ifToken(",");
    ifToken("-");
    ifToken(".");
    ifToken("/");
    ifToken("<<");
    ifToken("<=>");
    ifToken("<=");
    ifToken("<");
    ifToken("==");
    ifToken("=");
    ifToken(">=");
    ifToken(">>>");
    ifToken(">>");
    ifToken(">");
    ifToken("@");
    ifToken("^^");
    ifToken("^");
    ifToken("||");
    ifToken("|");
    ifToken("~");
    #undef ifToken
    dest.reset(new Call(start, transCalling(global, defs, it, end)));
  }

  template<>
  void trans<"def"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    if (it == end) [[unlikely]] {
      dest = nvll(start);
      return;
    }
    auto id = dynamic_cast<const IDAtom *>(it->get());
    if (!id) {
      throw Bad(bad::UNEXPECTED_TOKEN, (**it).start);
    }
    defs.insert(id->name);
    dest.reset(new ID(start, id->name));
  }

  template<>
  void trans<"defer"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    transUnaryOper<Defer>(dest, global, defs, start, it, end);
  }

  template<>
  void trans<"forward"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    auto calling = transCalling(global, defs, it, end);
    if (global) {
      dest.reset(new GlobalForward(start, std::move(calling)));
    } else {
      dest.reset(new Forward(start, std::move(calling)));
    }
  }

  static void transIf(UNode &dest, bool global, Defs &defs, const char *start, It it, It elze, It end);

  template<>
  void trans<"if"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    It elze = it;
    for (; elze != end && !isTokenAtom<"if"_token>(*it); ++elze);
    transIf(dest, global, defs, start, it, elze, end);
  }

  static void transElse(UNode &dest, bool global, Defs &defs, It it, It end);

  void transIf(UNode &dest, bool global, Defs &defs, const char *start, It it, It elze, It end) {
    UNode cond;
    UNode then;
    if (it == elze) [[unlikely]] {
      cond = nvll();
      then = nvll();
    } else {
      trans(global, defs, *it);
      cond = std::move(*it);
      then = transItem(global, defs, ++it, end);
    }
    UNode elze1;
    transElse(elze1, global, defs, elze, end);
    dest.reset(new If(start, std::move(cond), std::move(then), std::move(elze1)));
  }

  void transElse(UNode &dest, bool global, Defs &defs, It it, It end) {
    if (it != end) {
      auto start = (**it).start;
      trans<"if"_token>(dest, global, defs, start, ++it, end);
    }
  }

  template<>
  void trans<"length"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    transUnaryOper<LengthOper>(dest, global, defs, start, it, end);
  }

  template<>
  void trans<"return"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    if (global) {
      transUnaryOper<GlobalReturn>(dest, global, defs, start, it, end);
    } else {
      transUnaryOper<Return>(dest, global, defs, start, it, end);
    }
  }

  template<>
  void trans<"throw"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    transUnaryOper<Throw>(dest, global, defs, start, it, end);
  }

  template<>
  void trans<"try"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    dest.reset(new Try(start, transCalling(global, defs, it, end)));
  }

  template<>
  void trans<"yield"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    transUnaryOper<Yield>(dest, global, defs, start, it, end);
  }

  template<>
  void trans<"!"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    transUnaryOper<LogicNotOper>(dest, global, defs, start, it, end);
  }

  template<>
  void trans<"%"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    auto &a = transMultiOper<ArithModOper>(dest, global, defs, start, it, end);
    if (a.empty()) {
      dest = number(0, start);
    } else if (a.size() == 1) {
      a.push_back(number(1));
    }
  }

  template<>
  void trans<"&&"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    auto &a = transMultiOper<LogicAndOper>(dest, global, defs, start, it, end);
    if (a.empty()) {
      dest = nvll(start);
    } else if (a.size() == 1) {
      dest = std::move(a.front());
    }
  }

  template<>
  void trans<"&"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    auto &a = transMultiOper<BitwsAndOper>(dest, global, defs, start, it, end);
    if (a.empty()) {
      dest = number(0, start);
    } else if (a.size() == 1) {
      a.push_back(number(-1));
    }
  }

  template<>
  void trans<"**"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    auto &a = transMultiOper<ArithPowOper>(dest, global, defs, start, it, end);
    if (a.empty()) {
      dest = number(0, start);
    } else if (a.size() == 1) {
      a.push_back(number(1));
    }
  }

  template<>
  void trans<"*"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    auto &a = transMultiOper<ArithMulOper>(dest, global, defs, start, it, end);
    if (a.empty()) {
      dest = number(0, start);
    } else if (a.size() == 1) {
      a.push_back(number(1));
    }
  }

  template<>
  void trans<"+"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    auto &a = transMultiOper<ArithAddOper>(dest, global, defs, start, it, end);
    if (a.empty()) {
      dest = number(0, start);
    } else if (a.size() == 1) {
      dest.reset(new PositiveOper(start, std::move(a.front())));
    }
  }

  template<>
  void trans<","_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    dest = transItem(global, defs, it, end);
  }

  template<>
  void trans<"-"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    auto &a = transMultiOper<ArithSubOper>(dest, global, defs, start, it, end);
    if (a.empty()) {
      dest = number(0, start);
    } else if (a.size() == 1) {
      dest.reset(new NegativeOper(start, std::move(a.front())));
    }
  }

  template<>
  void trans<"."_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    auto &a = transMultiOper<GetMembOper>(dest, global, defs, start, it, end);
    if (a.empty()) {
      dest = nvll(start);
    } else if (a.size() == 1) {
      dest = std::move(a.front());
    }
  }

  template<>
  void trans<"/"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    auto &a = transMultiOper<ArithDivOper>(dest, global, defs, start, it, end);
    if (a.empty()) {
      dest = nvll(start);
    } else if (a.size() == 1) {
      a.push_back(number(1));
    }
  }

  template<>
  void trans<"<<"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    auto &a = transMultiOper<LshOper>(dest, global, defs, start, it, end);
    if (a.empty()) {
      dest = number(0, start);
    } else if (a.size() == 1) {
      a.push_back(number(0));
    }
  }

  template<>
  void trans<"<=>"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    transXnaryOper<CompareOper, 2>(dest, global, defs, start, it, end);
  }

  template<>
  void trans<"<="_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    transXnaryOper<CmpLteqOper, 2>(dest, global, defs, start, it, end);
  }

  template<>
  void trans<"<"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    transXnaryOper<CmpLtOper, 2>(dest, global, defs, start, it, end);
  }

  template<>
  void trans<"=="_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    transXnaryOper<CmpEqOper, 2>(dest, global, defs, start, it, end);
  }

  static void setMembOper(UNode &dest, Defs &defs, const char *start, UNode &lhs, UNode &value);

  template<>
  void trans<"="_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    if (it == end) [[unlikely]] {
      dest = nvll(start);
      return;
    }
    trans(global, defs, *it);
    auto a = std::move(*it);
    if (++it == end) [[unlikely]] {
      dest = std::move(a);
      return;
    }
    auto b = transItem(global, defs, it, end);
    if (auto c = dynamic_cast<ID *>(a.get()); c) {
      dest.reset(new SetID(start, c->name, std::move(b)));
      return;
    }
    if (Dynamicastable<GetMembOper> {}(*a)) {
      setMembOper(dest, defs, start, a, b);
      return;
    }
    UNodes c;
    c.push_back(std::move(a));
    c.push_back(std::move(b));
    dest.reset(new SequenceOper(start, std::move(c)));
  }

  void setMembOper(UNode &dest, Defs &defs, const char *start, UNode &lhs, UNode &value) {
    auto &a = static_cast<GetMembOper &>(*lhs);
    array<UNode, 3> b;
    b[2] = std::move(value);
    if (a.items.size() == 2) {
      move(a.items.begin(), a.items.end(), b.begin());
    } else {
      b[1] = std::move(a.items.back());
      a.items.pop_back();
      b[0] = std::move(lhs);
    }
    dest.reset(new SetMembOper(start, std::move(b)));
  }

  template<>
  void trans<">="_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    transXnaryOper<CmpGteqOper, 2>(dest, global, defs, start, it, end);
  }

  template<>
  void trans<">>>"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    auto &a = transMultiOper<UshOper>(dest, global, defs, start, it, end);
    if (a.empty()) {
      dest = number(0, start);
    } else if (a.size() == 1) {
      a.push_back(number(0));
    }
  }

  template<>
  void trans<">>"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    auto &a = transMultiOper<RshOper>(dest, global, defs, start, it, end);
    if (a.empty()) {
      dest = number(0, start);
    } else if (a.size() == 1) {
      a.push_back(number(0));
    }
  }

  template<>
  void trans<">"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    transXnaryOper<CmpGtOper, 2>(dest, global, defs, start, it, end);
  }

  static void transFnParams(Function::Params &dest, Defs &defs, It it, It end);

  template<>
  void trans<"@"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    if (it == end) [[unlikely]] {
      dest.reset(new Function(start));
      return;
    }
    auto ls = dynamic_cast<List *>(it->get());
    if (!ls) {
      throw Bad(bad::UNEXPECTED_TOKEN, (**it).start);
    }
    Defs defs1;
    Function::Params params;
    transFnParams(params, defs1, ls->items.begin(), ls->items.end());
    auto body = transItems(false, defs1, ++it, end);
    UNode a(new Return(nullptr, nvll()));
    body.push_back(std::move(a));
    dest.reset(new Function(start, std::move(defs1), std::move(params), std::move(body)));
  }

  using ItParam = Function::Params::iterator;

  static void cleanDupFnParams(ItParam it, ItParam end, const char *name) noexcept;

  void transFnParams(Function::Params &dest, Defs &defs, It it, It end) {
    for (; it != end; ++it) {
      if (auto a = dynamic_cast<const IDAtom *>(it->get()); a) {
        auto it1 = find(dest.begin(), dest.end(), a->name);
        if (it1 == dest.end()) {
          defs.insert(a->name);
        } else {
          cleanDupFnParams(it1, dest.end(), a->name);
        }
        dest.push_back(a->name);
      } else if (auto a = dynamic_cast<const List *>(it->get()); a && a->items.empty()) {
        dest.push_back(nullptr);
      } else {
        throw Bad(bad::ILL_FN_PARAM, (**it).start);
      }
    }
  }

  static void cleanDupFnParams(ItParam it, ItParam end, const char *name) noexcept {
    do {
      *it = nullptr;
      it = find(++it, end, name);
    } while (it != end);
  }

  template<>
  void trans<"^^"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    auto &a = transMultiOper<LogicXorOper>(dest, global, defs, start, it, end);
    if (a.empty()) {
      dest = fa1se(start);
    } else if (a.size() == 1) {
      a.push_back(fa1se());
    }
  }

  template<>
  void trans<"^"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    auto &a = transMultiOper<BitwsXorOper>(dest, global, defs, start, it, end);
    if (a.empty()) {
      dest = number(0, start);
    } else if (a.size() == 1) {
      a.push_back(number(0));
    }
  }

  template<>
  void trans<"||"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    auto &a = transMultiOper<LogicOrOper>(dest, global, defs, start, it, end);
    if (a.empty()) {
      dest = nvll(start);
    } else if (a.size() == 1) {
      dest = std::move(a.front());
    }
  }

  template<>
  void trans<"|"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    auto &a = transMultiOper<BitwsOrOper>(dest, global, defs, start, it, end);
    if (a.empty()) {
      dest = number(0, start);
    } else if (a.size() == 1) {
      a.push_back(number(0));
    }
  }

  template<>
  void trans<"~"_token>(UNode &dest, bool global, Defs &defs, const char *start, It it, It end) {
    transUnaryOper<BitwsNotOper>(dest, global, defs, start, it, end);
  }
}
