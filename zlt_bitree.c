#include"zlt_bitree.h"

typedef zltBiTree BiTree;

void zltBiTreeClean(BiTree *root, zltBiTreeDtor *dtor) {
  if (!root) {
    return;
  }
  zltBiTreeClean(root->lchd, dtor);
  zltBiTreeClean(root->rchd, dtor);
  dtor(root);
}

// most functions begin
BiTree *zltBiTreeMostTop(const BiTree *tree) {
  return tree->top ? zltBiTreeMostTop(tree->top) : (BiTree *) tree;
}

BiTree *zltBiTreeMostLeft(const BiTree *tree) {
  return tree->lchd ? zltBiTreeMostLeft(tree->lchd) : (BiTree *) tree;
}

BiTree *zltBiTreeMostRight(const zltBiTree *tree) {
  return tree->rchd ? zltBiTreeMostRight(tree->rchd) : (BiTree *) tree;
}
// most functions end

// iterators begin
BiTree *zltBiTreeLNR(const BiTree *tree) {
  if (tree->rchd) {
    return zltBiTreeMostLeft(tree->rchd);
  }
  BiTree *parent = tree->parent;
  if (!parent) {
    return NULL;
  }
  if (tree == parent->lchd) {
    return parent;
  }
  return NULL;
}

BiTree *zltBiTreeRNL(const BiTree *tree) {
  if (tree->lchd) {
    return zltBiTreeMostRight(tree->lchd);
  }
  BiTree *parent = tree->parent;
  if (!parent) {
    return NULL;
  }
  if (tree == parent->rchd) {
    return parent;
  }
  return NULL;
}

BiTree *zltBiTreeLRN(const BiTree *tree) {
  BiTree *parent = tree->parent;
  if (!parent) {
    return NULL;
  }
  if (tree == parent->lchd) {
    return parent->rchd ? zltBiTreeMostLeft(parent->rchd) : parent;
  }
  return parent;
}

BiTree *zltBiTreeRLN(const BiTree *tree) {
  BiTree *parent = tree->parent;
  if (!parent) {
    return NULL;
  }
  if (tree == parent->rchd) {
    return parent->lchd ? zltBiTreeMostRight(parent->lchd) : parent;
  }
  return parent;
}

static BiTree *nlr(const BiTree *tree, const BiTree *parent);

BiTree *zltBiTreeNLR(const BiTree *tree) {
  if (tree->lchd) {
    return tree->lchd;
  }
  if (tree->rchd) {
    return tree->rchd;
  }
  return nlr(tree, tree->parent);
}

BiTree *nlr(const BiTree *tree, const BiTree *parent) {
  if (!parent) {
    return NULL;
  }
  if (tree == parent->lchd) {
    return parent->rchd;
  }
  return nlr(parent, parent->parent);
}

static BiTree *nrl(const BiTree *tree, const BiTree *parent);

BiTree *zltBiTreeNRL(const BiTree *tree) {
  if (tree->rchd) {
    return tree->rchd;
  }
  if (tree->lchd) {
    return tree->lchd;
  }
  return nrl(tree, tree->parent);
}

BiTree *nrl(const BiTree *tree, const BiTree *parent) {
  if (!parent) {
    return NULL;
  }
  if (tree == parent->rchd) {
    return parent->lchd;
  }
  return nrl(parent, parent->parent);
}
// iterators end

// rotations begin
BiTree *zltBiTreeLeftRotate(BiTree *pivot) {
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

BiTree *zltBiTreeRightRotate(BiTree *pivot) {
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
BiTree *zltBiTreeFind(const BiTree *tree, zltBiTreeCmpForFind *cmp, void *data) {
  if (!tree) {
    return NULL;
  }
  BiTree *next;
  int diff = cmp(data, tree);
  if (diff < 0) {
    next = tree->lchd;
  } else if (diff > 0) {
    next = tree->rchd;
  } else {
    return tree;
  }
  return zltBiTreeFind(next, cmp, data);
}

// find functions end
BiTree **zltBiTreeFindForInsert(BiTree **parent, BiTree **tree, zltBiTreeCmpForFind *cmp, void *data) {
  if (!*tree) {
    return tree;
  }
  BiTree **next;
  int diff = cmp(data, *tree);
  if (diff < 0) {
    next = &(*tree)->lchd;
  } else if (diff > 0) {
    next = &(*tree)->rchd;
  } else {
    return tree;
  }
  *parent = *tree;
  return zltBiTreeFindForInsert(parent, next, cmp, data);
}
