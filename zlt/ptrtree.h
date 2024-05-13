#ifndef ZLT_PTRTREE_H
#define ZLT_PTRTREE_H

#include"rbtree.h"

typedef struct {
  zltRBTree rbTree;
  void *value;
} zltPtrTree;

#define zltPtrTreeMemb(p, m) zltMemb(p, zltPtrTree, m)

static inline zltPtrTree zltPtrTreeMake(const void *parent, const void *value) {
  return (zltPtrTree) { .rbTree = zltRBTreeMake(parent), .value = (void *) value };
}

int zltPtrTreeCmpForFind(const void *data, const void *tree);

static inline void *zltPtrTreeFind(const void *tree, const void *value) {
  return zltBiTreeFind(tree, zltPtrTreeCmpForFind, value);
}

static inline void **zltPtrTreeFindForInsert(void **parent, void **tree, const void *value) {
  return zltBiTreeFindForInsert(parent, tree, zltPtrTreeCmpForFind, value);
}

#endif
