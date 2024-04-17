#include<sstream>
#include"mylisp/mylisp.hh"
#include"mylisp/opcode.hh"
#include"myutils/xyz.hh"
#include"nodes2.hh"

using namespace std;

namespace zlt::mylispc {
  namespace opcode = mylisp::opcode;

  struct Scope {
    const Function1::Defs &defs;
    const Function1::ClosureDefs &closureDefs;
    bool hasDefer;
    Scope(const Function1::Defs &defs, const Function1::ClosureDefs &closureDefs, bool hasDefer) noexcept:
    defs(defs), closureDefs(closureDefs), hasDefer(hasDefer) {}
  };

  using It = UNodes::const_iterator;

  static void compile(ostream &dest, const Scope &scope, const UNode &src);

  template<class It>
  static inline void compile(ostream &dest, const Scope &scope, It it, It end) {
    for (; it != end; ++it) {
      compile(dest, scope, *it);
    }
  }

  static inline void compile(string &dest, const Scope &scope, const UNode &src) {
    stringstream ss;
    compile(ss, scope, src);
    dest = ss.str();
  }

  template<class It>
  static inline void compile(string &dest, const Scope &scope, It it, It end) {
    stringstream ss;
    compile(ss, scope, it, end);
    dest = ss.str();
  }

  void compile(string &dest, It it, It end) {
    Function1::Defs _;
    Function1::ClosureDefs _1;
    Scope scope(_, _1, false);
    stringstream ss;
    compile(ss, scope, it, end);
    dest = ss.str();
  }

  #define declCompile(T) \
  static void compile(ostream &dest, const Scope &scope, const T &src)

  declCompile(Call);
  declCompile(Callee);
  declCompile(CharAtom);
  declCompile(Defer);
  declCompile(Forward);
  declCompile(Function1);
  declCompile(GetHighRef);
  declCompile(GetRef);
  declCompile(GlobalForward);
  declCompile(GlobalReturn);
  declCompile(If);
  declCompile(Null);
  declCompile(Number);
  declCompile(Return);
  declCompile(SetHighRef);
  declCompile(SetRef);
  declCompile(StringAtom);
  declCompile(Throw);
  declCompile(Try);
  declCompile(Yield);
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
    ifType(CharAtom);
    ifType(Defer);
    ifType(Forward);
    ifType(Function1);
    ifType(GetHighRef);
    ifType(GetRef);
    ifType(GlobalForward);
    ifType(GlobalReturn);
    ifType(If);
    ifType(Null);
    ifType(Number);
    ifType(Return);
    ifType(SetHighRef);
    ifType(SetRef);
    ifType(StringAtom);
    ifType(Throw);
    ifType(Try);
    ifType(Yield);
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

  template<class T>
  static inline void write(ostream &dest, const T &src) {
    dest.write((const char *) &src, sizeof(T));
  }

