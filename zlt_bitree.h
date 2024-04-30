#ifndef ZLT_BITREE_H
#define ZLT_BITREE_H

#include"zlt_xyz.h"

typedef struct zltBiTree zltBiTree;

struct zltBiTree {
  zltBiTree *parent;
  zltBiTree *lchd;
  zltBiTree *rchd;
};

static inline void zltBiTreeMake(zltBiTree *dest, ZLT_NN zltBiTree *parent) {
  dest->parent = parent;
  dest->lchd = NULL;
  dest->rchd = NULL;
}

typedef void zltBiTreeDtor(void *tree);

void zltBiTreeClean(zltBiTree *root, zltBiTreeDtor *dtor);

// most functions begin
zltBiTree *zltBiTreeMostTop(ZLT_NN const zltBiTree *tree);
zltBiTree *zltBiTreeMostLeft(ZLT_NN const zltBiTree *tree);
zltBiTree *zltBiTreeMostRight(ZLT_NN const zltBiTree *tree);
// most functions end

// iterators begin
zltBiTree *zltBiTreeLNR(ZLT_NN const zltBiTree *tree);
zltBiTree *zltBiTreeRNL(ZLT_NN const zltBiTree *tree);
zltBiTree *zltBiTreeLRN(ZLT_NN const zltBiTree *tree);
zltBiTree *zltBiTreeRLN(ZLT_NN const zltBiTree *tree);
zltBiTree *zltBiTreeNLR(ZLT_NN const zltBiTree *tree);
zltBiTree *zltBiTreeNRL(ZLT_NN const zltBiTree *tree);
// iterators end

// rotations begin
/// @param pivot has right child
/// @return origin right child
zltBiTree *zltBiTreeLeftRotate(ZLT_NN zltBiTree *pivot);

/// @param pivot has left child
/// @return origin left child
zltBiTree *zltBiTreeRightRotate(ZLT_NN zltBiTree *pivot);
// rotations end

// find functions begin
typedef int zltBiTreeFinder(void *a, void *b);

zltBiTree *zltBiTreeFind(const zltBiTree *tree, ZLT_NN zltBiTreeFinder *finder, void *data);

/// @param[out] parent parent of inserted, required init by NULL
/// @return insert slot
zltBiTree **zltBiTreeFindForWrite(
  ZLT_NN zltBiTree **parent, ZLT_NN zltBiTree **tree, ZLT_NN zltBiTreeFinder *finder, void *data);
// find functions end

#endif
