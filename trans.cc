#include<iterator>
#include"nodes1.hh"
#include"token.hh"

using namespace std;

namespace zlt::mylispc {
  using Context = TransContext;
  using Defs = set<const string *>;
  using It = UNodes::iterator;

  static void transList(Context &ctx, Defs &defs, const Pos *pos, It it, It end);

  void trans(Context &ctx, Defs &defs, UNode &src) {
    if (auto a = dynamic_cast<Number *>(src.get()); a) {
      src = number(a->value, a->pos);
      return;
    }
    if (Dynamicastable<StringAtom, IDAtom> {}(*src)) {
      return;
    }
    if (auto a = dynamic_cast<List *>(src.get()); a && a->items.size()) {
      transList(ctx, defs, a->pos, a->items.begin(), a->items.end());
      return;
    }
    if (isTokenAtom<"callee"_token>(src)) {
      src = callee(src->pos);
      return;
    }
    bad::report(ctx.err, bad::UNEXPECTED_TRANS_TOKEN, src->pos);
    src = nvll(src->pos);
  }

  // aa begin
  static inline UNodes transItems(Context &ctx, Defs &defs, It it, It end) {
    trans(ctx, defs, it, end);
    return UNodes(move_iterator(it), move_iterator(end));
  }

  static UNode transItem(Context &ctx, Defs &defs, It it, It end) {
    auto a = transItems(ctx, defs, it, end);
    if (a.empty()) {
      return nvll();
    }
    if (a.size() == 1) {
      return std::move(a.back());
    }
    auto pos = a.front()->pos;
    return UNode(new SequenceOper(pos, std::move(a)));
  }

  static void transItemN(UNode *dest, size_t n, Context &ctx, Defs &defs, It it, It end) {
    for (; n > 1 && it != end; ++dest, --n, ++it) {
      trans(ctx, defs, *it);
      *dest = std::move(*it);
    }
    for (; n > 1; ++dest, --n) {
      *dest = nvll();
    }
    *dest = transItem(ctx, defs, it, end);
  }

  static Calling transCalling(Context &ctx, Defs &defs, It it, It end) {
    auto a = transItems(ctx, defs, it, end);
    if (a.empty()) [[unlikely]] {
      return Calling(nvll(), {});
    }
    auto callee = std::move(a.front());
    a.pop_front();
    return Calling(std::move(callee), std::move(a));
  }
  // aa end

  using Trans = void (UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end);

  static Trans *isTrans(const UNode &src) noexcept;

  void transList(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    if (it == end) [[unlikely]] {
      dest = nvll(pos);
      return;
    }
    if (auto t = isTrans(*it); t) {
      t(dest, ctx, defs, pos, it, end);
      return;
    }
    auto c = transCalling(ctx, defs, it, end);
    dest.reset(new Call(pos, std::move(c)));
  }

  static Trans transDef;
  static Trans transDefer;
  static Trans transForward;
  static Trans transGuard;
  static Trans transIf;
  static Trans transLength;
  static Trans transReturn;
  static Trans transThrow;
  static Trans transTry;
  static Trans transExclam;
  static Trans transPercent;
  static Trans transAmp2;
  static Trans transAmp;
  static Trans transAsterisk2;
  static Trans transAsterisk;
  static Trans transPlus;
  static Trans transComma;
  static Trans transMinus;
  static Trans transDot;
  static Trans transSlash;
  static Trans transLt2;
  static Trans transLtEqGt;
  static Trans transLtEq;
  static Trans transLt;
  static Trans transEq2;
  static Trans transEq;
  static Trans transGtEq;
  static Trans transGt3;
  static Trans transGt2;
  static Trans transGt;
  static Trans transAt;
  static Trans transCaret2;
  static Trans transCaret;
  static Trans transVertical2;
  static Trans transVertical;
  static Trans transTilde;