  static inline void writeSize(ostream &dest, size_t n) {
    write(dest, n);
  }

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
    writeSize(dest, src.args.size() + 1);
    dest.put(opcode::PUSH_PC_JMP);
    writeSize(dest, 1 + sizeof(size_t));
    dest.put(opcode::CALL);
    writeSize(dest, src.args.size());
  }

  void compile(ostream &dest, const Scope &scope, const Callee &src) {
    dest.put(opcode::GET_CALLEE);
  }

  void compile(ostream &dest, const Scope &scope, const CharAtom &src) {
    dest.put(opcode::CHAR_LITERAL);
    dest.put(src.value);
  }

  void compile(ostream &dest, const Scope &scope, const Defer &src) {
    compile(dest, scope, src.value);
    dest.put(opcode::PUSH_DEFER);
  }

  void compile(ostream &dest, const Scope &scope, const Forward &src) {
    compileCalling(dest, scope, src);
    dest.put(opcode::FORWARD);
    writeSize(dest, src.args.size());
    if (scope.hasDefer) {
      dest.put(opcode::PUSH_PC_JMP);
      writeSize(dest, 1);
      dest.put(opcode::CLEAN_FN_DEFERS);
    }
    dest.put(opcode::CALL);
    write(dest, src.args.size());
  }

  static void compileFnBody(string &dest, const Scope &scope, const Function1 &src);
  static void getRef(ostream &dest, const Scope &scope, const Reference &src);
  static size_t defIndex(const Scope &scope, const string *name) noexcept;
  static size_t closureDefIndex(const Scope &scope, const string *name) noexcept;

  void compile(ostream &dest, const Scope &scope, const Function1 &src) {
    string body;
    compileFnBody(body, scope, src);
    dest.put(opcode::MAKE_FN);
    writeSize(dest, src.paramn);
    writeSize(dest, src.defs.size());
    writeSize(dest, src.closureDefs.size());
    writeSize(dest, body.size());
    dest << body;
    if (src.closureDefs.empty()) {
      return;
    }
    dest.put(opcode::PUSH);
    for (auto [name, ref] : src.closureDefs) {
      getRef(dest, scope, ref);
      dest.put(opcode::SET_FN_CLOSURE);
      writeSize(dest, closureDefIndex(scope, name));
    }
    dest.put(opcode::POP);
  }

  void compileFnBody(string &dest, const Scope &scope, const Function1 &src) {
    stringstream ss;
    for (auto name : src.highDefs) {
      ss.put(opcode::WRAP_HIGH_REF);
      writeSize(ss, defIndex(scope, name));
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
      writeSize(dest, closureDefIndex(scope, src.name));
    } else if (src.scope == Reference::GLOBAL_SCOPE) {
      dest.put(opcode::GET_GLOBAL);
      write(dest, src.name);
    } else {
      dest.put(opcode::GET_LOCAL);
      writeSize(dest, defIndex(scope, src.name));
    }
  }

  void compile(ostream &dest, const Scope &scope, const GetHighRef &src) {
    getRef(dest, scope, src.ref);
    dest.put(opcode::GET_HIGH_REF);
  }

  void compile(ostream &dest, const Scope &scope, const GetRef &src) {
    getRef(dest, scope, src.ref);
  }

  void compile(ostream &dest, const Scope &scope, const GlobalForward &src) {
    compileCalling(dest, scope, src);
    dest.put(opcode::GLOBAL_FORWARD);
    writeSize(dest, src.args.size());
    dest.put(opcode::PUSH_PC_JMP);
    writeSize(dest, 1);
    dest.put(opcode::CLEAN_ALL_DEFERS);
    dest.put(opcode::CALL);
    writeSize(dest, src.args.size());
  }

  void compile(ostream &dest, const Scope &scope, const GlobalReturn &src) {
    compile(dest, scope, src.value);
    dest.put(opcode::PUSH_PC_JMP);
    writeSize(dest, 1);
    dest.put(opcode::CLEAN_ALL_DEFERS);
    dest.put(opcode::END);
  }

  void compile(ostream &dest, const Scope &scope, const If &src) {
    compile(dest, scope, src.cond);
    string then;
    compile(then, scope, src.then);
    string elze;
    compile(elze, scope, src.elze);
    dest.put(opcode::JIF);
    writeSize(dest, elze.size() + 1 + sizeof(size_t));
    dest << elze;
    dest.put(opcode::JMP);
    writeSize(dest, then.size());
    dest << then;
  }

  void compile(ostream &dest, const Scope &scope, const Null &src) {
    dest.put(opcode::NULL_LITERAL);
  }

  void compile(ostream &dest, const Scope &scope, const Number &src) {
    dest.put(opcode::NUM_LITERAL);
    write(dest, src.value);
  }

  void compile(ostream &dest, const Scope &scope, const Return &src) {
    compile(dest, scope, src.value);
    dest.put(opcode::POP_BP);
    dest.put(opcode::POP_SP);
    if (scope.hasDefer) {
      dest.put(opcode::PUSH);
      dest.put(opcode::PUSH_PC_JMP);
      writeSize(dest, 1);
      dest.put(opcode::CLEAN_FN_DEFERS);
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
      write(dest, src.ref.name);
    } else {
      dest.put(opcode::SET_LOCAL);
      writeSize(dest, defIndex(scope, src.ref.name));
    }
  }

  void compile(ostream &dest, const Scope &scope, const StringAtom &src) {
    dest.put(opcode::STRING_LITERAL);
    write(dest, src.value);
  }

  void compile(ostream &dest, const Scope &scope, const Throw &src) {
    compile(dest, scope, src.value);
    dest.put(opcode::THROW);
  }

  void compile(ostream &dest, const Scope &scope, const Try &src) {
    compileCalling(dest, scope, src);
    dest.put(opcode::PUSH_BP);
    dest.put(opcode::PUSH_SP_BACK);
    writeSize(dest, src.args.size() + 1);
    dest.put(opcode::PUSH_TRY);
    dest.put(opcode::PUSH_PC_JMP);
    write(dest, 3 + sizeof(size_t));
    dest.put(opcode::CALL);
    write(dest, src.args.size());
    dest.put(opcode::NULL_LITERAL);
    dest.put(opcode::THROW);
  }

  void compile(ostream &dest, const Scope &scope, const Yield &src) {
    dest.put(opcode::YIELD);
    compile(dest, scope, src.then);
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
    write(dest, 1 + sizeof(size_t));
    dest.put(opcode::JMP);
    write(dest, s.size());
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
    write(dest, s.size());
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
