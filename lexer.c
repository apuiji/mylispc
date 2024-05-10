#include<ctype.h>
#include<stdlib.h>
#include<string.h>
#include"mylisp/mylisp.h"
#include"mylispc.h"
#include"token.h"

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
    mylispBad = MYLISPC_UNRECOGNIZED_SYMB_BAD;
    return NULL;
  }
  dest->token = mylispcTokenOfRaw(&dest->numVal, zltStrMakeBE(it, end1));
  return end1;
}

static It lexerStr1(char *dest, size_t *left, int quot, It it, It end);

It lexerStr(It it, It end) {
  char *s = (char *) malloc(STR_BUF_SIZE);
  if (!s) {
    mylispBad = MYLISP_OOM_BAD;
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
    mylispBad = MYLISPC_UNTERMINATED_STR_BAD;
    return NULL;
  }
  if (!*left) {
    mylispBad = MYLISPC_STR_TOO_LONG_BAD;
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
