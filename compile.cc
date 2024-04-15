#include<sstream>
#include"mylisp/mylisp.hh"
#include"mylisp/opcode.hh"
#include"myutils/xyz.hh"
#include"nodes2.hh"

using namespace std;

namespace zlt::mylispc {
  namespace opcode = mylisp::opcode;

  using Defs = Function1::Defs;
  using ClosureDefs = Function1::ClosureDefs;
  using It = UNodes::const_iterator;

  static void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const UNode &src);

  template<class It>
  static inline void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, It it, It end) {
    for (; it != end; ++it) {
      compile(dest, defs, closureDefs, *it);
    }
  }

  void compile(string &dest, It it, It end) {
    stringstream ss;
    compile(ss, Defs(), ClosureDefs(), it, end);
    dest = ss.str();
  }

  #define declCompile(T) \
  static void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const T &src)

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

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const UNode &src) {
    #define ifType(T) \
    if (auto a = dynamic_cast<const T *>(src.get()); a) { \
      compile(dest, defs, closureDefs, *a); \
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

  static void compileCalling(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const Calling &src) {
    compile(dest, defs, closureDefs, src.callee);
    dest.put(opcode::PUSH);
    for (auto &a : src.args) {
      compile(dest, defs, closureDefs, a);
      dest.put(opcode::PUSH);
    }
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const Call &src) {
    compileCalling(dest, defs, closureDefs, src);
    dest.put(opcode::CALL);
    write(dest, src.args.size());
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const Callee &src) {
    dest.put(opcode::GET_CALLEE);
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const CharAtom &src) {
    dest.put(opcode::CHAR_LITERAL);
    dest.put(src.value);
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const Defer &src) {
    compile(dest, defs, closureDefs, src.value);
    dest.put(opcode::PUSH_DEFER);
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const Forward &src) {
    compileCalling(dest, defs, closureDefs, src);
    dest.put(opcode::CLEAN_FN_DEFERS);
    dest.put(opcode::FORWARD);
    write(dest, src.args.size());
  }

  static size_t defIndex(const Defs &defs, const string *name) noexcept;
  static size_t closureDefIndex(const ClosureDefs &closureDefs, const string *name) noexcept;
  static void getRef(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const Reference &src);

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const Function1 &src) {
    string body;
    {
      stringstream ss;
      for (auto name : src.highDefs) {
        ss.put(opcode::WRAP_HIGH_REF);
        write(ss, defIndex(src.defs, name));
      }
      compile(ss, src.defs, src.closureDefs, src.body.begin(), src.body.end());
      body = ss.str();
    }
    dest.put(opcode::MAKE_FN);
    write(dest, src.paramn);
    write(dest, src.defs.size());
    write(dest, src.closureDefs.size());
    write(dest, body.size());
    dest << body;
    if (src.closureDefs.empty()) {
      return;
    }
    dest.put(opcode::PUSH);
    for (auto [name, ref] : src.closureDefs) {
      getRef(dest, defs, closureDefs, ref);
      dest.put(opcode::SET_FN_CLOSURE);
      write(dest, closureDefIndex(src.closureDefs, name));
    }
    dest.put(opcode::POP);
  }

  size_t defIndex(const Defs &defs, const string *name) noexcept {
    size_t i = 0;
    for (auto def : defs) {
      if (name == def) {
        break;
      }
      ++i;
    }
    return i;
  }

  size_t closureDefIndex(const ClosureDefs &closureDefs, const string *name) noexcept {
    size_t i = 0;
    for (auto &p : closureDefs) {
      if (name == p.first) {
        break;
      }
      ++i;
    }
    return i;
  }

  void getRef(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const Reference &src) {
    if (src.scope == Reference::CLOSURE_SCOPE) {
      dest.put(opcode::GET_CLOSURE);
      write(dest, closureDefIndex(closureDefs, src.name));
    } else if (src.scope == Reference::GLOBAL_SCOPE) {
      dest.put(opcode::GET_GLOBAL);
      write(dest, src.name);
    } else {
      dest.put(opcode::GET_LOCAL);
      write(dest, defIndex(defs, src.name));
    }
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const GetHighRef &src) {
    getRef(dest, defs, closureDefs, src.ref);
    dest.put(opcode::GET_HIGH_REF);
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const GetRef &src) {
    getRef(dest, defs, closureDefs, src.ref);
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const GlobalForward &src) {
    compileCalling(dest, defs, closureDefs, src);
    dest.put(opcode::CLEAN_ALL_DEFERS);
    dest.put(opcode::GLOBAL_FORWARD);
    write(dest, src.args.size());
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const GlobalReturn &src) {
    compile(dest, defs, closureDefs, src.value);
    dest.put(opcode::CLEAN_ALL_DEFERS);
    dest.put(opcode::GLOBAL_RETURN);
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const If &src) {
    compile(dest, defs, closureDefs, src.cond);
    string then;
    {
      stringstream ss;
      compile(ss, defs, closureDefs, src.then);
      then = ss.str();
    }
    string elze;
    {
      stringstream ss;
      compile(ss, defs, closureDefs, src.elze);
      elze = ss.str();
    }
    dest.put(opcode::JIF);
    write(dest, elze.size() + 1 + sizeof(size_t));
    dest << elze;
    dest.put(opcode::JMP);
    write(dest, then.size());
    dest << then;
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const Null &src) {
    dest.put(opcode::NULL_LITERAL);
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const Number &src) {
    dest.put(opcode::NUM_LITERAL);
    write(dest, src.value);
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const Return &src) {
    compile(dest, defs, closureDefs, src.value);
    dest.put(opcode::CLEAN_FN_DEFERS);
    dest.put(opcode::RETURN);
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const SetHighRef &src) {
    getRef(dest, defs, closureDefs, src.ref);
    dest.put(opcode::PUSH);
    compile(dest, defs, closureDefs, src.value);
    dest.put(opcode::SET_HIGH_REF);
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const SetRef &src) {
    compile(dest, defs, closureDefs, src.value);
    if (src.ref.scope == Reference::GLOBAL_SCOPE) {
      dest.put(opcode::SET_GLOBAL);
      write(dest, src.ref.name);
    } else {
      dest.put(opcode::SET_LOCAL);
      write(dest, defIndex(defs, src.ref.name));
    }
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const StringAtom &src) {
    dest.put(opcode::STRING_LITERAL);
    write(dest, src.value);
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const Throw &src) {
    compile(dest, defs, closureDefs, src.value);
    dest.put(opcode::THROW);
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const Try &src) {
    compileCalling(dest, defs, closureDefs, src);
    dest.put(opcode::PUSH_TRY);
    dest.put(opcode::CALL);
    write(dest, src.args.size());
    dest.put(opcode::NULL_LITERAL);
    dest.put(opcode::THROW);
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const Yield &src) {
    dest.put(opcode::YIELD);
    compile(dest, defs, closureDefs, src.then);
  }

  // arithmetical operations begin
  static void arithMultiOper(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, int opcode, It it, It end) {
    compile(dest, defs, closureDefs, *it);
    if (!Dynamicastable<Number> {}(**it)) {
      dest.put(opcode::POSITIVE);
    }
    dest.put(opcode::PUSH);
    while (++it != end) {
      compile(dest, defs, closureDefs, *it);
      dest.put(opcode);
    }
    dest.put(opcode::POP);
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const ArithAddOper &src) {
    arithMultiOper(dest, defs, closureDefs, opcode::ADD, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const ArithSubOper &src) {
    arithMultiOper(dest, defs, closureDefs, opcode::SUB, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const ArithMulOper &src) {
    arithMultiOper(dest, defs, closureDefs, opcode::MUL, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const ArithDivOper &src) {
    arithMultiOper(dest, defs, closureDefs, opcode::DIV, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const ArithModOper &src) {
    arithMultiOper(dest, defs, closureDefs, opcode::MOD, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const ArithPowOper &src) {
    arithMultiOper(dest, defs, closureDefs, opcode::POW, src.items.begin(), src.items.end());
  }
  // arithmetical operations end

  // logical operations begin
  static void logicAnd(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, It it, It end);

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const LogicAndOper &src) {
    logicAnd(dest, defs, closureDefs, src.items.begin(), src.items.end());
  }

  void logicAnd(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, It it, It end) {
    compile(dest, defs, closureDefs, *it);
    if (++it == end) [[unlikely]] {
      return;
    }
    string s;
    {
      stringstream ss;
      logicAnd(ss, defs, closureDefs, it, end);
      s = ss.str();
    }
    dest.put(opcode::JIF);
    write(dest, 1 + sizeof(size_t));
    dest.put(opcode::JMP);
    write(dest, s.size());
    dest << s;
  }

  static void logicOr(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, It it, It end);

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const LogicOrOper &src) {
    logicOr(dest, defs, closureDefs, src.items.begin(), src.items.end());
  }

  void logicOr(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, It it, It end) {
    compile(dest, defs, closureDefs, *it);
    if (++it == end) [[unlikely]] {
      return;
    }
    string s;
    {
      stringstream ss;
      logicOr(ss, defs, closureDefs, it, end);
      s = ss.str();
    }
    dest.put(opcode::JIF);
    write(dest, s.size());
    dest << s;
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const LogicNotOper &src) {
    compile(dest, defs, closureDefs, src.item);
    dest.put(opcode::LOGIC_NOT);
  }

  static void multiOper(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, int opcode, It it, It end) {
    compile(dest, defs, closureDefs, *it);
    dest.put(opcode::PUSH);
    while (++it != end) {
      compile(dest, defs, closureDefs, *it);
      dest.put(opcode);
    }
    dest.put(opcode::POP);
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const LogicXorOper &src) {
    multiOper(dest, defs, closureDefs, opcode::LOGIC_XOR, src.items.begin(), src.items.end());
  }
  // logical operations end

  // bitwise operations begin
  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const BitwsAndOper &src) {
    arithMultiOper(dest, defs, closureDefs, opcode::BIT_AND, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const BitwsOrOper &src) {
    arithMultiOper(dest, defs, closureDefs, opcode::BIT_OR, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const BitwsNotOper &src) {
    compile(dest, defs, closureDefs, src.item);
    dest.put(opcode::BIT_NOT);
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const BitwsXorOper &src) {
    arithMultiOper(dest, defs, closureDefs, opcode::BIT_XOR, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const LshOper &src) {
    arithMultiOper(dest, defs, closureDefs, opcode::LSH, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const RshOper &src) {
    arithMultiOper(dest, defs, closureDefs, opcode::RSH, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const UshOper &src) {
    arithMultiOper(dest, defs, closureDefs, opcode::USH, src.items.begin(), src.items.end());
  }
  // bitwise operations end

  // compare operations begin
  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const CmpEqOper &src) {
    compile(dest, defs, closureDefs, src.items[0]);
    dest.put(opcode::PUSH);
    compile(dest, defs, closureDefs, src.items[1]);
    dest.put(opcode::EQ);
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const CmpLtOper &src) {
    compile(dest, defs, closureDefs, src.items[0]);
    dest.put(opcode::PUSH);
    compile(dest, defs, closureDefs, src.items[1]);
    dest.put(opcode::LT);
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const CmpGtOper &src) {
    compile(dest, defs, closureDefs, src.items[0]);
    dest.put(opcode::PUSH);
    compile(dest, defs, closureDefs, src.items[1]);
    dest.put(opcode::GT);
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const CmpLteqOper &src) {
    compile(dest, defs, closureDefs, src.items[0]);
    dest.put(opcode::PUSH);
    compile(dest, defs, closureDefs, src.items[1]);
    dest.put(opcode::LTEQ);
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const CmpGteqOper &src) {
    compile(dest, defs, closureDefs, src.items[0]);
    dest.put(opcode::PUSH);
    compile(dest, defs, closureDefs, src.items[1]);
    dest.put(opcode::GTEQ);
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const CompareOper &src) {
    compile(dest, defs, closureDefs, src.items[0]);
    dest.put(opcode::PUSH);
    compile(dest, defs, closureDefs, src.items[1]);
    dest.put(opcode::COMPARE);
  }
  // compare operations end

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const GetMembOper &src) {
    multiOper(dest, defs, closureDefs, opcode::GET_MEMB, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const LengthOper &src) {
    compile(dest, defs, closureDefs, src.item);
    dest.put(opcode::LENGTH);
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const NegativeOper &src) {
    compile(dest, defs, closureDefs, src.item);
    dest.put(opcode::NEGATIVE);
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const PositiveOper &src) {
    compile(dest, defs, closureDefs, src.item);
    dest.put(opcode::POSITIVE);
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const SequenceOper &src) {
    compile(dest, defs, closureDefs, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const Defs &defs, const ClosureDefs &closureDefs, const SetMembOper &src) {
    compile(dest, defs, closureDefs, src.items[0]);
    dest.put(opcode::PUSH);
    compile(dest, defs, closureDefs, src.items[1]);
    dest.put(opcode::PUSH);
    compile(dest, defs, closureDefs, src.items[2]);
    dest.put(opcode::SET_MEMB);
  }
  // operations end
}
