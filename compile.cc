#include<sstream>
#include"mylisp.hh"
#include"mylisp/opcode.hh"
#include"myutils/xyz.hh"
#include"nodes2.hh"

using namespace std;

namespace zlt::mylispc {
  namespace opcode = mylisp::opcode;

  struct Scope {
    const Function1::Defs &defs;
    const Function1::ClosureDefs &closureDefs;
    bool hasGuard;
    Scope(const Function1::Defs &defs, const Function1::ClosureDefs &closureDefs, bool hasGuard) noexcept:
    defs(defs), closureDefs(closureDefs), hasGuard(hasGuard) {}
  };

  static void compile(ostream &dest, const Scope &scope, const UNode &src);

  using It = UNodes::const_iterator;

  static inline void compile(ostream &dest, const Scope &scope, It it, It end) {
    for (; it != end; ++it) {
      compile(dest, scope, *it);
    }
  }

  void compile(string &dest, const UNode &src) {
    // main function
    auto &mf = static_cast<const Function1 &>(*src);
    Scope scope(mf.defs, mf.closureDefs, mf.hasGuard);
    stringstream ss;
    compile(ss, scope, mf.body.begin(), mf.body.end());
    dest = ss.str();
  }

  static inline void compile(string &dest, const Scope &scope, const UNode &src) {
    stringstream ss;
    compile(ss, scope, src);
    dest = ss.str();
  }

  static inline void compile(string &dest, const Scope &scope, It it, It end) {
    stringstream ss;
    compile(ss, scope, it, end);
    dest = ss.str();
  }

  #define declCompile(T) \
  static void compile(ostream &dest, const Scope &scope, const T &src)

  declCompile(Call);
  declCompile(Callee);
  declCompile(Defer);
  declCompile(Forward);
  declCompile(Function1);
  declCompile(GetHighRef);
  declCompile(GetRef);
  declCompile(Guard);
  declCompile(If);
  declCompile(Null);
  declCompile(Number);
  declCompile(Return);
  declCompile(SetHighRef);
  declCompile(SetRef);
  declCompile(StringAtom);
  declCompile(Throw);
  declCompile(Try);
  // arithmetical operations begin
  declCompile(ArithAddOper);
  declCompile(ArithSubOper);
  declCompile(ArithMulOper);
  declCompile(ArithDivOper);
  declCompile(ArithModOper);
  declCompile(ArithPowOper);
  // arithmetical operations end
  // logical operations begin
  declCompile(LogicAndOper);
  declCompile(LogicOrOper);
  declCompile(LogicNotOper);
  declCompile(LogicXorOper);
  // logical operations end
  // bitwise operations begin
  declCompile(BitwsAndOper);
  declCompile(BitwsOrOper);
  declCompile(BitwsNotOper);
  declCompile(BitwsXorOper);
  declCompile(LshOper);
  declCompile(RshOper);
  declCompile(UshOper);
  // bitwise operations end
  // compare operations begin
  declCompile(CmpEqOper);
  declCompile(CmpLtOper);
  declCompile(CmpGtOper);
  declCompile(CmpLteqOper);
  declCompile(CmpGteqOper);
  declCompile(CompareOper);
  // compare operations end
  declCompile(GetMembOper);
  declCompile(LengthOper);
  declCompile(NegativeOper);
  declCompile(PositiveOper);
  declCompile(SequenceOper);
  declCompile(SetMembOper);
  // operations end

  #undef declCompile

