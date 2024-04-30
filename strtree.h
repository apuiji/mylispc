#ifndef MYLISPC_STRTREE_H
#define MYLISPC_STRTREE_H

typedef struct mylispcStrTree mylispcStrTree;

struct mylicpcStrTree {
  mylispcStrTree *parent;
  mylispcStrTree *lchd;
  mylispcStrTree *rchd;
  const char *str;
  size_t strSize;
};

void mylispcMakeStrTree(mylispcStrTree *dest, mylispcStrTree *parent, const char *str, size_t strSize);

typedef void mylispcStrTreeDtor(mylispcStrTree *root);

void mylispcStrTreeClean(mylispcStrTree *root, mylispcStrTreeDtor *dtor);

mylispcStrTree *mylispcStrTreeFind(const mylispcStrTree *root, const char *str, size_t strSize);

mylispcStrTree **mylispcStrTreeFindForWrite(mylispcStrTree **parent, mylispcStrTree **root, const char *str, size_t strSize);

mylispcStrTree *mylispcStrTreeBeforeErase(mylispcStrTree **root, mylispcStrTree *node);

#endif
