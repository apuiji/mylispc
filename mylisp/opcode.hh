#pragma once

namespace zlt::mylisp::opcode {
  enum {
    ADD,
    BIT_AND,
    BIT_NOT,
    BIT_OR,
    BIT_XOR,
    CALL,
    CHAR_LITERAL,
    CLEAN_ALL_DEFERS,
    CLEAN_FN_DEFERS,
    COMPARE,
    DIV,
    EQ,
    FORWARD,
    GET_CALLEE,
    GET_CLOSURE,
    GET_GLOBAL,
    GET_HIGH_REF,
    GET_LOCAL,
    GET_MEMB,
    GLOBAL_FORWARD,
    GLOBAL_RETURN,
    GT,
    GTEQ,
    JIF,
    JMP,
    LENGTH,
    LOGIC_NOT,
    LOGIC_XOR,
    LSH,
    LT,
    LTEQ,
    MAKE_FN,
    MOD,
    MUL,
    NEGATIVE,
    NULL_LITERAL,
    NUM_LITERAL,
    POP,
    POSITIVE,
    POW,
    PUSH,
    PUSH_DEFER,
    PUSH_TRY,
    RETURN,
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
    WRAP_HIGH_REF,
    YIELD
  };
}
