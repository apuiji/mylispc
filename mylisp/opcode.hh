#pragma once

namespace zlt::mylisp::opcode {
  enum {
    ADD,
    BIT_AND,
    BIT_NOT,
    BIT_OR,
    BIT_XOR,
    CALL,
    CATCH_NAT_FN,
    CHAR_LITERAL,
    CLEAN_FN_GUARDS,
    CLEAN_GUARDS,
    COMPARE,
    DIV,
    END,
    EQ,
    FORWARD,
    GET_CALLEE,
    GET_CLOSURE,
    GET_GLOBAL,
    GET_HIGH_REF,
    GET_LOCAL,
    GET_MEMB,
    GT,
    GTEQ,
    INC_FN_GUARD,
    JIF,
    JMP,
    JMP_TO,
    LENGTH,
    LOGIC_NOT,
    LOGIC_XOR,
    LSH,
    LT,
    LTEQ,
    MAKE_FN,
    MOD,
    MORE_FN_GUARD,
    MORE_GUARD,
    MUL,
    NEGATIVE,
    NULL_LITERAL,
    NUM_LITERAL,
    POP,
    POP_BP,
    POP_DEFER,
    POP_GUARD,
    POP_PC,
    POP_SP,
    POSITIVE,
    POW,
    PUSH,
    PUSH_BP,
    PUSH_DEFER,
    PUSH_GUARD,
    PUSH_PC_JMP,
    PUSH_SP_BACK,
    RSH,
    SET_FN_CLOSURE,
    SET_GLOBAL,
    SET_HIGH_REF,
    SET_LOCAL,
    SET_MEMB,
    STRING_LITERAL,
    SUB,
    THROW,
    USH,
    WRAP_HIGH_REF
  };
}
