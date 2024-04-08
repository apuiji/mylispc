#include"nodes2.hh"

using namespace std;

namespace zlt::mylispc {
  using Defs = Function::Defs;
  using It = UNodes::iterator;

  static void trans(const Defs &highDefs, UNode &src);

  template<class It>
  static inline void trans(const Defs &highDefs, It it, It end) {
    for (; it != end; ++it) {
      trans(highDefs, *it);
    }
  }

  void trans2(It it, It end) {
    trans(Defs(), it, end);
  }

  #define declTrans(T) \
  static void trans(UNode &dest, const Defs &highDefs, T &src)

  declTrans(Call);
  declTrans(Defer);
  declTrans(Forward);
  declTrans(Function1);
  declTrans(GetRef);
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

  void trans(const Defs &highDefs, UNode &src) {
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

  void trans(UNode &dest, const Defs &highDefs, Call &src) {
    trans(highDefs, src.callee);
    trans(highDefs, src.args.begin(), src.args.end());
  }

  void trans(UNode &dest, const Defs &highDefs, Defer &src) {
    trans(highDefs, src.value);
  }

  void trans(UNode &dest, const Defs &highDefs, Forward &src) {
    trans(highDefs, src.callee);
    trans(highDefs, src.args.begin(), src.args.end());
  }

  void trans(UNode &dest, const Defs &highDefs, Function1 &src) {
    UNodes body;
    for (auto name : src.highDefs) {
      Reference ref(Reference::LOCAL_SCOPE, name);
      UNode value(new MakeHighRef);
      body.push_back({});
      body.back().reset(new SetRef(nullptr, ref, std::move(value)));
    }
    trans(src.highDefs, src.body.begin(), src.body.end());
    body.insert(body.end(), move_iterator(src.body.begin()), move_iterator(src.body.end()));
    src.body = std::move(body);
  }

  static bool isHighDef(const Defs &highDefs, const Reference &ref) noexcept;

  void trans(UNode &dest, const Defs &highDefs, GetRef &src) {
    if (isHighDef(highDefs, src.ref)) {
      dest.reset(new GetHighRef(src.start, src.ref));
    }
  }

  bool isHighDef(const Defs &highDefs, const Reference &ref) noexcept {
    if (ref.scope == Reference::CLOSURE_SCOPE) {
      return true;
    }
    if (ref.scope == Reference::LOCAL_SCOPE) {
      return highDefs.find(ref.name) != highDefs.end();
    }
    return false;
  }

  void trans(UNode &dest, const Defs &highDefs, If &src) {
    trans(highDefs, src.cond);
    trans(highDefs, src.then);
    trans(highDefs, src.elze);
  }

  void trans(UNode &dest, const Defs &highDefs, Return &src) {
    trans(highDefs, src.value);
  }

  void trans(UNode &dest, const Defs &highDefs, SetRef &src) {
    trans(highDefs, src.value);
    if (isHighDef(highDefs, src.ref)) {
      dest.reset(new SetHighRef(src.start, src.ref, std::move(src.value)));
    }
  }

  void trans(UNode &dest, const Defs &highDefs, Throw &src) {
    trans(highDefs, src.value);
  }

  void trans(UNode &dest, const Defs &highDefs, Try &src) {
    trans(highDefs, src.body.begin(), src.body.end());
  }

  void trans(UNode &dest, const Defs &highDefs, Yield &src) {
    trans(highDefs, src.then);
  }

  void trans(UNode &dest, const Defs &highDefs, Operation<1> &src) {
    trans(highDefs, src.item);
  }

  template<int N>
  void trans(UNode &dest, const Defs &highDefs, Operation<N> &src) {
    trans(highDefs, src.items.begin(), src.items.end());
  }
}
