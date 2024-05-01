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

typedef void AfterInsert(RBTree **root, RBTree *node, RBTree *parent, RBTree *gparent);

static AfterInsert afterInsertLL;
static AfterInsert afterInsertRR;
static AfterInsert afterInsertLR;
static AfterInsert afterInsertRL;

void zltRBTreeAfterInsert(RBTree **root, RBTree *node) {
  RBTree *parent = node->parent;
  if (!parent) {
    *root = node;
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
    zltRBTreeAfterInsert(root, gparent);
    return;
  }
  AfterInsert *ai;
  if (node == parent->lchd) {
    ai = parent == gparent->lchd ? afterInsertLL : afterInsertLR;
  } else {
    ai = parent == gparent->lchd ? afterInsertRL : afterInsertRR;
  }
  ai(root, node, parent, gparent);
}

//     GB        PB
//    /  \      /  \ 
//   PR  UB -> NR  GR
//  /  \          /  \ 
// NR  SB        SB  UB
void afterInsertLL(RBTree **root, RBTree *node, RBTree *parent, RBTree *gparent) {
  rightRotate(gparent);
  parent->red = false;
  gparent->red = true;
  if (*root == gparent) {
    *root = parent;
  }
}

void afterInsertRR(RBTree **root, RBTree *node, RBTree *parent, RBTree *gparent) {
  leftRotate(gparent);
  parent->red = false;
  gparent->red = true;
  if (*root == gparent) {
    *root = parent;
  }
}

//   GB        GB
//  /  \      /  \ 
// UB  PR -> UB  NR
//    /  \      /  \ 
//   NR  SB    XB  PR
//  /  \          /  \ 
// XB  YB        YB  SB
void afterInsertLR(RBTree **root, RBTree *node, RBTree *parent, RBTree *gparent) {
  rightRotate(parent);
  afterInsertRR(root, parent, node, gparent);
}

void afterInsertRL(RBTree **root, RBTree *node, RBTree *parent, RBTree *gparent) {
  leftRotate(parent);
  afterInsertLL(root, parent, node, gparent);
}

static void beforeErase1(RBTree **root, RBTree *node);
static void beforeErase2(RBTree **root, RBTree *node, RBTree *chd);
static void beforeErase3(RBTree *node, RBTree *parent);

void zltRBTreeBeforeErase(RBTree **root, RBTree *node) {
  beforeErase1(root, node);
  if (node->red) {
    RBTree *parent = node->parent;
    if (node == parent->lchd) {
      parent->lchd = NULL;
    } else {
      parent->rchd = NULL;
    }
    return;
  }
  if (node->lchd) {
    beforeErase2(root, node, node->lchd);
    return;
  }
  if (node->rchd) {
    beforeErase2(root, node, node->rchd);
    return;
  }
  if (!node->parent) {
    *root = NULL;
    return;
  }
  beforeErase3(node, node->parent);
  *root = mostTop(node);
  RBTree *parent = node->parent;
  if (node == parent->lchd) {
    parent->lchd = node;
  } else {
    parent->rchd = node;
  }
}

void beforeErase1(RBTree **root, RBTree *node) {
  RBTree *a = zltBiTreeLNR((BiTree *) node);
  if (!a) {
    return;
  }
  RBTree b = *a;
  *a = *node;
  *node = b;
  if (*root == node) {
    *root = a;
  }
}

void beforeErase2(RBTree **root, RBTree *node, RBTree *chd) {
  RBTree *parent = node->parent;
  if (!parent) {
    *root = chd;
  } else if (node == parent->lchd) {
    parent->lchd = chd;
  } else {
    parent->rchd = chd;
  }
  chd->parent = parent;
  chd->red = false;
}

typedef void BeforeErase3a(RBTree *node, RBTree *sibling, RBTree *parent);

static BeforeErase3a beforeErase3a1;
static BeforeErase3a beforeErase3a1a;
static BeforeErase3a beforeErase3a2;
static BeforeErase3a beforeErase3a2a;

void beforeErase3(RBTree *node, RBTree *parent) {
  if (!parent) {
    return;
  }
  RBTree *sibling;
  BeforeErase3a *be3a;
  if (node == parent->lchd) {
    sibling = parent->rchd;
    be3a = sibling->red ? beforeErase3a1 : beforeErase3a2;
  } else {
    sibling = parent->lchd;
    be3a = sibling->red ? beforeErase3a1a : beforeErase3a2a;
  }
  be3a(node, sibling, parent);
}

