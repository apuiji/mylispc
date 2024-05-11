#include<string.h>
#include"token.h"

static bool isBaseInt(long *dest, zltString raw);

int mylispcTokenOfRaw(double *numVal, zltString raw) {
  {
    long l;
    if (isBaseInt(&l, raw)) {
      *numVal = l;
      return MYLISPC_NUM_TOKEN;
    }
  }
  if (!zltStrToDouble(numVal, raw).size) {
    return MYLISPC_NUM_TOKEN;
  }
  if (raw.size < 8) {
    return MYLISPC_ID_TOKEN;
  }
  #define ifSymbol(r, t) \
  if (raw.size == sizeof(r) && !strncmp(raw.data, r, sizeof(r))) { \
    return t; \
  }
  // keywords begin
  ifSymbol("callee", MYLISPC_CALLEE_TOKEN);
  ifSymbol("def", MYLISPC_DEF_TOKEN);
  ifSymbol("defer", MYLISPC_DEFER_TOKEN);
  ifSymbol("forward", MYLISPC_FORWARD_TOKEN);
  ifSymbol("guard", MYLISPC_GUARD_TOKEN);
  ifSymbol("if", MYLISPC_IF_TOKEN);
  ifSymbol("length", MYLISPC_LENGTH_TOKEN);
  ifSymbol("return", MYLISPC_RETURN_TOKEN);
  ifSymbol("throw", MYLISPC_THROW_TOKEN);
  ifSymbol("try", MYLISPC_TRY_TOKEN);
  // keywords end
  // preproc operations begin
  ifSymbol("#", MYLISPC_POUND_TOKEN);
  ifSymbol("##", MYLISPC_POUND2_TOKEN);
  ifSymbol("#def", MYLISPC_POUND_DEF_TOKEN);
  ifSymbol("#if", MYLISPC_POUND_IF_TOKEN);
  ifSymbol("#include", MYLISPC_POUND_INCLUDE_TOKEN);
  ifSymbol("#line", MYLISPC_POUND_LINE_TOKEN);
  ifSymbol("#move", MYLISPC_POUND_MOVE_TOKEN);
  ifSymbol("#pop", MYLISPC_POUND_POP_TOKEN);
  ifSymbol("#push", MYLISPC_POUND_PUSH_TOKEN);
  ifSymbol("#undef", MYLISPC_POUND_UNDEF_TOKEN);
  // preproc operations end
  // symbols begin
  ifSymbol("!", MYLISPC_EXCLAM_TOKEN);
  ifSymbol("%", MYLISPC_PERCENT_TOKEN);
  ifSymbol("&&", MYLISPC_AMP2_TOKEN);
  ifSymbol("&", MYLISPC_AMP_TOKEN);
  ifSymbol("(", MYLISPC_LPAREN_TOKEN);
  ifSymbol(")", MYLISPC_RPAREN_TOKEN);
  ifSymbol("**", MYLISPC_ASTERISK2_TOKEN);
  ifSymbol("*", MYLISPC_ASTERISK_TOKEN);
  ifSymbol("+", MYLISPC_PLUS_TOKEN);
  ifSymbol(",", MYLISPC_COMMA_TOKEN);
  ifSymbol("-", MYLISPC_MINUS_TOKEN);
  ifSymbol(".", MYLISPC_DOT_TOKEN);
  ifSymbol("/", MYLISPC_SLASH_TOKEN);
  ifSymbol("<<", MYLISPC_LT2_TOKEN);
  ifSymbol("<=>", MYLISPC_LT_EQ_GT_TOKEN);
  ifSymbol("<=", MYLISPC_LT_EQ_TOKEN);
  ifSymbol("<", MYLISPC_LT_TOKEN);
  ifSymbol("==", MYLISPC_EQ2_TOKEN);
  ifSymbol("=", MYLISPC_EQ_TOKEN);
  ifSymbol(">=", MYLISPC_GT_EQ_TOKEN);
  ifSymbol(">>>", MYLISPC_GT3_TOKEN);
  ifSymbol(">>", MYLISPC_GT2_TOKEN);
  ifSymbol(">", MYLISPC_GT_TOKEN);
  ifSymbol("@", MYLISPC_AT_TOKEN);
  ifSymbol("^^", MYLISPC_CARET2_TOKEN);
  ifSymbol("^", MYLISPC_CARET_TOKEN);
  ifSymbol("||", MYLISPC_VERTICAL2_TOKEN);
  ifSymbol("|", MYLISPC_VERTICAL_TOKEN);
  ifSymbol("~", MYLISPC_TILDE_TOKEN);
  // symbols end
  #undef ifSymbol
  return MYLISPC_ID_TOKEN;
}

static bool isBaseInt1(unsigned long *dest, zltString raw);

