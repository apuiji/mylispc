#ifndef ZLT_RBTREE_H
#define ZLT_RBTREE_H

#include"zlt_bitree.h"

typedef struct zltRBTree zltRBTree;

struct zltRBTree {
  zltRBTree *parent;
  zltRBTree *lchd;
  zltRBTree *rchd;
  bool red;
};

static inline void zltRBTreeMake(zltRBTree *dest, zltRBTree *parent) {
  zltBiTreeMake((zltBiTree *) dest, (zltBiTree *) parent);
  dest->red = true;
}

void zltRBTreeAfterInsert(zltRBTree **root, zltRBTree *node);
void zltRBTreeBeforeErase(zltRBTree **root, zltRBTree *node);

#endif