  Trans *isTrans(const UNode &src) noexcept {
    auto t = dynamic_cast<const TokenAtom *>(src.get());
    if (!t) [[unlikely]] {
      return nullptr;
    }
    if (t->token == "def"_token) {
      return transDef;
    }
    if (t->token == "defer"_token) {
      return transDefer;
    }
    if (t->token == "forward"_token) {
      return transForward;
    }
    if (t->token == "guard"_token) {
      return transGuard;
    }
    if (t->token == "if"_token) {
      return transIf;
    }
    if (t->token == "length"_token) {
      return transLength;
    }
    if (t->token == "return"_token) {
      return transReturn;
    }
    if (t->token == "throw"_token) {
      return transThrow;
    }
    if (t->token == "try"_token) {
      return transTry;
    }
    if (t->token == "!"_token) {
      return transExclam;
    }
    if (t->token == "%"_token) {
      return transPercent;
    }
    if (t->token == "&&"_token) {
      return transAmp2;
    }
    if (t->token == "&"_token) {
      return transAmp;
    }
    if (t->token == "**"_token) {
      return transAsterisk2;
    }
    if (t->token == "*"_token) {
      return transAsterisk;
    }
    if (t->token == "+"_token) {
      return transPlus;
    }
    if (t->token == ","_token) {
      return transComma;
    }
    if (t->token == "-"_token) {
      return transMinus;
    }
    if (t->token == "."_token) {
      return transDot;
    }
    if (t->token == "/"_token) {
      return transSlash;
    }
    if (t->token == "<<"_token) {
      return transLt2;
    }
    if (t->token == "<=>"_token) {
      return transLtEqGt;
    }
    if (t->token == "<="_token) {
      return transLtEq;
    }
    if (t->token == "<"_token) {
      return transLt;
    }
    if (t->token == "=="_token) {
      return transEq2;
    }
    if (t->token == "="_token) {
      return transEq;
    }
    if (t->token == ">="_token) {
      return transGtEq;
    }
    if (t->token == ">>>"_token) {
      return transGt3;
    }
    if (t->token == ">>"_token) {
      return transGt2;
    }
    if (t->token == ">"_token) {
      return transGt;
    }
    if (t->token == "@"_token) {
      return transAt;
    }
    if (t->token == "^^"_token) {
      return transCaret2;
    }
    if (t->token == "^"_token) {
      return transCaret;
    }
    if (t->token == "||"_token) {
      return transVertical2;
    }
    if (t->token == "|"_token) {
      return transVertical;
    }
    if (t->token == "~"_token) {
      return transTilde;
    }
    return nullptr;
  }

  void transDef(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    if (it == end) [[unlikely]] {
      dest = nvll(pos);
      return;
    }
    auto id = dynamic_cast<const IDAtom *>(it->get());
    if (!id) {
      bad::report(ctx.err, bad::UNEXPECTED_TRANS_TOKEN, (**it).pos);
      dest = nvll(pos);
      return;
    }
    defs.insert(id->name);
    dest.reset(new ID(pos, id->name));
  }

  void transDefer(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    auto a = transItem(ctx, defs, it, end);
    dest.reset(new Defer(pos, std::move(a)));
  }

  void transForward(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    auto calling = transCalling(ctx, defs, it, end);
    dest.reset(new Forward(pos, std::move(calling)));
  }

  void transGuard(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    auto a = transItem(ctx, defs, it, end);
    dest.reset(new Guard(pos, std::move(a)));
  }

  static void transIf(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It elze, It end);

  void transIf(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    It elze = find_if(it, end, isTokenAtom<"if"_token>);
    transIf(dest, ctx, defs, pos, it, elze, end);
  }

  static void transElse(UNode &dest, Context &ctx, Defs &defs, It it, It end);

  void transIf(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It elze, It end) {
    UNode cond;
    UNode then;
    if (it == elze) [[unlikely]] {
      cond = nvll();
      then = nvll();
    } else {
      trans(ctx, defs, *it);
      cond = std::move(*it);
      then = transItem(ctx, defs, ++it, elze);
    }
    UNode elze1;
    transElse(elze1, ctx, defs, elze, end);
    dest.reset(new If(pos, std::move(cond), std::move(then), std::move(elze1)));
  }

  void transElse(UNode &dest, Context &ctx, Defs &defs, It it, It end) {
    if (it == end) [[unlikely]] {
      dest = nvll();
      return;
    }
    auto pos = (**it).pos;
    transIf(dest, ctx, defs, pos, ++it, end);
  }

  void transLength(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    auto a = transItem(ctx, defs, it, end);
    dest.reset(new LengthOper(pos, std::move(a)));
  }

  void transReturn(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    auto a = transItem(ctx, defs, it, end);
    dest.reset(new Return(pos, std::move(a)));
  }

  void transThrow(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    auto a = transItem(ctx, defs, it, end);
    dest.reset(new Throw(pos, std::move(a)));
  }

  void transTry(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    auto c = transCalling(ctx, defs, it, end);
    dest.reset(new Try(pos, std::move(c)));
  }

  void transExclam(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    auto a = transItem(ctx, defs, it, end);
    dest.reset(new LogicNotOper(pos, std::move(a)));
  }

  void transPercent(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    auto a = transItems(ctx, defs, it, end);
    if (a.empty()) [[unlikely]] {
      dest = number(0, pos);
      return;
    }
    if (a.size() == 1) {
      a.push_back(number(1));
    }
    dest.reset(new ArithModOper(pos, std::move(a)));
  }

