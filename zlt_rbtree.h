#ifndef ZLT_RBTREE_H
#define ZLT_RBTREE_H

#include"zlt_bitree.h"

typedef struct zltRBTree zltRBTree;

struct zltRBTree {
  zltBiTree base;
  int red;
};

// most functions begin
static inline zltRBTree *zltRBTreeMostTop(ZLT_NN const zltRBTree *tree) {
  return (zltRBTree *) zltBiTreeMostTop((const zltBiTree *) tree);
}

static inline zltRBTree *zltRBTreeMostLeft(ZLT_NN const zltRBTree *tree) {
  return (zltRBTree *) zltBiTreeMostLeft((const zltBiTree *) tree);
}

static inline zltRBTree *zltRBTreeMostRight(ZLT_NN const zltRBTree *tree) {
  return (zltRBTree *) zltBiTreeMostRight((const zltBiTree *) tree);
}
// most functions end

// iterators begin
static inline zltRBTree *zltRBTreeLNR(ZLT_NN const zltRBTree *tree) {
  return (zltRBTree *) zltBiTreeLNR((const zltBiTree *) tree);
}

static inline zltRBTree *zltRBTreeLNR(ZLT_NN const zltRBTree *tree) {
  return (zltRBTree *) zltBiTreeLNR((const zltBiTree *) tree);
}

static inline zltRBTree *zltRBTreeLNR(ZLT_NN const zltRBTree *tree) {
  return (zltRBTree *) zltBiTreeLNR((const zltBiTree *) tree);
}

static inline zltRBTree *zltRBTreeLNR(ZLT_NN const zltRBTree *tree) {
  return (zltRBTree *) zltBiTreeLNR((const zltBiTree *) tree);
}

static inline zltRBTree *zltRBTreeLNR(ZLT_NN const zltRBTree *tree) {
  return (zltRBTree *) zltBiTreeLNR((const zltBiTree *) tree);
}

static inline zltRBTree *zltRBTreeLNR(ZLT_NN const zltRBTree *tree) {
  return (zltRBTree *) zltBiTreeLNR((const zltBiTree *) tree);
}
// iterators end

#endif
