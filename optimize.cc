#include<cmath>
#include<functional>
#include"myutils/xyz.hh"
#include"nodes1.hh"

using namespace std;

namespace zlt::mylispc {
  using It = UNodes::iterator;

  #define declOptimize(T) \
  static void optimize(UNode &dest, T &src)

  declOptimize(Call);
  declOptimize(Defer);
  declOptimize(Forward);
  declOptimize(Function);
  declOptimize(Guard);
  declOptimize(If);
  declOptimize(Return);
  declOptimize(SetID);
  declOptimize(Throw);
  declOptimize(Try);
  // operations begin
  declOptimize(Operation<1>);
  template<int N>
  declOptimize(Operation<N>);
  // arithmetical operations begin
  declOptimize(ArithAddOper);
  declOptimize(ArithSubOper);
  declOptimize(ArithMulOper);
  declOptimize(ArithDivOper);
  // arithmetical operations end
  // logical operations begin
  declOptimize(LogicAndOper);
  declOptimize(LogicOrOper);
  declOptimize(LogicNotOper);
  declOptimize(LogicXorOper);
  // logical operations end
  // bitwise operations begin
  declOptimize(BitwsAndOper);
  declOptimize(BitwsOrOper);
  declOptimize(BitwsNotOper);
  declOptimize(BitwsXorOper);
  declOptimize(LshOper);
  declOptimize(RshOper);
  declOptimize(UshOper);
  // bitwise operations end
  declOptimize(LengthOper);
  declOptimize(NegativeOper);
  declOptimize(PositiveOper);
  declOptimize(SequenceOper);
  // operations end

  #undef declOptimize

  void optimize(UNode &src) {
    #define ifType(T) \
    if (auto a = dynamic_cast<T *>(src.get()); a) { \
      optimize(src, *a); \
      return; \
    }
    ifType(Call);
    ifType(Defer);
    ifType(Forward);
    ifType(Function);
    ifType(Guard);
    ifType(If);
    ifType(Return);
    ifType(SetID);
    ifType(Throw);
    ifType(Try);
    // operations begin
    // arithmetical operations begin
    ifType(ArithAddOper);
    ifType(ArithSubOper);
    ifType(ArithMulOper);
    ifType(ArithDivOper);
    // arithmetical operations end
    // logical operations begin
    ifType(LogicAndOper);
    ifType(LogicOrOper);
    ifType(LogicNotOper);
    ifType(LogicXorOper);
    // logical operations end
    // bitwise operations begin
    ifType(BitwsAndOper);
    ifType(BitwsOrOper);
    ifType(BitwsNotOper);
    ifType(BitwsXorOper);
    ifType(LshOper);
    ifType(RshOper);
    ifType(UshOper);
    // bitwise operations end
    ifType(LengthOper);
    ifType(NegativeOper);
    ifType(PositiveOper);
    ifType(SequenceOper);
    // operations end
    ifType(Operation<1>);
    ifType(Operation<2>);
    ifType(Operation<3>);
    ifType(Operation<-1>);
    #undef ifType
  }

  // aa begin
  static bool isBoolConst(bool &dest, const UNode &src) noexcept {
    if (Dynamicastable<Callee, CharAtom, Function, Number, StringAtom> {}(*src)) {
      dest = true;
      return true;
    }
    if (Dynamicastable<Null> {}(*src)) {
      dest = false;
      return true;
    }
    return false;
  }

  static bool isNumConst(double &dest, const UNode &src) noexcept {
    if (Dynamicastable<Callee, CharAtom, Function, Null, StringAtom> {}(*src)) {
      dest = NAN;
      return true;
    }
    if (auto a = dynamic_cast<const Number *>(src.get()); a) {
      dest = a->value;
      return true;
    }
    return false;
  }

  static bool isIntConst(int &dest, const UNode &src) noexcept {
    if (double d; isNumConst(d, src)) {
      dest = (int) d;
      return true;
    }
    return false;
  }
  // aa end

  void optimize(UNode &dest, Call &src) {
    optimize(src.callee);
    optimize(src.args.begin(), src.args.end());
  }

  void optimize(UNode &dest, Defer &src) {
    optimize(src.value);
  }

  void optimize(UNode &dest, Forward &src) {
    optimize(src.callee);
    optimize(src.args.begin(), src.args.end());
  }

  void optimize(UNode &dest, Function &src) {
    optimize(src.body.begin(), src.body.end());
    UNodes body;
    optimizeBody(body, src.body.begin(), src.body.end());
    src.body = std::move(body);
  }

  void optimize(UNode &dest, Guard &src) {
    optimize(src.value);
  }

  void optimize(UNode &dest, If &src) {
    optimize(src.cond);
    optimize(src.then);
    optimize(src.elze);
    if (bool b; isBoolConst(b, src.cond)) {
      dest = std::move(b ? src.then : src.elze);
      return;
    }
    if (auto a = dynamic_cast<LogicNotOper *>(src.cond.get()); a) {
      src.cond = std::move(a->item);
      swap(src.then, src.elze);
      return;
    }
  }

