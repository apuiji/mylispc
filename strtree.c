#include<string.h>
#include"strtree.h"

typedef mylispcStrTree StrTree;

void mylispcMakeStrTree(StrTree *dest, StrTree *parent, const char *str, size_t strSize) {
  dest->parent = parent;
  dest->lchd = NULL;
  dest->rchd = NULL;
  dest->str = str;
  dest->strSize = strSize;
}

void mylispcStrTreeClean(StrTree *root, mylispcStrTreeDtor *dtor) {
  if (!root) {
    return;
  }
  mylispcStrTreeClean(root->lchd, dtor);
  mylispcStrTreeClean(root->rchd, dtor);
  dtor(root);
}

static StrTree **compareForFind(const StrTree *root, const char *str, size_t strSize);

StrTree *mylispcStrTreeFind(const StrTree *root, const char *str, size_t strSize) {
  if (!root) {
    return NULL;
  }
  StrTree **next = compareForFind(root, str, strSize);
  return next ? mylispcStrTreeFind(*next, str, strSize) : root;
}

StrTree **compareForFind(const StrTree *root, const char *str, size_t strSize) {
  if (strSize < root->strSize) {
    return &root->lchd;
  }
  if (strSize > root->strSize) {
    return &root->rchd;
  }
  int diff = strncmp(str, root->str);
  if (diff < 0) {
    return &root->lchd;
  }
  if (diff > 0) {
    return &root->rchd;
  }
  return NULL;
}

StrTree **mylispcStrTreeFindForWrite(StrTree **parent, StrTree **root, const char *str, size_t strSize) {
  if (!*root) {
    return root;
  }
  StrTree **next = compareForFind(*root, str, strSize);
  if (!next) {
    return root;
  }
  *parent = *root;
  return mylispcStrTreeFindForWrite(parent, next, str, strSize);
}

StrTree *mylispcStrTreeBeforeErase(StrTree **root, StrTree *node) {}