  void compile(ostream &dest, const Scope &scope, const UNode &src) {
    #define ifType(T) \
    if (auto a = dynamic_cast<const T *>(src.get()); a) { \
      compile(dest, scope, *a); \
      return; \
    }
    ifType(Call);
    ifType(Callee);
    ifType(Defer);
    ifType(Forward);
    ifType(Function1);
    ifType(GetHighRef);
    ifType(GetRef);
    ifType(Guard);
    ifType(If);
    ifType(Null);
    ifType(Number);
    ifType(Return);
    ifType(SetHighRef);
    ifType(SetRef);
    ifType(StringAtom);
    ifType(Throw);
    ifType(Try);
    // arithmetical operations begin
    ifType(ArithAddOper);
    ifType(ArithSubOper);
    ifType(ArithMulOper);
    ifType(ArithDivOper);
    ifType(ArithModOper);
    ifType(ArithPowOper);
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
    // compare operations begin
    ifType(CmpEqOper);
    ifType(CmpLtOper);
    ifType(CmpGtOper);
    ifType(CmpLteqOper);
    ifType(CmpGteqOper);
    ifType(CompareOper);
    // compare operations end
    ifType(GetMembOper);
    ifType(LengthOper);
    ifType(NegativeOper);
    ifType(PositiveOper);
    ifType(SequenceOper);
    ifType(SetMembOper);
    // operations end
    #undef ifType
  }