  void transAmp2(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    auto a = transItems(ctx, defs, it, end);
    if (a.empty()) [[unlikely]] {
      dest = nvll(pos);
      return;
    }
    if (a.size() == 1) [[unlikely]] {
      dest = std::move(a.back());
      return;
    }
    dest.reset(new LogicAndOper(pos, std::move(a)));
  }

  void transAmp(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    auto a = transItems(ctx, defs, it, end);
    if (a.empty()) [[unlikely]] {
      dest = number(0, pos);
      return;
    }
    if (a.size() == 1) {
      a.push_back(number(-1));
    }
    dest.reset(new BitwsAndOper(pos, std::move(a)));
  }

  void transAsterisk2(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    auto a = transItems(ctx, defs, it, end);
    if (a.empty()) [[unlikely]] {
      dest = number(0, pos);
      return;
    }
    if (a.size() == 1) {
      a.push_back(number(1));
    }
    dest.reset(new ArithPowOper(pos, std::move(a)));
  }

  void transAsterisk(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    auto a = transItems(ctx, defs, it, end);
    if (a.empty()) [[unlikely]] {
      dest = number(0, pos);
      return;
    }
    if (a.size() == 1) {
      a.push_back(number(1));
    }
    dest.reset(new ArithMulOper(pos, std::move(a)));
  }

  void transPlus(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    auto a = transItems(ctx, defs, it, end);
    if (a.empty()) [[unlikely]] {
      dest = number(0, pos);
      return;
    }
    if (a.size() == 1) {
      dest.reset(new PositiveOper(pos, std::move(a.back())));
    } else {
      dest.reset(new ArithAddOper(pos, std::move(a)));
    }
  }

  void transComma(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    dest = transItem(ctx, defs, it, end);
  }

  void transMinus(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    auto a = transItems(ctx, defs, it, end);
    if (a.empty()) [[unlikely]] {
      dest = number(0, pos);
      return;
    }
    if (a.size() == 1) {
      dest.reset(new NegativeOper(pos, std::move(a.back())));
    } else {
      dest.reset(new ArithSubOper(pos, std::move(a)));
    }
  }

  void transDot(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    auto a = transItems(ctx, defs, it, end);
    if (a.empty()) [[unlikely]] {
      dest = nvll(pos);
      return;
    }
    if (a.size() == 1) [[unlikely]] {
      dest = std::move(a.back());
      return;
    }
    dest.reset(new GetMembOper(pos, std::move(a)));
  }

  void transSlash(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    auto a = transItems(ctx, defs, it, end);
    if (a.empty()) [[unlikely]] {
      dest = nvll(pos);
      return;
    }
    if (a.size() == 1) {
      a.push_back(number(1));
    }
    dest.reset(new ArithDivOper(pos, std::move(a)));
  }

  void transLt2(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    auto a = transItems(ctx, defs, it, end);
    if (a.empty()) [[unlikely]] {
      dest = number(0, pos);
      return;
    }
    if (a.size() == 1) {
      a.push_back(number(0));
    }
    dest.reset(new LshOper(pos, std::move(a)));
  }

  void transLtEqGt(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    array<UNode, 2> a;
    transItemN(a.data(), 2, ctx, defs, it, end);
    dest.reset(new CompareOper(pos, std::move(a)));
  }

  void transLtEq(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    array<UNode, 2> a;
    transItemN(a.data(), 2, ctx, defs, it, end);
    dest.reset(new CmpLteqOper(pos, std::move(a)));
  }

  void transLt(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    array<UNode, 2> a;
    transItemN(a.data(), 2, ctx, defs, it, end);
    dest.reset(new CmpLtOper(pos, std::move(a)));
  }

  void transEq2(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    array<UNode, 2> a;
    transItemN(a.data(), 2, ctx, defs, it, end);
    dest.reset(new CmpEqOper(pos, std::move(a)));
  }

  static void setMembOper(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, UNode &lhs, UNode &value);

  void transEq(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    if (it == end) [[unlikely]] {
      bad::report(ctx.err, bad::ILL_ASSIGN, pos);
      dest = nvll(pos);
      return;
    }
    trans(ctx, defs, *it);
    auto a = std::move(*it);
    if (++it == end) [[unlikely]] {
      bad::report(ctx.err, bad::ILL_ASSIGN, pos);
      dest = std::move(a);
      return;
    }
    auto b = transItem(ctx, defs, it, end);
    if (auto c = dynamic_cast<ID *>(a.get()); c) {
      dest.reset(new SetID(pos, c->name, std::move(b)));
      return;
    }
    if (Dynamicastable<GetMembOper> {}(*a)) {
      setMembOper(dest, ctx, defs, pos, a, b);
      return;
    }
    bad::report(ctx.err, bad::INV_LHS, a->pos);
    UNodes c;
    c.push_back(std::move(a));
    c.push_back(std::move(b));
    dest.reset(new SequenceOper(start, std::move(c)));
  }

