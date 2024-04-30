#include"zlt_rbtree.h"

typedef zltBiTree BiTree;
typedef zltRBTree RBTree;

static inline RBTree *mostLeft(const RBTree *tree) {
  return (RBTree *) zltBiTreeMostLeft((const BiTree *) tree);
}

static inline RBTree *mostRight(const RBTree *tree) {
  return (RBTree *) zltBiTreeMostRight((const BiTree *) tree);
}

static inline RBTree *mostTop(const RBTree *tree) {
  return (RBTree *) zltBiTreeMostTop((const BiTree *) tree);
}

static inline void leftRotate(RBTree *pivot) {
  zltBiTreeLeftRotate((BiTree *) pivot);
}

static inline void rightRotate(RBTree *pivot) {
  zltBiTreeRightRotate((BiTree *) pivot);
}

static void afterInsert(RBTree *node);

void zltRBTreeAfterInsert(RBTree **root, RBTree *node) {
  afterInsert(node);
  *root = mostTop(node);
}

typedef void AfterInsert1(RBTree *node, RBTree *parent, RBTree *gparent);

static AfterInsert1 afterInsertLL;
static AfterInsert1 afterInsertRR;
static AfterInsert1 afterInsertLR;
static AfterInsert1 afterInsertRL;

void afterInsert(RBTree *node) {
  RBTree *parent = node->parent;
  if (!parent) {
    node->red = false;
    return;
  }
  if (!parent->red) {
    return;
  }
  RBTree *gparent = parent->parent;
  RBTree *uncle = parent == gparent->lchd ? gparent->rchd : gparent->lchd;
  if (uncle && uncle->red) {
    parent->red = false;
    uncle->red = false;
    gparent->red = true;
    afterInsert(gparent);
    return;
  }
  AfterInsert1 *f;
  if (node == parent->lchd) {
    f = parent == gparent->lchd ? afterInsertLL : afterInsertLR;
  } else {
    f = parent == gparent->lchd ? afterInsertRL : afterInsertRR;
  }
  f(node, parent, gparent);
}

//     GB        PR
//    /  \      /  \ 
//   PR  UB -> NB  GB
//  /  \          /  \ 
// NR  SB        SB  UB
void afterInsertLL(RBTree *node, RBTree *parent, RBTree *gparent) {
  rightRotate(gparent);
  node->red = false;
  afterInsert(parent);
}

//   GB         PR
//  /  \       /  \ 
// UB  PR ->  GB  NB
//    /  \   /  \ 
//   SB  NR UB  SB
void afterInsertRR(RBTree *node, RBTree *parent, RBTree *gparent) {
  leftRotate(gparent);
  node->red = false;
  afterInsert(parent);
}

//   GB        GB          __NR__
//  /  \      /  \        /      \ 
// UB  PR -> UB  NR  ->  GB      PB
//    /  \      /  \    /  \    /  \ 
//   NR  SB    XB  PR  UB  XB  YB  SB
//  /  \          /  \ 
// XB  YB        YB  SB
void afterInsertLR(RBTree *node, RBTree *parent, RBTree *gparent) {
  rightRotate(parent);
  leftRotate(gparent);
  parent->red = false;
  afterInsert(node);
}

//     GB         GB        __NR__
//    /  \       /  \      /      \ 
//   PR  UB ->  NR  UB -> PB      GB
//  /  \       /  \      /  \    /  \ 
// SB  NR     PR  YB    SB  XB  YB  UB
//    /  \   /  \ 
//   XB  YB SB  XB
void afterInsertRL(RBTree *node, RBTree *parent, RBTree *gparent) {
  leftRotate(parent);
  rightRotate(gparent);
  parent->red = false;
  afterInsert(node);
}

static void beforeErase(RBTree *node);

void zltRBTreeBeforeErase(RBTree **root, RBTree *node) {
  if (node->lchd) {
    RBTree *a = mostRight(node->lchd);
    RBTree b = *a;
    *a = *node;
    *node = b;
  }
  beforeErase(node);
  RBTree *parent = node->parent;
  if (!parent) {
    *root = NULL;
    return;
  }
  *root = mostTop(parent);
  if (node == parent->lchd) {
    parent->lchd = NULL;
  } else {
    parent->rchd = NULL;
  }
}

typedef void BeforeErase1(RBTree *node);

static BeforeErase1 beforeEraseL0;
static BeforeErase1 beforeEraseR0;
static BeforeErase1 beforeEraseL1;
static BeforeErase1 beforeEraseR1;

void beforeErase(RBTree *node) {
  RBTree *parent = node->parent;
  if (node->red) {
    return;
  }
  RBTree *rchd = node->rchd;
  if (rchd) {
    rchd->parent = parent;
    rchd->red = false;
    if (!parent) {
      *root = rchd;
    } else if (parent->lchd) {
      parent->lchd = rchd;
    } else {
      parent->rchd = rchd;
    }
    return;
  }
  if (!parent) {
    *root = NULL;
    return;
  }
  BeforeErase *f;
  RBTree *sibling;
  if (node == parent->lchd) {
    sibling = node->rchd;
    f = sibling->red ? beforeEraseL0 : beforeEraseL1;
  } else {
    sibling = node->rchd;
    f = sibling->red ? beforeEraseR0 : beforeEraseR1;
  }
  f(root, node, sibling, parent);
}

//   PB         SB
//  /  \       /  \ 
// NB  SR  -> PR  YB
//    /  \   /  \ 
//   XB  YB NB  XB
void beforeEraseL0(RBTree **root, RBTree *node, RBTree *sibling, RBTree *parent) {
  RBTree *x = sibling->lchd;
  leftRotate(parent);
  parent->red = true;
  sibling->red = false;
  beforeEraseL1(root, node, x, parent);
}

//     PB        SB
//    /  \      /  \ 
//   SR  NB -> XB  PR
//  /  \          /  \ 
// XB  YB        YB  NB
void beforeEraseR0(RBTree **root, RBTree *node, RBTree *sibling, RBTree *parent) {
  RBTree *y = sibling->rchd;
  rightRotate(parent);
  parent->red = true;
  sibling->red = false;
  beforeEraseR1(root, node, y, parent);
}

static BeforeErase beforeEraseL1a;
static BeforeErase beforeEraseL1b;
static BeforeErase beforeEraseL1c;

void beforeEraseL1(RBTree **root, RBTree *node, RBTree *sibling, RBTree *parent) {
  RBTree *x = sibling->lchd;
  RBTree *y = sibling->rchd;
  BeforeErase *f = x && x->red ? y && y->red ? beforeEraseL1a : beforeEraseL1b : beforeEraseL1c;
  f(root, node, sibling, parent);
}

//   PC         SC
//  /  \       /  \ 
// NB  SB  -> PB  YB
//    /  \   /  \ 
//   XR  YR NB  XR
void beforeEraseL1a(RBTree **root, RBTree *node, RBTree *sibling, RBTree *parent) {
  RBTree *y = node->rchd;
  leftRotate(parent);
  sibling->red = parent->red;
  parent->red = false;
  y->red = false;
}

static BeforeErase beforeEraseL1b;
static BeforeErase beforeEraseL1c;
