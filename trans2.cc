#include"nodes2.hh"

using namespace std;

namespace zlt::mylispc {
  using HighDefs = Function1::HighDefs;
  using It = UNodes::iterator;

  static void trans(const HighDefs &highDefs, UNode &src);

  template<class It>
  static inline void trans(const HighDefs &highDefs, It it, It end) {
    for (; it != end; ++it) {
      trans(highDefs, *it);
    }
  }

  void trans2(It it, It end) {
    trans(HighDefs(), it, end);
  }

  #define declTrans(T) \
  static void trans(UNode &dest, const HighDefs &highDefs, T &src)

  declTrans(Call);
  declTrans(Defer);
  declTrans(Forward);
  declTrans(Function1);
  declTrans(GetRef);
  declTrans(GlobalForward);
  declTrans(GlobalReturn);
  declTrans(If);
  declTrans(Return);
  declTrans(SetRef);
  declTrans(Throw);
  declTrans(Try);
  declTrans(Yield);
  declTrans(Operation<1>);
  template<int N>
  declTrans(Operation<N>);

  #undef declTrans

  void trans(const HighDefs &highDefs, UNode &src) {
    #define ifType(T) \
    if (auto a = dynamic_cast<T *>(src.get()); a) { \
      trans(src, highDefs, *a); \
      return; \
    }
    ifType(Call);
    ifType(Defer);
    ifType(Forward);
    ifType(Function1);
    ifType(GetRef);
    ifType(GlobalForward);
    ifType(GlobalReturn);
    ifType(If);
    ifType(Return);
    ifType(SetRef);
    ifType(Throw);
    ifType(Try);
    ifType(Yield);
    ifType(Operation<1>);
    ifType(Operation<2>);
    ifType(Operation<3>);
    ifType(Operation<-1>);
    #undef ifType
  }

  void trans(UNode &dest, const HighDefs &highDefs, Call &src) {
    trans(highDefs, src.callee);
    trans(highDefs, src.args.begin(), src.args.end());
  }

  void trans(UNode &dest, const HighDefs &highDefs, Defer &src) {
    trans(highDefs, src.value);
  }

  void trans(UNode &dest, const HighDefs &highDefs, Forward &src) {
    trans(highDefs, src.callee);
    trans(highDefs, src.args.begin(), src.args.end());
  }

  void trans(UNode &dest, const HighDefs &highDefs, Function1 &src) {
    trans(src.highDefs, src.body.begin(), src.body.end());
  }

  static bool isHighDef(const HighDefs &highDefs, const Reference &ref) noexcept;

  void trans(UNode &dest, const HighDefs &highDefs, GetRef &src) {
    if (isHighDef(highDefs, src.ref)) {
      dest.reset(new GetHighRef(src.start, src.ref));
    }
  }

  bool isHighDef(const HighDefs &highDefs, const Reference &ref) noexcept {
    if (ref.scope == Reference::CLOSURE_SCOPE) {
      return true;
    }
    if (ref.scope == Reference::LOCAL_SCOPE) {
      return highDefs.find(ref.name) != highDefs.end();
    }
    return false;
  }

  void trans(UNode &dest, const HighDefs &highDefs, GlobalForward &src) {
    trans(highDefs, src.callee);
    trans(highDefs, src.args.begin(), src.args.end());
  }

  void trans(UNode &dest, const HighDefs &highDefs, GlobalReturn &src) {
    trans(highDefs, src.value);
  }

  void trans(UNode &dest, const HighDefs &highDefs, If &src) {
    trans(highDefs, src.cond);
    trans(highDefs, src.then);
    trans(highDefs, src.elze);
  }

  void trans(UNode &dest, const HighDefs &highDefs, Return &src) {
    trans(highDefs, src.value);
  }

  void trans(UNode &dest, const HighDefs &highDefs, SetRef &src) {
    trans(highDefs, src.value);
    if (isHighDef(highDefs, src.ref)) {
      dest.reset(new SetHighRef(src.start, src.ref, std::move(src.value)));
    }
  }

  void trans(UNode &dest, const HighDefs &highDefs, Throw &src) {
    trans(highDefs, src.value);
  }

  void trans(UNode &dest, const HighDefs &highDefs, Try &src) {
    trans(highDefs, src.callee);
    trans(highDefs, src.args.begin(), src.args.end());
  }

  void trans(UNode &dest, const HighDefs &highDefs, Yield &src) {
    trans(highDefs, src.then);
  }

  void trans(UNode &dest, const HighDefs &highDefs, Operation<1> &src) {
    trans(highDefs, src.item);
  }

  template<int N>
  void trans(UNode &dest, const HighDefs &highDefs, Operation<N> &src) {
    trans(highDefs, src.items.begin(), src.items.end());
  }
}