  void setMembOper(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, UNode &lhs, UNode &value) {
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

  void transGtEq(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    array<UNode, 2> a;
    transItemN(a.data(), 2, ctx, defs, it, end);
    dest.reset(new CmpGteqOper(pos, std::move(a)));
  }

  void transGt3(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    auto a = transItems(ctx, defs, it, end);
    if (a.empty()) [[unlikely]] {
      dest = number(0, pos);
      return;
    }
    if (a.size() == 1) {
      a.push_back(number(0));
    }
    dest.reset(new UshOper(pos, std::move(a)));
  }

  void transGt2(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    auto a = transItems(ctx, defs, it, end);
    if (a.empty()) [[unlikely]] {
      dest = number(0, pos);
      return;
    }
    if (a.size() == 1) {
      a.push_back(number(0));
    }
    dest.reset(new RshOper(pos, std::move(a)));
  }

  void transGt(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    array<UNode, 2> a;
    transItemN(a.data(), 2, ctx, defs, it, end);
    dest.reset(new CmpGtOper(pos, std::move(a)));
  }

  static void transFnParams(Function::Params &dest, Context &ctx, Defs &defs, It it, It end);

  void transAt(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    if (it == end) [[unlikely]] {
      dest.reset(new Function(pos));
      return;
    }
    Defs defs1;
    Function::Params params;
    if (auto ls = dynamic_cast<List *>(it->get()); ls) {
      transFnParams(params, ctx, defs1, ls->items.begin(), ls->items.end());
    } else {
      bad::report(ctx.err, bad::UNEXPECTED_TRANS_TOKEN, (**it).pos);
    }
    auto body = transItems(ctx, defs1, ++it, end);
    body.push_back({});
    body.back().reset(new Return(nullptr, nvll()));
    dest.reset(new Function(pos, std::move(defs1), std::move(params), std::move(body)));
  }

  using ItParam = Function::Params::iterator;

  static void cleanDupFnParams(ItParam it, ItParam end, const string *name) noexcept;

  void transFnParams(Function::Params &dest, Defs &defs, It it, It end) {
    for (; it != end; ++it) {
      if (auto a = dynamic_cast<const IDAtom *>(it->get()); a) {
        cleanDupFnParams(dest.begin(), dest.end(), a->name);
        defs.insert(a->name);
        dest.push_back(a->name);
      } else if (auto a = dynamic_cast<const List *>(it->get()); a && a->items.empty()) {
        dest.push_back(nullptr);
      } else {
        bad::report(ctx.err, bad::ILL_FN_PARAM, (**it).pos);
        dest.push_back(nullptr);
      }
    }
  }

  void cleanDupFnParams(ItParam it, ItParam end, const string *name) noexcept {
    for (; it != end; ++it) {
      if (*it == name) {
        *it = nullptr;
      }
    }
  }

  void transCaret2(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    auto a = transItems(ctx, defs, it, end);
    if (a.empty()) [[unlikely]] {
      dest = fa1se(pos);
      return;
    }
    if (a.size() == 1) {
      a.push_back(fa1se());
    }
    dest.reset(new LogicXorOper(pos, std::move(a)));
  }

  void transCaret(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    auto a = transItems(ctx, defs, it, end);
    if (a.empty()) [[unlikely]] {
      dest = number(0, pos);
      return;
    }
    if (a.size() == 1) {
      a.push_back(number(0));
    }
    dest.reset(new BitwsXorOper(pos, std::move(a)));
  }

  void transVertical2(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    auto a = transItems(ctx, defs, it, end);
    if (a.empty()) [[unlikely]] {
      dest = nvll(pos);
      return;
    }
    if (a.size() == 1) [[unlikely]] {
      dest = std::move(a.back());
      return;
    }
    dest.reset(new LogicOrOper(pos, std::move(a)));
  }

  void transVertical(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    auto a = transItems(ctx, defs, it, end);
    if (a.empty()) [[unlikely]] {
      dest = number(0, pos);
      return;
    }
    if (a.size() == 1) {
      a.push_back(number(0));
    }
    dest.reset(new BitwsOrOper(pos, std::move(a)));
  }

  void transTilde(UNode &dest, Context &ctx, Defs &defs, const Pos *pos, It it, It end) {
    auto a = transItem(ctx, defs, it, end);
    dest.reset(new BitwsNotOper(pos, std::move(a)));
  }
}
