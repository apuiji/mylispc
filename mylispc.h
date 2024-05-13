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
  // limit options begin
  size_t strLtrlSizeLimit;
  size_t reportBadPoskLimit;
  // limit options end
} mylispcContext;

void mylispcCtxClean(mylispcContext *ctx);

void mylispcReportBad(const mylispcContext *ctx, int bad);

enum {
  MYLISPC_NO_BAD,
  MYLISPC_INV_MACRO_ARG_BAD,
  MYLISPC_OOM_BAD,
  MYLISPC_MACRO_ALREADY_DEFINED_BAD,
  MYLISPC_POS_STACK_SIZE_OVER_LIMIT_BAD,
  MYLISPC_STR_LTRL_SIZE_OVER_LIMIT_BAD,
  MYLISPC_UNEXPECTED_TOKEN_BAD,
  MYLISPC_UNTERMINATED_STR_BAD,
  MYLISPC_UNRECOGNIZED_SYMB_BAD
};

/// if symbol already exists, delete param symbol data
/// @return null when bad
const zltString *mylispcCtxAddSymbol(mylispcContext *ctx, zltString symbol);

/// @return null when bad
const zltString *mylispcCtxAddSymbolClone(mylispcContext *ctx, zltString symbol);

/// @return false when bad
bool mylispcCtxAddMacro(mylispcContext *ctx, const zltString *name, const void *macro);

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
