#include"nodes2.hh"

using namespace std;

namespace zlt::mylispc {
  using Defs = set<const string *>;
  using It = UNodes::iterator;

  struct Scope {
    Scope *parent;
    const Defs &defs;
    Function1::HighDefs highDefs;
    Function1::ClosureDefs closureDefs;
    bool hasGuard;
    Scope(Scope *parent, const Defs &defs) noexcept: parent(parent), defs(defs) {}
  };

  static void trans(Scope &scope, UNode &src);

  template<class It>
  static inline void trans(Scope &scope, It it, It end) {
    for (; it != end; ++it) {
      trans(scope, *it);
    }
  }

  void trans1(UNode &dest, It it, It end) {
    Scope gs(nullptr, Defs());
    trans(gs, it, end);
    auto f = new Function1(nullptr);
    dest.reset(f);
    f->paramn = 0;
    f->hasGuard = gs.hasGuard;
    f->body = UNodes(move_iterator(it), move_iterator(end));
  }

  #define declTrans(T) \
  static void trans(UNode &dest, Scope &scope, T &src)

  declTrans(Call);
  declTrans(Defer);
  declTrans(Forward);
  declTrans(Function);
  declTrans(Guard);
  declTrans(ID);
  declTrans(If);
  declTrans(Return);
  declTrans(SetID);
  declTrans(Throw);
  declTrans(Try);
  declTrans(Operation<1>);
  template<int N>
  declTrans(Operation<N>);

  #undef declTrans

  void trans(Scope &scope, UNode &src) {
    #define ifType(T) \
    if (auto a = dynamic_cast<T *>(src.get()); a) { \
      trans(src, scope, *a); \
      return; \
    }
    ifType(Call);
    ifType(Defer);
    ifType(Forward);
    ifType(Function);
    ifType(Guard);
    ifType(ID);
    ifType(If);
    ifType(Return);
    ifType(SetID);
    ifType(Throw);
    ifType(Try);
    ifType(Operation<1>);
    ifType(Operation<2>);
    ifType(Operation<3>);
    ifType(Operation<-1>);
    #undef ifType
  }

  void trans(UNode &dest, Scope &scope, Call &src) {
    trans(scope, src.callee);
    trans(scope, src.args.begin(), src.args.end());
  }

  void trans(UNode &dest, Scope &scope, Defer &src) {
    trans(scope, src.value);
  }

  void trans(UNode &dest, Scope &scope, Forward &src) {
    trans(scope, src.callee);
    trans(scope, src.args.begin(), src.args.end());
  }

  void trans(UNode &dest, Scope &scope, Function &src) {
    Function1::Defs defs(src.params);
    for (auto &def : src.defs) {
      if (find(defs.begin(), defs.end(), def) == defs.end()) {
        defs.push_back(def);
      }
    }
    Scope fs(&scope, src.defs);
    trans(fs, src.body.begin(), src.body.end());
    auto f = new Function1(src.start);
    dest.reset(f);
    f->paramn = src.params.size();
    f->defs = std::move(defs);
    f->highDefs = std::move(fs.highDefs);
    f->closureDefs = std::move(fs.closureDefs);
    f->hasGuard = fs.hasGuard;
    f->body = std::move(src.body);
  }

  void trans(UNode &dest, Scope &scope, Guard &src) {
    scope.hasGuard = true;
    trans(scope, src.value);
  }

  static Reference findDef(Scope &scope, const string *name, bool local);

  void trans(UNode &dest, Scope &scope, ID &src) {
    auto ref = findDef(scope, src.name, true);
    dest.reset(new GetRef(src.start, ref));
  }

  Reference findDef(Scope &scope, const string *name, bool local) {
    if (!scope.parent) [[unlikely]] {
      return Reference(Reference::GLOBAL_SCOPE, name);
    }
    if (scope.defs.find(name) != scope.defs.end()) {
      if (!local) {
        scope.highDefs.insert(name);
      }
      return Reference(Reference::LOCAL_SCOPE, name);
    }
    if (auto it = scope.closureDefs.find(name); it != scope.closureDefs.end()) {
      return Reference(Reference::CLOSURE_SCOPE, name);
    }
    auto ref = findDef(*scope.parent, name, false);
    if (ref.scope == Reference::GLOBAL_SCOPE) {
      return ref;
    }
    scope.closureDefs[name] = ref;
    return Reference(Reference::CLOSURE_SCOPE, name);
  }

  void trans(UNode &dest, Scope &scope, If &src) {
    trans(scope, src.cond);
    trans(scope, src.then);
    trans(scope, src.elze);
  }

  void trans(UNode &dest, Scope &scope, Return &src) {
    trans(scope, src.value);
  }

  void trans(UNode &dest, Scope &scope, SetID &src) {
    auto ref = findDef(scope, src.name, true);
    trans(scope, src.value);
    dest.reset(new SetRef(src.start, ref, std::move(src.value)));
  }

  void trans(UNode &dest, Scope &scope, Throw &src) {
    trans(scope, src.value);
  }

  void trans(UNode &dest, Scope &scope, Try &src) {
    trans(scope, src.callee);
    trans(scope, src.args.begin(), src.args.end());
  }

  void trans(UNode &dest, Scope &scope, Operation<1> &src) {
    trans(scope, src.item);
  }

  template<int N>
  void trans(UNode &dest, Scope &scope, Operation<N> &src) {
    trans(scope, src.items.begin(), src.items.end());
  }
}