  void optimize(UNode &dest, Return &src) {
    optimize(src.value);
  }

  void optimize(UNode &dest, SetID &src) {
    optimize(src.value);
  }

  void optimize(UNode &dest, Throw &src) {
    optimize(src.value);
  }

  void optimize(UNode &dest, Try &src) {
    optimize(src.callee);
    optimize(src.args.begin(), src.args.end());
  }

  // operations begin
  void optimize(UNode &dest, Operation<1> &src) {
    optimize(src.item);
  }

  template<int N>
  void optimize(UNode &dest, Operation<N> &src) {
    optimize(src.items.begin(), src.items.end());
  }

  // arithmetical operations begin
  template<class T, class F, class G>
  void sum(UNodes &dest, T &t, It it, It end, F &&f, G &&g) {
    for (; it != end; ++it) {
      if (T t1; f(t1, *it)) {
        t = g(t, t1);
      } else {
        dest.push_back(std::move(*it));
      }
    }
  }

  void optimize(UNode &dest, ArithAddOper &src) {
    optimize(src.items.begin(), src.items.end());
    UNodes a;
    double b = 0;
    sum(a, b, src.items.begin(), src.items.end(), isNumConst, plus<double>());
    if (a.empty()) {
      dest = number(b, src.start);
      return;
    }
    if (a.size() != src.items.size()) {
      a.push_front(number(b));
    }
    src.items = std::move(a);
  }

  void optimize(UNode &dest, ArithSubOper &src) {
    optimize(src.items.begin(), src.items.end());
    UNodes a;
    double b = 0;
    sum(a, b, next(src.items.begin()), src.items.end(), isNumConst, plus<double>());
    if (double c; isNumConst(c, src.items.front())) {
      if (a.empty()) {
        dest = number(c - b, src.start);
        return;
      }
      a.push_front(number(c - b));
    } else {
      a.push_front(std::move(src.items.front()));
      if (a.size() != src.items.size()) {
        a.push_back(number(b));
      }
    }
    src.items = std::move(a);
  }

  void optimize(UNode &dest, ArithMulOper &src) {
    optimize(src.items.begin(), src.items.end());
    UNodes a;
    double b = 1;
    sum(a, b, src.items.begin(), src.items.end(), isNumConst, multiplies<double>());
    if (a.empty()) {
      dest = number(b, src.start);
      return;
    }
    if (a.size() != src.items.size()) {
      a.push_front(number(b));
    }
    src.items = std::move(a);
  }

  void optimize(UNode &dest, ArithDivOper &src) {
    optimize(src.items.begin(), src.items.end());
    UNodes a;
    double b = 0;
    sum(a, b, next(src.items.begin()), src.items.end(), isNumConst, multiplies<double>());
    if (double c; isNumConst(c, src.items.front())) {
      if (a.empty()) {
        dest = number(c / b, src.start);
        return;
      }
      a.push_front(number(c / b));
    } else {
      a.push_front(std::move(src.items.front()));
      if (a.size() != src.items.size()) {
        a.push_back(number(b));
      }
    }
    src.items = std::move(a);
  }
  // arithmetical operations end

  // logical operations begin
  static bool logicAnd(UNodes &dest, It it, It end);

  void optimize(UNode &dest, LogicAndOper &src) {
    optimize(src.items.begin(), src.items.end());
    UNodes a;
    if (logicAnd(a, src.items.begin(), prev(src.items.end()))) {
      a.push_back(std::move(src.items.back()));
    }
    if (a.size() == 1) {
      dest = std::move(a.front());
    } else {
      src.items = std::move(a);
    }
  }

  bool logicAnd(UNodes &dest, It it, It end) {
    for (; it != end; ++it) {
      if (bool b; isBoolConst(b, *it)) {
        if (!b) {
          dest.push_back(std::move(*it));
          return false;
        }
      } else {
        dest.push_back(std::move(*it));
      }
    }
    return true;
  }

  static bool logicOr(UNodes &dest, It it, It end);

  void optimize(UNode &dest, LogicOrOper &src) {
    optimize(src.items.begin(), src.items.end());
    UNodes a;
    if (logicOr(a, src.items.begin(), prev(src.items.end()))) {
      a.push_back(std::move(src.items.back()));
    }
    if (a.size() == 1) {
      dest = std::move(a.front());
    } else {
      src.items = std::move(a);
    }
  }

  bool logicOr(UNodes &dest, It it, It end) {
    for (; it != end; ++it) {
      if (bool b; isBoolConst(b, *it)) {
        if (b) {
          dest.push_back(std::move(*it));
          return false;
        }
      } else {
        dest.push_back(std::move(*it));
      }
    }
    return true;
  }

  void optimize(UNode &dest, LogicNotOper &src) {
    optimize(src.item);
    if (bool b; isBoolConst(b, src.item)) {
      dest = boo1(!b, src.start);
    }
  }

