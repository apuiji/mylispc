#include<sstream>
#include"mylisp/mylisp.hh"
#include"mylisp/opcode.hh"
#include"nodes2.hh"

using namespace std;

namespace zlt::mylispc {
  namespace opcode = mylisp::opcode;

  using It = UNodes::const_iterator;

  static void compile(ostream &dest, const UNode &src);

  template<class It>
  static inline void compile(ostream &dest, It it, It end) {
    for (; it != end; ++it) {
      compile(dest, *it);
    }
  }

  void compile(string &dest, It it, It end) {
    stringstream ss;
    compile(ss, it, end);
    dest = ss.str();
  }

  #define declCompile(T) \
  static void compile(ostream &dest, const T &src)

  declCompile(Argument);
  declCompile(Call);
  declCompile(Callee);
  declCompile(CharAtom);
  declCompile(CleanArgs);
  declCompile(Defer);
  declCompile(Forward);
  declCompile(Function1);
  declCompile(GetHighRef);
  declCompile(GetRef);
  declCompile(GlobalForward);
  declCompile(GlobalReturn);
  declCompile(If);
  declCompile(MakeHighRef);
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

  void compile(ostream &dest, const UNode &src) {
    #define ifType(T) \
    if (auto a = dynamic_cast<const T *>(src.get()); a) { \
      compile(dest, *a); \
      return; \
    }
    ifType(Argument);
    ifType(Call);
    ifType(Callee);
    ifType(CharAtom);
    ifType(CleanArgs);
    ifType(Defer);
    ifType(Forward);
    ifType(Function1);
    ifType(GetHighRef);
    ifType(GetRef);
    ifType(GlobalForward);
    ifType(GlobalReturn);
    ifType(If);
    ifType(MakeHighRef);
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

  void compile(ostream &dest, const Argument &src) {
    dest.put(opcode::GET_ARG);
    write(dest, src.index);
  }

  static void compileCalling(ostream &dest, const Calling &src) {
    compile(dest, src.callee);
    dest.put(opcode::PUSH);
    for (auto &a : src.args) {
      compile(dest, a);
      dest.put(opcode::PUSH);
    }
  }

  void compile(ostream &dest, const Call &src) {
    compileCalling(dest, src);
    dest.put(opcode::CALL);
    write(dest, src.args.size());
  }

  void compile(ostream &dest, const Callee &src) {
    dest.put(opcode::GET_CALLEE);
  }

  void compile(ostream &dest, const CharAtom &src) {
    dest.put(opcode::CHAR_LITERAL);
    dest.put(src.value);
  }

  void compile(ostream &dest, const CleanArgs &src) {
    dest.put(opcode::CLEAN_ARGS);
  }

  void compile(ostream &dest, const Defer &src) {
    compile(dest, src.value);
    dest.put(opcode::PUSH_DEFER);
  }

  void compile(ostream &dest, const Forward &src) {
    compileCalling(dest, src);
    dest.put(opcode::CLEAN_FN_DEFERS);
    dest.put(opcode::FORWARD);
    write(dest, src.args.size());
  }

  static void getRef(ostream &dest, const Reference &src);

  void compile(ostream &dest, const Function1 &src) {
    string body;
    compile(body, src.body.begin(), src.body.end());
    auto it = mylisp::bodies.insert(std::move(body)).first;
    dest.put(opcode::MAKE_FN);
    write(dest, src.paramn);
    write(dest, &*it);
    if (src.closureDefs.empty()) {
      return;
    }
    dest.put(opcode::PUSH);
    for (auto [name, ref] : src.closureDefs) {
      getRef(dest, ref);
      dest.put(opcode::SET_FN_CLOSURE);
      write(dest, name);
    }
    dest.put(opcode::POP);
  }

  void getRef(ostream &dest, const Reference &src) {
    if (src.scope == Reference::CLOSURE_SCOPE) {
      dest.put(opcode::GET_CLOSURE);
    } else if (src.scope == Reference::GLOBAL_SCOPE) {
      dest.put(opcode::GET_GLOBAL);
    } else {
      dest.put(opcode::GET_LOCAL);
    }
    write(dest, src.name);
  }

  void compile(ostream &dest, const GetHighRef &src) {
    getRef(dest, src.ref);
    dest.put(opcode::GET_HIGH_REF);
  }

  void compile(ostream &dest, const GetRef &src) {
    getRef(dest, src.ref);
  }

  void compile(ostream &dest, const GlobalForward &src) {
    compileCalling(dest, src);
    dest.put(opcode::CLEAN_ALL_DEFERS);
    dest.put(opcode::GLOBAL_FORWARD);
    write(dest, src.args.size());
  }

  void compile(ostream &dest, const GlobalReturn &src) {
    compile(dest, src.value);
    dest.put(opcode::CLEAN_ALL_DEFERS);
    dest.put(opcode::GLOBAL_RETURN);
  }

  void compile(ostream &dest, const If &src) {
    compile(dest, src.cond);
    string then;
    {
      stringstream ss;
      compile(ss, src.then);
      then = ss.str();
    }
    string elze;
    {
      stringstream ss;
      compile(ss, src.elze);
      elze = ss.str();
    }
    dest.put(opcode::JIF);
    write(dest, elze.size() + 1 + sizeof(size_t));
    dest << elze;
    dest.put(opcode::JMP);
    write(dest, then.size());
    dest << then;
  }

  void compile(ostream &dest, const MakeHighRef &src) {
    dest.put(opcode::MAKE_HIGH_REF);
  }

  void compile(ostream &dest, const Null &src) {
    dest.put(opcode::NULL_LITERAL);
  }

  void compile(ostream &dest, const Number &src) {
    dest.put(opcode::NUM_LITERAL);
    write(dest, src.value);
  }

  void compile(ostream &dest, const Return &src) {
    compile(dest, src.value);
    dest.put(opcode::CLEAN_FN_DEFERS);
    dest.put(opcode::RETURN);
  }

  void compile(ostream &dest, const SetHighRef &src) {
    getRef(dest, src.ref);
    dest.put(opcode::PUSH);
    compile(dest, src.value);
    dest.put(opcode::SET_HIGH_REF);
  }

  void compile(ostream &dest, const SetRef &src) {
    compile(dest, src.value);
    if (src.ref.scope == Reference::GLOBAL_SCOPE) {
      dest.put(opcode::SET_GLOBAL);
    } else {
      dest.put(opcode::SET_LOCAL);
    }
    write(dest, src.ref.name);
  }

  void compile(ostream &dest, const StringAtom &src) {
    dest.put(opcode::STRING_LITERAL);
    write(dest, src.value);
  }

  void compile(ostream &dest, const Throw &src) {
    compile(dest, src.value);
    dest.put(opcode::THROW);
  }

  void compile(ostream &dest, const Try &src) {
    compileCalling(dest, src);
    dest.put(opcode::PUSH_TRY);
    dest.put(opcode::CALL);
    write(dest, src.args.size());
    dest.put(opcode::NULL_LITERAL);
    dest.put(opcode::THROW);
  }

  void compile(ostream &dest, const Yield &src) {
    dest.put(opcode::YIELD);
    compile(dest, src.then);
  }

  // arithmetical operations begin
  static void arithMultiOper(ostream &dest, int opcode, It it, It end) {
    compile(dest, *it);
    if (!Dynamicastable<Number> {}(**it)) {
      dest.put(opcode::POSITIVE);
    }
    ++it;
    do {
      dest.put(opcode::PUSH);
      compile(dest, *it);
      dest.put(opcode);
    } while (++it != end);
  }

  void compile(ostream &dest, const ArithAddOper &src) {
    arithMultiOper(dest, opcode::ADD, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const ArithSubOper &src) {
    arithMultiOper(dest, opcode::SUB, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const ArithMulOper &src) {
    arithMultiOper(dest, opcode::MUL, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const ArithDivOper &src) {
    arithMultiOper(dest, opcode::DIV, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const ArithModOper &src) {
    arithMultiOper(dest, opcode::MOD, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const ArithPowOper &src) {
    arithMultiOper(dest, opcode::POW, src.items.begin(), src.items.end());
  }
  // arithmetical operations end

  // logical operations begin
  static void logicAnd(ostream &dest, It it, It end);

  void compile(ostream &dest, const LogicAndOper &src) {
    logicAnd(dest, src.items.begin(), src.items.end());
  }

  void logicAnd(ostream &dest, It it, It end) {
    compile(dest, *it);
    if (++it == end) [[unlikely]] {
      return;
    }
    string s;
    {
      stringstream ss;
      logicAnd(ss, it, end);
      s = ss.str();
    }
    dest.put(opcode::JIF);
    write(dest, 1 + sizeof(size_t));
    dest.put(opcode::JMP);
    write(dest, s.size());
    dest << s;
  }

  static void logicOr(ostream &dest, It it, It end);

  void compile(ostream &dest, const LogicOrOper &src) {
    logicOr(dest, src.items.begin(), src.items.end());
  }

  void logicOr(ostream &dest, It it, It end) {
    compile(dest, *it);
    if (++it == end) [[unlikely]] {
      return;
    }
    string s;
    {
      stringstream ss;
      logicOr(ss, it, end);
      s = ss.str();
    }
    dest.put(opcode::JIF);
    write(dest, s.size());
    dest << s;
  }

  void compile(ostream &dest, const LogicNotOper &src) {
    compile(dest, src.item);
    dest.put(opcode::LOGIC_NOT);
  }

  static void multiOper(ostream &dest, int opcode, It it, It end) {
    compile(dest, *it);
    ++it;
    do {
      dest.put(opcode::PUSH);
      compile(dest, *it);
      dest.put(opcode);
    } while (++it != end);
  }

  void compile(ostream &dest, const LogicXorOper &src) {
    multiOper(dest, opcode::LOGIC_XOR, src.items.begin(), src.items.end());
  }
  // logical operations end

  // bitwise operations begin
  void compile(ostream &dest, const BitwsAndOper &src) {
    arithMultiOper(dest, opcode::BIT_AND, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const BitwsOrOper &src) {
    arithMultiOper(dest, opcode::BIT_OR, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const BitwsNotOper &src) {
    compile(dest, src.item);
    dest.put(opcode::BIT_NOT);
  }

  void compile(ostream &dest, const BitwsXorOper &src) {
    arithMultiOper(dest, opcode::BIT_XOR, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const LshOper &src) {
    arithMultiOper(dest, opcode::LSH, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const RshOper &src) {
    arithMultiOper(dest, opcode::RSH, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const UshOper &src) {
    arithMultiOper(dest, opcode::USH, src.items.begin(), src.items.end());
  }
  // bitwise operations end

  // compare operations begin
  void compile(ostream &dest, const CmpEqOper &src) {
    compile(dest, src.items[0]);
    dest.put(opcode::PUSH);
    compile(dest, src.items[1]);
    dest.put(opcode::EQ);
  }

  void compile(ostream &dest, const CmpLtOper &src) {
    compile(dest, src.items[0]);
    dest.put(opcode::PUSH);
    compile(dest, src.items[1]);
    dest.put(opcode::LT);
  }

  void compile(ostream &dest, const CmpGtOper &src) {
    compile(dest, src.items[0]);
    dest.put(opcode::PUSH);
    compile(dest, src.items[1]);
    dest.put(opcode::GT);
  }

  void compile(ostream &dest, const CmpLteqOper &src) {
    compile(dest, src.items[0]);
    dest.put(opcode::PUSH);
    compile(dest, src.items[1]);
    dest.put(opcode::LTEQ);
  }

  void compile(ostream &dest, const CmpGteqOper &src) {
    compile(dest, src.items[0]);
    dest.put(opcode::PUSH);
    compile(dest, src.items[1]);
    dest.put(opcode::GTEQ);
  }

  void compile(ostream &dest, const CompareOper &src) {
    compile(dest, src.items[0]);
    dest.put(opcode::PUSH);
    compile(dest, src.items[1]);
    dest.put(opcode::COMPARE);
  }
  // compare operations end

  void compile(ostream &dest, const GetMembOper &src) {
    multiOper(dest, opcode::GET_MEMB, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const LengthOper &src) {
    compile(dest, src.item);
    dest.put(opcode::LENGTH);
  }

  void compile(ostream &dest, const NegativeOper &src) {
    compile(dest, src.item);
    dest.put(opcode::NEGATIVE);
  }

  void compile(ostream &dest, const PositiveOper &src) {
    compile(dest, src.item);
    dest.put(opcode::POSITIVE);
  }

  void compile(ostream &dest, const SequenceOper &src) {
    compile(dest, src.items.begin(), src.items.end());
  }

  void compile(ostream &dest, const SetMembOper &src) {
    compile(dest, src.items[0]);
    dest.put(opcode::PUSH);
    compile(dest, src.items[1]);
    dest.put(opcode::PUSH);
    compile(dest, src.items[2]);
    dest.put(opcode::SET_MEMB);
  }
  // operations end
}
