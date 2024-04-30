#include"zlt_bitree.h"

typedef zltBiTree BiTree;

void zltBiTreeClean(BiTree *root, ZLT_NN zltBiTreeDtor *dtor) {
  if (!root) ZLT_UNLIKELY {
    return;
  }
  zltBiTreeClean(root->lchd, dtor);
  zltBiTreeClean(root->rchd, dtor);
  dtor(root);
}

// most functions begin
BiTree *zltBiTreeMostTop(ZLT_NN const BiTree *tree) {
  return tree->top ? zltBiTreeMostTop(tree->top) : (BiTree *) tree;
}

BiTree *zltBiTreeMostLeft(ZLT_NN const BiTree *tree) {
  return tree->lchd ? zltBiTreeMostLeft(tree->lchd) : (BiTree *) tree;
}

BiTree *zltBiTreeMostRight(ZLT_NN const zltBiTree *tree) {
  return tree->rchd ? zltBiTreeMostRight(tree->rchd) : (BiTree *) tree;
}
// most functions end

// iterators begin
BiTree *zltBiTreeLNR(ZLT_NN const BiTree *tree) {
  if (tree->rchd) {
    return zltBiTreeMostLeft(tree->rchd);
  }
  BiTree *parent = tree->parent;
  if (!parent) ZLT_UNLIKELY {
    return NULL;
  }
  if (tree == parent->lchd) {
    return parent;
  }
  return NULL;
}

BiTree *zltBiTreeRNL(ZLT_NN const BiTree *tree) {
  if (tree->lchd) {
    return zltBiTreeMostRight(tree->lchd);
  }
  BiTree *parent = tree->parent;
  if (!parent) ZLT_UNLIKELY {
    return NULL;
  }
  if (tree == parent->rchd) {
    return parent;
  }
  return NULL;
}

BiTree *zltBiTreeLRN(ZLT_NN const BiTree *tree) {
  BiTree *parent = tree->parent;
  if (!parent) ZLT_UNLIKELY {
    return NULL;
  }
  if (tree == parent->lchd) {
    return parent->rchd ? zltBiTreeMostLeft(parent->rchd) : parent;
  }
  return parent;
}

BiTree *zltBiTreeRLN(ZLT_NN const BiTree *tree) {
  BiTree *parent = tree->parent;
  if (!parent) ZLT_UNLIKELY {
    return NULL;
  }
  if (tree == parent->rchd) {
    return parent->lchd ? zltBiTreeMostRight(parent->lchd) : parent;
  }
  return parent;
}

static BiTree *nlr(ZLT_NN const BiTree *tree, const BiTree *parent);

BiTree *zltBiTreeNLR(ZLT_NN const BiTree *tree) {
  if (tree->lchd) {
    return tree->lchd;
  }
  if (tree->rchd) {
    return tree->rchd;
  }
  return nlr(tree, tree->parent);
}

BiTree *nlr(ZLT_NN const BiTree *tree, const BiTree *parent) {
  if (!parent) ZLT_UNLIKELY {
    return NULL;
  }
  if (tree == parent->lchd) {
    return parent->rchd;
  }
  return nlr(parent, parent->parent);
}

static BiTree *nrl(ZLT_NN const BiTree *tree, const BiTree *parent);

BiTree *zltBiTreeNRL(ZLT_NN const BiTree *tree) {
  if (tree->rchd) {
    return tree->rchd;
  }
  if (tree->lchd) {
    return tree->lchd;
  }
  return nrl(tree, tree->parent);
}

BiTree *nrl(ZLT_NN const BiTree *tree, const BiTree *parent) {
  if (!parent) ZLT_UNLIKELY {
    return NULL;
  }
  if (tree == parent->rchd) {
    return parent->lchd;
  }
  return nrl(parent, parent->parent);
}
// iterators end

// rotations begin
BiTree *zltBiTreeLeftRotate(ZLT_NN BiTree *pivot) {
  BiTree *parent = pivot->parent;
  BiTree *rchd = pivot->rchd;
  pivot->parent = rchd;
  pivot->rchd = rchd->lchd;
  rchd->parent = parent;
  rchd->lchd = pivot;
  if (parent) {
    if (pivot == parent->lchd) {
      parent->lchd = rchd;
    } else {
      parent->rchd = rchd;
    }
  }
  return rchd;
}

BiTree *zltBiTreeRightRotate(ZLT_NN BiTree *pivot) {
  BiTree *parent = pivot->parent;
  BiTree *lchd = pivot->lchd;
  pivot->parent = lchd;
  pivot->lchd = lchd->rchd;
  lchd->parent = parent;
  lchd->rchd = pivot;
  if (parent) {
    if (pivot == parent->lchd) {
      parent->lchd = lchd;
    } else {
      parent->rchd = lchd;
    }
  }
  return lchd;
}
// rotations end

// find functions begin
BiTree *zltBiTreeFind(const BiTree *tree, ZLT_NN zltBiTreeFinder *finder, void *data) {
  if (!tree) ZLT_UNLIKELY {
    return NULL;
  }
  int diff = finder(data, tree);
  if (!diff) {
    return tree;
  }
  BiTree *next = diff < 0 ? tree->lchd : tree->rchd;
  return zltBiTreeFind(next, finder, data);
}

// find functions end
BiTree **zltBiTreeFindForWrite(ZLT_NN BiTree **parent, ZLT_NN BiTree **tree, ZLT_NN zltBiTreeFinder *finder, void *data) {
  if (!*tree) ZLT_UNLIKELY {
    return tree;
  }
  int diff = finder(data, *tree);
  if (!diff) {
    return tree;
  }
  *parent = *tree;
  BiTree *next = diff < 0 ? &(**tree).lchd : &(**tree).rchd;
  return zltBiTreeFindForWrite(parent, next, find, data);
}
