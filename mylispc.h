#ifndef MYLISPC_H
#define MYLISPC_H

#include<stdio.h>
#include"mylispx/pos.h"
#include"zlt/link.h"
#include"zlt/ptrtree.h"
#include"zlt/strtree.h"

typedef struct {
  FILE *out;
  FILE *err;
  void *symbols;
  void *macros;
  mylispxPos pos;
  zltStack posk;
  bool ignoreWarns;
  size_t reportBadPoskLimit;
} mylispcContext;

void mylispcCtxClean(mylispcContext *ctx);

void mylispcReportBad(const mylispcCtx *ctx, int bad);

enum {
  MYLISPC_NO_BAD,
  MYLISPC_OOM_BAD,
  MYLISPC_STR_TOO_LONG_BAD,
  MYLISPC_UNEXPECTED_TOKEN_BAD,
  MYLISPC_UNTERMINATED_STR_BAD,
  MYLISPC_UNRECOGNIZED_SYMB_BAD
};

zltString mylispcCtxAddSymbol(mylispcContext *ctx, zltString symbol);

/// @return false when bad
bool mylispcCtxAddMacro(mylispcContext *ctx, const void *macro);

typedef struct {
  zltLink link;
  int clazz;
} mylispcNode;

#define mylispcNodeMemb(p, m) zltMemb(p, mylispcNode, m)

static inline mylispcNode mylispcNodeMake(int clazz) {
  return (mylispcNode) { .clazz = clazz };
}

void mylispcNodeDelete(void *node);

static inline void mylispcNodeClean(void *node, const void *end) {
  zltLinkClean(node, end, mylispcNodeDelete);
}

enum {
  // parse productions begin
  MYLISPC_EOL_ATOM_CLASS = 1,
  MYLISPC_ID_ATOM_CLASS,
  MYLISPC_LIST_ATOM_CLASS,
  MYLISPC_NUM_ATOM_CLASS,
  MYLISPC_STR_ATOM_CLASS,
  MYLISPC_TOKEN_ATOM_CLASS
  // parse productions end
};

#endif
