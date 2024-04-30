#ifndef ZLT_BITREE_H
#define ZLT_BITREE_H

#include"zlt_xyz.h"

typedef struct zltBiTree zltBiTree;

struct zltBiTree {
  zltBiTree *parent;
  zltBiTree *lchd;
  zltBiTree *rchd;
};

static inline void zltBiTreeMake(zltBiTree *dest, zltBiTree *parent) {
  dest->parent = parent;
  dest->lchd = NULL;
  dest->rchd = NULL;
}

typedef void zltBiTreeDtor(void *tree);

void zltBiTreeClean(zltBiTree *root, zltBiTreeDtor *dtor);

// most functions begin
/// @param tree requires not null
zltBiTree *zltBiTreeMostTop(const zltBiTree *tree);

/// @param tree requires not null
zltBiTree *zltBiTreeMostLeft(const zltBiTree *tree);

/// @param tree requires not null
zltBiTree *zltBiTreeMostRight(const zltBiTree *tree);
// most functions end

// iterators begin
/// @param tree requires not null
zltBiTree *zltBiTreeLNR(const zltBiTree *tree);

/// @param tree requires not null
zltBiTree *zltBiTreeRNL(const zltBiTree *tree);

/// @param tree requires not null
zltBiTree *zltBiTreeLRN(const zltBiTree *tree);

/// @param tree requires not null
zltBiTree *zltBiTreeRLN(const zltBiTree *tree);

/// @param tree requires not null
zltBiTree *zltBiTreeNLR(const zltBiTree *tree);

/// @param tree requires not null
zltBiTree *zltBiTreeNRL(const zltBiTree *tree);
// iterators end

// rotations begin
/// @param pivot requires not null and has right child
/// @return origin right child
zltBiTree *zltBiTreeLeftRotate(zltBiTree *pivot);

/// @param pivot requires not null and has left child
/// @return origin left child
zltBiTree *zltBiTreeRightRotate(zltBiTree *pivot);
// rotations end

// find functions begin
typedef int zltBiTreeCmpForFind(void *a, void *b);

zltBiTree *zltBiTreeFind(const zltBiTree *tree, zltBiTreeCmpForFind *cmp, void *data);

/// @param[out] parent parent of inserted, requires not null and initializer by NULL
/// @param[out] tree requires not null
/// @return insert slot
zltBiTree **zltBiTreeFindForInsert(zltBiTree **parent, zltBiTree **tree, zltBiTreeCmpForFind *cmp, void *data);
// find functions end

#endif
