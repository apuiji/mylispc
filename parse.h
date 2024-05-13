#ifndef MYLISPC_PARSE_H
#define MYLISPC_PARSE_H

#include"mylispc.h"

const char *mylispcHit(const char *it, const char *end);

typedef struct {
  int token;
  double numVal;
  zltString strVal;
} mylispcLexerDest;

/// @return null when bad
const char *mylispcLexer(mylispcLexerDest *dest, mylispcContext *ctx, const char *it, const char *end);

bool mylispcParse(void **dest, mylispcContext *ctx, const char *it, const char *end);

#endif
