#ifndef MYLISPC_H
#define MYLISPC_H

#include"mylispx/pos.h"
#include"zlt/link.h"
#include"zlt/strtree.h"

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
