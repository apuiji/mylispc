#ifndef ZLT_STRTREE_H
#define ZLT_STRTREE_H

#include"rbtree.h"
#include"string.h"

typedef struct {
  zltRBTree rbTree;
  zltString value;
} zltStrTree;

#define zltStrTreeMemb(p, m) zltMemb(p, zltStrTree, m)

static inline zltStrTree zltStrTreeMake(const void *parent, zltString value) {
  return (zltStrTree) { .rbTree = zltRBTreeMake(parent), .value = value };
}

int zltStrTreeCmpForFind(const void *data, const void *tree);

static inline void *zltStrTreeFind(const void *tree, zltString value) {
  return zltBiTreeFind(tree, zltStrTreeCmpForFind, &value);
}

static inline void **zltStrTreeFindForInsert(void **parent, void **tree, zltString value) {
  return zltBiTreeFindForInsert(parent, tree, zltStrTreeCmpForFind, &value);
}

#endif