bool isBaseInt(long *dest, zltString raw) {
  if (*raw.data == '+') {
    return isBaseInt1((unsigned long *) dest, zltStrForward(raw, 1));
  }
  if (*raw.data == '-') {
    if (!isBaseInt1((unsigned long *) dest, zltStrForward(raw, 1))) {
      return false;
    }
    *dest = -*dest;
    return true;
  }
  return isBaseInt1(dest, raw);
}

bool isBaseInt1(unsigned long *dest, zltString raw) {
  if (!(raw.size >= 3 && *raw.data == '0')) {
    return false;
  }
  size_t base;
  char c = raw.data[1];
  if (c == 'B' || c == 'b') {
    base = 2;
  } else if (c == 'Q' || c == 'q') {
    base = 4;
  } else if (c == 'O' || c == 'o') {
    base = 8;
  } else if (c == 'X' || c == 'x') {
    base = 16;
  } else {
    return false;
  }
  return !zltStrToUnsignedLong(dest, zltStrForward(raw, 2), base).size;
}

zltString mylispcRawOfToken(int token) {
  #define ifToken(t, r) \
  if (token == t) { \
    return zltStrMakeStatic(r); \
  }
  // keywords begin
  ifToken(MYLISPC_CALLEE_TOKEN, "callee");
  ifToken(MYLISPC_DEF_TOKEN, "def");
  ifToken(MYLISPC_DEFER_TOKEN, "defer");
  ifToken(MYLISPC_FORWARD_TOKEN, "forward");
  ifToken(MYLISPC_GUARD_TOKEN, "guard");
  ifToken(MYLISPC_IF_TOKEN, "if");
  ifToken(MYLISPC_LENGTH_TOKEN, "length");
  ifToken(MYLISPC_RETURN_TOKEN, "return");
  ifToken(MYLISPC_THROW_TOKEN, "throw");
  ifToken(MYLISPC_TRY_TOKEN, "try");
  // keywords end
  // preproc operations begin
  ifToken(MYLISPC_POUND_TOKEN, "#");
  ifToken(MYLISPC_POUND2_TOKEN, "##");
  ifToken(MYLISPC_POUND_DEF_TOKEN, "#def");
  ifToken(MYLISPC_POUND_IF_TOKEN, "#if");
  ifToken(MYLISPC_POUND_INCLUDE_TOKEN, "#include");
  ifToken(MYLISPC_POUND_LINE_TOKEN, "#line");
  ifToken(MYLISPC_POUND_MOVE_TOKEN, "#move");
  ifToken(MYLISPC_POUND_POP_TOKEN, "#pop");
  ifToken(MYLISPC_POUND_PUSH_TOKEN, "#push");
  ifToken(MYLISPC_POUND_UNDEF_TOKEN, "#undef");
  // preproc operations end
  // symbols begin
  ifToken(MYLISPC_EXCLAM_TOKEN, "!");
  ifToken(MYLISPC_PERCENT_TOKEN, "%");
  ifToken(MYLISPC_AMP2_TOKEN, "&&");
  ifToken(MYLISPC_AMP_TOKEN, "&");
  ifToken(MYLISPC_LPAREN_TOKEN, "(");
  ifToken(MYLISPC_RPAREN_TOKEN, ")");
  ifToken(MYLISPC_ASTERISK2_TOKEN, "**");
  ifToken(MYLISPC_ASTERISK_TOKEN, "*");
  ifToken(MYLISPC_PLUS_TOKEN, "+");
  ifToken(MYLISPC_COMMA_TOKEN, ",");
  ifToken(MYLISPC_MINUS_TOKEN, "-");
  ifToken(MYLISPC_DOT_TOKEN, ".");
  ifToken(MYLISPC_SLASH_TOKEN, "/");
  ifToken(MYLISPC_LT2_TOKEN, "<<");
  ifToken(MYLISPC_LT_EQ_GT_TOKEN, "<=>");
  ifToken(MYLISPC_LT_EQ_TOKEN, "<=");
  ifToken(MYLISPC_LT_TOKEN, "<");
  ifToken(MYLISPC_EQ2_TOKEN, "==");
  ifToken(MYLISPC_EQ_TOKEN, "=");
  ifToken(MYLISPC_GT_EQ_TOKEN, ">=");
  ifToken(MYLISPC_GT3_TOKEN, ">>>");
  ifToken(MYLISPC_GT2_TOKEN, ">>");
  ifToken(MYLISPC_GT_TOKEN, ">");
  ifToken(MYLISPC_AT_TOKEN, "@");
  ifToken(MYLISPC_CARET2_TOKEN, "^^");
  ifToken(MYLISPC_CARET_TOKEN, "^");
  ifToken(MYLISPC_VERTICAL2_TOKEN, "||");
  ifToken(MYLISPC_VERTICAL_TOKEN, "|");
  ifToken(MYLISPC_TILDE_TOKEN, "~");
  // symbols end
  #undef ifToken
  return (zltString) {};
}
