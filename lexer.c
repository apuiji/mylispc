#include<ctype.h>
#include<stdlib.h>
#include<string.h>
#include"mylispc.h"

#define STR_BUF_SIZE 256

typedef const char *It;

static It lineComment(It it, It end);

It mylispcHit(It it, It end) {
  if (it == end) {
    return end;
  }
  if (isspace(*it)) {
    return mylispcHit(it + 1, end);
  }
  if (*it == ';') {
    return lineComment(it + 1, end);
  }
  return it;
}

It lineComment(It it, It end) {
  while (it != end && *it != '\n') {
    ++it;
  }
  return it;
}

static It lexerStr(mylispcLexerDest *dest, It it, It end);

static It consumeRaw(It it, It end);

It mylispcLexer(mylispcLexerDest *dest, It it, It end) {
  if (it == end) {
    dest->token = MYLISPC_EOF_TOKEN;
    return end;
  }
  if (*it == '\n') {
    dest->token = MYLISPC_EOL_TOKEN;
    return it + 1;
  }
  if (*it == '(') {
    dest->token = MYLISPC_LPAREN_TOKEN;
    return it + 1;
  }
  if (*it == ')') {
    dest->token = MYLISPC_RPAREN_TOKEN;
    return it + 1;
  }
  if (*it == '"' || *it == '\'') {
    return lexerStr(dest, it, end);
  }
  It end1 = consumeRaw(it, end);
  if (end1 == it) {
    mylispcBad = MYLISPC_UNRECOGNIZED_SYMB_BAD;
    return NULL;
  }
  mylispcLexerRaw(dest, zltStrMake(it, end1 - it));
  return end1;
}

static It lexerStr1(char *dest, size_t *left, int quot, It it, It end);

It lexerStr(It it, It end) {
  char *s = (char *) malloc(STR_BUF_SIZE);
  if (!s) {
    mylispcBad = MYLISPC_OOM_BAD;
    return NULL;
  }
  size_t left = STR_BUF_SIZE;
  It end1 = lexerStr1(s, &left, *it, it + 1, end);
  if (!end1) {
    free(s);
    return NULL;
  }
  s = (const char *) realloc(s, STR_BUF_SIZE - left);
  dest->strVal = zltStrMake(s, STR_BUF_SIZE - left);
  dest->token = MYLISPC_STR_TOKEN;
  return end1;
}

static size_t esch(char *dest, It it, It end);

It lexerStr1(char *dest, size_t *left, int quot, It it, It end) {
  if (it == end || *it == '\n') {
    mylispcBad = MYLISPC_UNTERMINATED_STR_BAD;
    return NULL;
  }
  if (!*left) {
    mylispcBad = MYLISPC_STR_TOO_LONG_BAD;
    return NULL;
  }
  if (*it == quot) {
    return it + 1;
  }
  if (*it == '\\') {
    size_t n = esch(dest, it + 1, end);
    --*left;
    return lexerStr1(dest + 1, left, quot, it + 1 + n, end);
  }
  *dest = *it;
  --*left;
  return lexerStr1(dest + 1, left, quot, it + 1, end);
}

static size_t esch8(char *dest, It it, It end, size_t left);
static size_t esch16(char *dest, It it, It end);

size_t esch(char *dest, It it, It end) {
  if (it == end || *it == '\n') {
    *dest = '\\';
    return 0;
  }
  if (*it == '"' || *it == '\'' || *it == '\\') {
    *dest = *it;
    return 1;
  }
  if (*it == 'n') {
    *dest = '\n';
    return 1;
  }
  if (*it == 'r') {
    *dest = '\r';
    return 1;
  }
  if (*it == 't') {
    *dest = '\t';
    return 1;
  }
  if (*it >= '0' && *it <= '3') {
    return esch8(dest, it, end, 3);
  }
  if (*it >= '4' && *it <= '7') {
    return esch8(dest, it, end, 2);
  }
  if (*it == 'x') {
    return esch16(dest, it, end);
  }
  *dest = '\\';
  return 0;
}

size_t esch8(char *dest, It it, It end, size_t left) {
  *dest = 0;
  size_t n = 0;
  for (; n < left && it != end && *it >= '0' && *it <= '7'; ++n, ++it) {
    *dest = (*dest << 3) | (*it - '0');
  }
  return n;
}

size_t esch16(char *dest, It it, It end) {
  if (end - it < 3) {
    goto A;
  }
  int a = zltIsHexDigitChar(it[1]);
  if (a < 0) {
    goto A;
  }
  int b = zltIsHexDigitChar(it[2]);
  if (b < 0) {
    goto A;
  }
  *dest = (a << 4) | b;
  return 3;
  A:
  *dest = '\\';
  return 0;
}

It consumeRaw(It it, It end) {
  while (it != end && !strspn("\"'();", *it) && !isspace(*it)) {
    ++it;
  }
  return it;
}

static bool isBaseInt(double *dest, zltString raw);

void mylispcLexerRaw(mylispcLexerDest *dest, zltString raw) {
  if (raw.size < 8) {
    #define ifSymbol(r, t) \
    if (raw.size == sizeof(r) && !strncmp(raw.data, r, sizeof(r))) { \
      dest->token = t; \
      return; \
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
  }
  if (isBaseInt(&dest->numVal, raw)) {
    dest->token = MYLISPC_NUM_TOKEN;
    return;
  }
  if (!zltStrToDouble(&dest->numVal, raw).size) {
    dest->token = MYLISPC_NUM_TOKEN;
    return;
  }
  return MYLISPC_ID_TOKEN;
}

static bool isBaseInt1(double *dest, It it, It end);
static bool isBaseInt2(unsigned long *dest, It it, It end);

bool isBaseInt(double *dest, It it, It end) {
  if (*it == '+') {
    return isBaseInt1(dest, it + 1, end);
  }
  if (*it == '-') {
    if (!isBaseInt1(dest, it + 1, end)) {
      return false;
    }
    *dest = -*dest;
    return true;
  }
  return isBaseInt1(dest, it, end);
}

bool isBaseInt1(double *dest, It it, It end) {
  unsigned long l = 0;
  if (!isBaseInt2(&l, it, end)) {
    return false;
  }
  *dest = l;
  return true;
}

bool isBaseInt2(unsigned long *dest, It it, It end) {
  if (!(end - it >= 3 && *it == '0')) {
    return false;
  }
  char c = it[1];
  size_t base;
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
  return !zltStrToUnsignedLong(dest, zltStrMake(it + 2, end - it - 2)).size;
}
