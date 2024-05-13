#include<ctype.h>
#include<stdlib.h>
#include<string.h>
#include"parse.h"
#include"token.h"

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

static It lexerStr(mylispcLexerDest *dest, mylispcContext *ctx, It it, It end);

static It consumeRaw(It it, It end);

It mylispcLexer(mylispcLexerDest *dest, mylispcContext *ctx, It it, It end) {
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
    return lexerStr(dest, ctx, it, end);
  }
  It end1 = consumeRaw(it, end);
  if (end1 == it) {
    mylispcReportBad(ctx, MYLISPC_UNRECOGNIZED_SYMB_BAD);
    return NULL;
  }
  dest->token = mylispcTokenOfRaw(&dest->numVal, zltStrMakeBE(it, end1));
  return end1;
}

static It lexerStr1(char *dest, size_t *left, mylispcContext *ctx, int quot, It it, It end);

It lexerStr(mylispcLexerDest *dest, mylispcContext *ctx, It it, It end) {
  char *s = (char *) malloc(ctx->strLtrlSizeLimit);
  if (!s) {
    mylispcReportBad(ctx, MYLISPC_OOM_BAD);
    return NULL;
  }
  size_t left = ctx->strLtrlSizeLimit;
  It end1 = lexerStr1(s, &left, ctx, *it, it + 1, end);
  if (!end1) {
    free(s);
    return NULL;
  }
  s = (char *) realloc(s, ctx->strLtrlSizeLimit - left);
  dest->strVal = zltStrMake(s, ctx->strLtrlSizeLimit - left);
  dest->token = MYLISPC_STR_TOKEN;
  return end1;
}

static size_t esch(char *dest, It it, It end);

It lexerStr1(char *dest, size_t *left, mylispcContext *ctx, int quot, It it, It end) {
  if (it == end || *it == '\n') {
    mylispcReportBad(ctx, MYLISPC_UNTERMINATED_STR_BAD);
    return NULL;
  }
  if (!*left) {
    mylispcReportBad(ctx, MYLISPC_STR_LTRL_SIZE_OVER_LIMIT_BAD);
    return NULL;
  }
  if (*it == quot) {
    return it + 1;
  }
  if (*it == '\\') {
    size_t n = esch(dest, it + 1, end);
    --*left;
    return lexerStr1(dest + 1, left, ctx, quot, it + 1 + n, end);
  }
  *dest = *it;
  --*left;
  return lexerStr1(dest + 1, left, ctx, quot, it + 1, end);
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
  int a = zltIsBasedDigitChar(it[1], 16);
  if (a < 0) {
    goto A;
  }
  int b = zltIsBasedDigitChar(it[2], 16);
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
  while (it != end && !strchr("\"'();", *it) && !isspace(*it)) {
    ++it;
  }
  return it;
}