  void optimize(UNode &dest, LogicXorOper &src) {
    optimize(src.items.begin(), src.items.end());
    UNodes a;
    bool b = false;
    sum(a, b, src.items.begin(), src.items.end(), isBoolConst, [] (bool x, bool y) { return x ^ y; });
    if (a.empty()) {
      dest = boo1(b, src.start);
      return;
    }
    if (a.size() != src.items.size()) {
      a.push_back(boo1(b));
    }
    src.items = std::move(a);
  }
  // logical operations end

  // bitwise operations begin
  void optimize(UNode &dest, BitwsAndOper &src) {
    optimize(src.items.begin(), src.items.end());
    UNodes a;
    int b = 0;
    sum(a, b, src.items.begin(), src.items.end(), isIntConst, bit_and<int>());
    if (a.empty()) {
      dest = number(b, src.start);
      return;
    }
    if (a.size() != src.items.size()) {
      a.push_front(number(b));
    }
    src.items = std::move(a);
  }

  void optimize(UNode &dest, BitwsOrOper &src) {
    optimize(src.items.begin(), src.items.end());
    UNodes a;
    int b = 0;
    sum(a, b, src.items.begin(), src.items.end(), isIntConst, bit_or<int>());
    if (a.empty()) {
      dest = number(b, src.start);
      return;
    }
    if (a.size() != src.items.size()) {
      a.push_front(number(b));
    }
    src.items = std::move(a);
  }

  void optimize(UNode &dest, BitwsNotOper &src) {
    optimize(src.item);
    if (int i; isIntConst(i, src.item)) {
      dest = number(~i, src.start);
    }
  }

  void optimize(UNode &dest, BitwsXorOper &src) {
    optimize(src.items.begin(), src.items.end());
    UNodes a;
    int b = 0;
    sum(a, b, src.items.begin(), src.items.end(), isIntConst, bit_xor<int>());
    if (a.empty()) {
      dest = number(b, src.start);
      return;
    }
    if (a.size() != src.items.size()) {
      a.push_front(number(b));
    }
    src.items = std::move(a);
  }

  void optimize(UNode &dest, LshOper &src) {
    optimize(src.items.begin(), src.items.end());
    UNodes a;
    int b = 0;
    sum(a, b, next(src.items.begin()), src.items.end(), isIntConst, plus<int>());
    if (int c; isIntConst(c, src.items.front())) {
      if (a.empty()) {
        dest = number(c << b, src.start);
        return;
      }
      a.push_front(number(c << b));
    } else {
      a.push_front(std::move(src.items.front()));
      if (a.size() != src.items.size()) {
        a.push_back(number(b));
      }
    }
    src.items = std::move(a);
  }

  void optimize(UNode &dest, RshOper &src) {
    optimize(src.items.begin(), src.items.end());
    UNodes a;
    int b = 0;
    sum(a, b, next(src.items.begin()), src.items.end(), isIntConst, plus<int>());
    if (int c; isIntConst(c, src.items.front())) {
      if (a.empty()) {
        dest = number(c >> b, src.start);
        return;
      }
      a.push_front(number(c >> b));
    } else {
      a.push_front(std::move(src.items.front()));
      if (a.size() != src.items.size()) {
        a.push_back(number(b));
      }
    }
    src.items = std::move(a);
  }

  void optimize(UNode &dest, UshOper &src) {
    optimize(src.items.begin(), src.items.end());
    UNodes a;
    int b = 0;
    sum(a, b, next(src.items.begin()), src.items.end(), isIntConst, plus<int>());
    if (int c; isIntConst(c, src.items.front())) {
      if (a.empty()) {
        dest = number((unsigned) c >> b, src.start);
        return;
      }
      a.push_front(number((unsigned) c >> b));
    } else {
      a.push_front(std::move(src.items.front()));
      if (a.size() != src.items.size()) {
        a.push_back(number(b));
      }
    }
    src.items = std::move(a);
  }
  // bitwise operations end

  void optimize(UNode &dest, LengthOper &src) {
    optimize(src.item);
    if (Dynamicastable<CharAtom> {}(*src.item)) {
      dest = number(1, src.start);
    } else if (auto a = dynamic_cast<StringAtom *>(src.item.get()); a) {
      dest = number(a->value->size(), src.start);
    }
  }

  void optimize(UNode &dest, NegativeOper &src) {
    optimize(src.item);
    if (double d; isNumConst(d, src.item)) {
      dest = number(-d, src.start);
    }
  }

  void optimize(UNode &dest, PositiveOper &src) {
    optimize(src.item);
    if (double d; isNumConst(d, src.item)) {
      dest = number(d, src.start);
    }
  }

  void optimize(UNode &dest, SequenceOper &src) {
    optimize(src.items.begin(), src.items.end());
    UNodes a;
    optimizeBody(a, src.items.begin(), src.items.end());
    if (a.size() == 1) {
      dest = std::move(a.front());
    } else {
      src.items = std::move(a);
    }
  }
  // operations end
}