  #define defWrite(name, T) \
  static inline void write_##name(ostream &dest, T t) { \
    dest.write((const char *) &t, sizeof(T)); \
  }

  #define defWrite1(T) defWrite(T, T)

  defWrite1(double);
  defWrite1(size_t);
  defWrite(ptr, void *);

  #undef defWrite1
  #undef defWrite

  static void compileCalling(ostream &dest, const Scope &scope, const Calling &src) {
    compile(dest, scope, src.callee);
    dest.put(opcode::PUSH);
    for (auto &a : src.args) {
      compile(dest, scope, a);
      dest.put(opcode::PUSH);
    }
  }

  void compile(ostream &dest, const Scope &scope, const Call &src) {
    compileCalling(dest, scope, src);
    dest.put(opcode::PUSH_BP);
    dest.put(opcode::PUSH_SP_BACK);
    write_size_t(dest, src.args.size() + 1);
    dest.put(opcode::PUSH_PC_JMP);
    write_size_t(dest, 1 + sizeof(size_t));
    dest.put(opcode::CALL);
    write_size_t(dest, src.args.size());
    dest.put(opcode::POP_SP);
    dest.put(opcode::POP_BP);
  }

  void compile(ostream &dest, const Scope &scope, const Callee &src) {
    dest.put(opcode::GET_CALLEE);
  }

  void compile(ostream &dest, const Scope &scope, const Defer &src) {
    compile(dest, scope, src.value);
    dest.put(opcode::PUSH_DEFER);
  }

  void compile(ostream &dest, const Scope &scope, const Forward &src) {
    compileCalling(dest, scope, src);
    dest.put(opcode::FORWARD);
    write_size_t(dest, src.args.size());
    if (scope.hasGuard) {
      dest.put(opcode::PUSH_PC_JMP);
      write_size_t(dest, 1);
      dest.put(opcode::CLEAN_FN_GUARDS);
    }
    dest.put(opcode::CALL);
    write_size_t(dest, src.args.size());
  }

  static void compileFnBody(string &dest, const Scope &scope, const Function1 &src);
  static void getRef(ostream &dest, const Scope &scope, const Reference &src);
  static size_t defIndex(const Scope &scope, const string *name) noexcept;
  static size_t closureDefIndex(const Scope &scope, const string *name) noexcept;

  void compile(ostream &dest, const Scope &scope, const Function1 &src) {
    string body;
    compileFnBody(body, scope, src);
    dest.put(opcode::MAKE_FN);
    write_size_t(dest, src.paramn);
    write_size_t(dest, src.defs.size());
    write_size_t(dest, src.closureDefs.size());
    write_size_t(dest, body.size());
    dest << body;
    if (src.closureDefs.empty()) {
      return;
    }
    dest.put(opcode::PUSH);
    for (auto [name, ref] : src.closureDefs) {
      getRef(dest, scope, ref);
      dest.put(opcode::SET_FN_CLOSURE);
      write_size_t(dest, closureDefIndex(scope, name));
    }
    dest.put(opcode::POP);
  }

  void compileFnBody(string &dest, const Scope &scope, const Function1 &src) {
    stringstream ss;
    for (auto name : src.highDefs) {
      ss.put(opcode::WRAP_HIGH_REF);
      write_size_t(ss, defIndex(scope, name));
    }
    compile(ss, scope, src.body.begin(), src.body.end());
    dest = ss.str();
  }

  size_t defIndex(const Scope &scope, const string *name) noexcept {
    size_t i = 0;
    for (auto def : scope.defs) {
      if (name == def) {
        break;
      }
      ++i;
    }
    return i;
  }

  size_t closureDefIndex(const Scope &scope, const string *name) noexcept {
    size_t i = 0;
    for (auto &p : scope.closureDefs) {
      if (name == p.first) {
        break;
      }
      ++i;
    }
    return i;
  }

  void getRef(ostream &dest, const Scope &scope, const Reference &src) {
    if (src.scope == Reference::CLOSURE_SCOPE) {
      dest.put(opcode::GET_CLOSURE);
      write_size_t(dest, closureDefIndex(scope, src.name));
    } else if (src.scope == Reference::GLOBAL_SCOPE) {
      dest.put(opcode::GET_GLOBAL);
      write_ptr(dest, src.name);
    } else {
      dest.put(opcode::GET_LOCAL);
      write_size_t(dest, defIndex(scope, src.name));
    }
  }

  void compile(ostream &dest, const Scope &scope, const GetHighRef &src) {
    getRef(dest, scope, src.ref);
    dest.put(opcode::GET_HIGH_REF);
  }

  void compile(ostream &dest, const Scope &scope, const GetRef &src) {
    getRef(dest, scope, src.ref);
  }

  void compile(ostream &dest, const Scope &scope, const Guard &src) {
    compile(dest, scope, src.value);
    dest.put(opcode::PUSH_GUARD);
    dest.put(opcode::INC_FN_GUARD);
  }

  void compile(ostream &dest, const Scope &scope, const If &src) {
    compile(dest, scope, src.cond);
    string then;
    compile(then, scope, src.then);
    string elze;
    compile(elze, scope, src.elze);
    dest.put(opcode::JIF);
    write_size_t(dest, elze.size() + 1 + sizeof(size_t));
    dest << elze;
    dest.put(opcode::JMP);
    write_size_t(dest, then.size());
    dest << then;
  }

  void compile(ostream &dest, const Scope &scope, const Null &src) {
    dest.put(opcode::NULL_LITERAL);
  }

  void compile(ostream &dest, const Scope &scope, const Number &src) {
    dest.put(opcode::NUM_LITERAL);
    write_double(dest, src.value);
  }

  void compile(ostream &dest, const Scope &scope, const Return &src) {
    compile(dest, scope, src.value);
    if (scope.hasGuard) {
      dest.put(opcode::PUSH);
      dest.put(opcode::PUSH_PC_JMP);
      write_size_t(dest, 1);
      dest.put(opcode::CLEAN_FN_GUARDS);
      dest.put(opcode::POP);
    }
    dest.put(opcode::POP_PC);
  }

  void compile(ostream &dest, const Scope &scope, const SetHighRef &src) {
    getRef(dest, scope, src.ref);
    dest.put(opcode::PUSH);
    compile(dest, scope, src.value);
    dest.put(opcode::SET_HIGH_REF);
  }

  void compile(ostream &dest, const Scope &scope, const SetRef &src) {
    compile(dest, scope, src.value);
    if (src.ref.scope == Reference::GLOBAL_SCOPE) {
      dest.put(opcode::SET_GLOBAL);
      write_ptr(dest, src.ref.name);
    } else {
      dest.put(opcode::SET_LOCAL);
      write_size_t(dest, defIndex(scope, src.ref.name));
    }
  }

  void compile(ostream &dest, const Scope &scope, const StringAtom &src) {
    dest.put(opcode::STRING_LITERAL);
    write_ptr(dest, src.value);
  }

  void compile(ostream &dest, const Scope &scope, const Throw &src) {
    compile(dest, scope, src.value);
    dest.put(opcode::PUSH);
    dest.put(opcode::CLEAN_GUARDS);
  }

  void compile(ostream &dest, const Scope &scope, const Try &src) {
    /// TODO:
    compileCalling(dest, scope, src);
    dest.put(opcode::PUSH_BP);
    dest.put(opcode::PUSH_SP_BACK);
    write_size_t(dest, src.args.size() + 1);
    dest.put(opcode::PUSH_PC_JMP);
    writeSize(dest, 4 + sizeof(size_t));
    dest.put(opcode::CALL);
    write_size_t(dest, src.args.size());
    dest.put(opcode::NULL_LITERAL);
    dest.put(opcode::PUSH);
    dest.put(opcode::CLEAN_GUARDS);
    dest.put(opcode::POP_SP);
    dest.put(opcode::POP_BP);
  }

  // arithmetical operations begin
  static void arithMultiOper(ostream &dest, const Scope &scope, int opcode, It it, It end) {
    compile(dest, scope, *it);
    if (!Dynamicastable<Number> {}(**it)) {
      dest.put(opcode::POSITIVE);
    }
    dest.put(opcode::PUSH);
    while (++it != end) {
      compile(dest, scope, *it);
      dest.put(opcode);
    }
    dest.put(opcode::POP);
  }

  void compile(ostream &dest, const Scope &scope, const ArithAddOper &src) {
    arithMultiOper(dest, scope, opcode::ADD, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const Scope &scope, const ArithSubOper &src) {
    arithMultiOper(dest, scope, opcode::SUB, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const Scope &scope, const ArithMulOper &src) {
    arithMultiOper(dest, scope, opcode::MUL, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const Scope &scope, const ArithDivOper &src) {
    arithMultiOper(dest, scope, opcode::DIV, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const Scope &scope, const ArithModOper &src) {
    arithMultiOper(dest, scope, opcode::MOD, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const Scope &scope, const ArithPowOper &src) {
    arithMultiOper(dest, scope, opcode::POW, src.items.begin(), src.items.end());
  }
  // arithmetical operations end

  // logical operations begin
  static void logicAnd(ostream &dest, const Scope &scope, It it, It end);

  void compile(ostream &dest, const Scope &scope, const LogicAndOper &src) {
    logicAnd(dest, scope, src.items.begin(), src.items.end());
  }

  void logicAnd(ostream &dest, const Scope &scope, It it, It end) {
    compile(dest, scope, *it);
    if (++it == end) [[unlikely]] {
      return;
    }
    string s;
    {
      stringstream ss;
      logicAnd(ss, scope, it, end);
      s = ss.str();
    }
    dest.put(opcode::JIF);
    write_size_t(dest, 1 + sizeof(size_t));
    dest.put(opcode::JMP);
    write_size_t(dest, s.size());
    dest << s;
  }

  static void logicOr(ostream &dest, const Scope &scope, It it, It end);

  void compile(ostream &dest, const Scope &scope, const LogicOrOper &src) {
    logicOr(dest, scope, src.items.begin(), src.items.end());
  }

  void logicOr(ostream &dest, const Scope &scope, It it, It end) {
    compile(dest, scope, *it);
    if (++it == end) [[unlikely]] {
      return;
    }
    string s;
    {
      stringstream ss;
      logicOr(ss, scope, it, end);
      s = ss.str();
    }
    dest.put(opcode::JIF);
    write_size_t(dest, s.size());
    dest << s;
  }

  void compile(ostream &dest, const Scope &scope, const LogicNotOper &src) {
    compile(dest, scope, src.item);
    dest.put(opcode::LOGIC_NOT);
  }

  static void multiOper(ostream &dest, const Scope &scope, int opcode, It it, It end) {
    compile(dest, scope, *it);
    dest.put(opcode::PUSH);
    while (++it != end) {
      compile(dest, scope, *it);
      dest.put(opcode);
    }
    dest.put(opcode::POP);
  }

  void compile(ostream &dest, const Scope &scope, const LogicXorOper &src) {
    multiOper(dest, scope, opcode::LOGIC_XOR, src.items.begin(), src.items.end());
  }
  // logical operations end

  // bitwise operations begin
  void compile(ostream &dest, const Scope &scope, const BitwsAndOper &src) {
    arithMultiOper(dest, scope, opcode::BIT_AND, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const Scope &scope, const BitwsOrOper &src) {
    arithMultiOper(dest, scope, opcode::BIT_OR, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const Scope &scope, const BitwsNotOper &src) {
    compile(dest, scope, src.item);
    dest.put(opcode::BIT_NOT);
  }

  void compile(ostream &dest, const Scope &scope, const BitwsXorOper &src) {
    arithMultiOper(dest, scope, opcode::BIT_XOR, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const Scope &scope, const LshOper &src) {
    arithMultiOper(dest, scope, opcode::LSH, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const Scope &scope, const RshOper &src) {
    arithMultiOper(dest, scope, opcode::RSH, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const Scope &scope, const UshOper &src) {
    arithMultiOper(dest, scope, opcode::USH, src.items.begin(), src.items.end());
  }
  // bitwise operations end

  // compare operations begin
  void compile(ostream &dest, const Scope &scope, const CmpEqOper &src) {
    compile(dest, scope, src.items[0]);
    dest.put(opcode::PUSH);
    compile(dest, scope, src.items[1]);
    dest.put(opcode::EQ);
  }

  void compile(ostream &dest, const Scope &scope, const CmpLtOper &src) {
    compile(dest, scope, src.items[0]);
    dest.put(opcode::PUSH);
    compile(dest, scope, src.items[1]);
    dest.put(opcode::LT);
  }

  void compile(ostream &dest, const Scope &scope, const CmpGtOper &src) {
    compile(dest, scope, src.items[0]);
    dest.put(opcode::PUSH);
    compile(dest, scope, src.items[1]);
    dest.put(opcode::GT);
  }

  void compile(ostream &dest, const Scope &scope, const CmpLteqOper &src) {
    compile(dest, scope, src.items[0]);
    dest.put(opcode::PUSH);
    compile(dest, scope, src.items[1]);
    dest.put(opcode::LTEQ);
  }

  void compile(ostream &dest, const Scope &scope, const CmpGteqOper &src) {
    compile(dest, scope, src.items[0]);
    dest.put(opcode::PUSH);
    compile(dest, scope, src.items[1]);
    dest.put(opcode::GTEQ);
  }

  void compile(ostream &dest, const Scope &scope, const CompareOper &src) {
    compile(dest, scope, src.items[0]);
    dest.put(opcode::PUSH);
    compile(dest, scope, src.items[1]);
    dest.put(opcode::COMPARE);
  }
  // compare operations end

  void compile(ostream &dest, const Scope &scope, const GetMembOper &src) {
    multiOper(dest, scope, opcode::GET_MEMB, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const Scope &scope, const LengthOper &src) {
    compile(dest, scope, src.item);
    dest.put(opcode::LENGTH);
  }

  void compile(ostream &dest, const Scope &scope, const NegativeOper &src) {
    compile(dest, scope, src.item);
    dest.put(opcode::NEGATIVE);
  }

  void compile(ostream &dest, const Scope &scope, const PositiveOper &src) {
    compile(dest, scope, src.item);
    dest.put(opcode::POSITIVE);
  }

  void compile(ostream &dest, const Scope &scope, const SequenceOper &src) {
    compile(dest, scope, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const Scope &scope, const SetMembOper &src) {
    compile(dest, scope, src.items[0]);
    dest.put(opcode::PUSH);
    compile(dest, scope, src.items[1]);
    dest.put(opcode::PUSH);
    compile(dest, scope, src.items[2]);
    dest.put(opcode::SET_MEMB);
  }
  // operations end
}