//   PB         SB
//  /  \       /  \ 
// NB  SR  -> PR  YB
//    /  \   /  \ 
//   XB  YB NB  XB
void beforeErase3a1(RBTree *node, RBTree *sibling, RBTree *parent) {
  RBTree *x = sibling->lchd;
  leftRotate(parent);
  sibling->red = parent->red;
  parent->red = true;
  beforeErase3a2(node, x, parent);
}

void beforeErase3a1a(RBTree *node, RBTree *sibling, RBTree *parent) {
  RBTree *x = sibling->rchd;
  rightRotate(parent);
  sibling->red = parent->red;
  parent->red = true;
  beforeErase3a2a(node, x, parent);
}

static void beforeErase3b1(RBTree *node, RBTree *sibling, RBTree *nephew, RBTree *parent);
static void beforeErase3b1a(RBTree *node, RBTree *sibling, RBTree *nephew, RBTree *parent);
static void beforeErase3b2(RBTree *node, RBTree *sibling, RBTree *nephew, RBTree *parent);
static void beforeErase3b2a(RBTree *node, RBTree *sibling, RBTree *nephew, RBTree *parent);
static void beforeErase3b3(RBTree *node, RBTree *sibling, RBTree *parent);

void beforeErase3a2(RBTree *node, RBTree *sibling, RBTree *parent) {
  RBTree *lnep = sibling->lchd;
  if (lnep && lnep->red) {
    beforeErase3b1(node, sibling, lnep, parent);
    return;
  }
  RBTree *rnep = sibling->rchd;
  if (rnep && rnep->red) {
    beforeErase3b2(node, sibling, rnep, parent);
    return;
  }
  beforeErase3b3(node, sibling, parent);
}

void beforeErase3a2a(RBTree *node, RBTree *sibling, RBTree *parent) {
  RBTree *rnep = sibling->rchd;
  if (rnep && rnep->red) {
    beforeErase3b1a(node, sibling, rnep, parent);
    return;
  }
  RBTree *lnep = sibling->lchd;
  if (lnep && lnep->red) {
    beforeErase3b2a(node, sibling, lnep, parent);
    return;
  }
  beforeErase3b3(node, sibling, parent);
}

//   PC        PC          __XC__
//  /  \      /  \        /      \ 
// NB  SB -> NB  XR  ->  PB      SB
//    /         /  \    /  \    /
//   XR        UB  SB  NB  UB  VB
//  /  \          /
// UB  VB        VB
void beforeErase3b1(RBTree *node, RBTree *sibling, RBTree *nephew, RBTree *parent) {
  rightRotate(sibling);
  leftRotate(parent);
  nephew->red = parent->red;
  parent->red = false;
}

void beforeErase3b1a(RBTree *node, RBTree *sibling, RBTree *nephew, RBTree *parent) {
  leftRotate(sibling);
  rightRotate(parent)
  nephew->red = parent->red;
  parent->red = false;
}

//   PC          __SC__
//  /  \        /      \ 
// NB  SB  ->  PB      YB
//    /  \    /  \    /  \ 
//   X?  YR  NB  X?  UB  VB
//      /  \ 
//     UB  VB
void beforeErase3b2(RBTree *node, RBTree *sibling, RBTree *nephew, RBTree *parent) {
  leftRotate(parent);
  sibling->red = parent->red;
  nephew->red = false;
  parent->red = false;
}

void beforeErase3b2a(RBTree *node, RBTree *sibling, RBTree *nephew, RBTree *parent) {
  rightRotate(parent);
  sibling->red = parent->red;
  nephew->red = false;
  parent->red = false;
}

//   PC         PB
//  /  \       /  \ 
// NB  SB  -> NB  SR
//    /  \       /  \ 
//   XB  YB     XB  YB
void beforeErase3b3(RBTree *node, RBTree *sibling, RBTree *parent) {
  sibling->red = true;
  if (parent->red) {
    parent->red = false;
  } else {
    beforeErase3(parent, parent->parent);
  }
}
