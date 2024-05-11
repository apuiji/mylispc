#ifndef ZLT_RBTREE_H
#define ZLT_RBTREE_H

#include"bitree.h"

typedef struct {
  zltBiTree biTree;
  bool red;
} zltRBTree;

#define zltRBTreeMemb(p, m) zltMemb(p, zltRBTree, m)

// constructors and destructors begin
static inline zltRBTree zltRBTreeMake(const void *parent) {
  return (zltRBTree) { .biTree = zltBiTreeMake(parent), .red = true };
}

static inline void zltRBTreeSwap(void *a, void *b) {
  zltMemSwap(a, b, sizeof(zltRBTree));
}
// constructors and destructors end

void zltRBTreeAfterInsert(void **root, void *node);
void zltRBTreeBeforeErase(void **root, void *node);

#endif
