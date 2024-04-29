#ifndef MYLISPC_STRSET_H
#define MYLISPC_STRSET_H

typedef struct zltStrTree zltStrTree;

struct zltStrTree {
  zltStrTree *parent;
  zltStrTree *lchd;
  zltStrTree *rchd;
  const char *value;
  size_t valueSize;
};

void zltMakeStrTree(zltStrTree *dest, zltStrTree *parent, const char *value, size_t valueSize);

zltStrTree *zltStrTreeFind(const zltStrTree *root, const char *value, size_t valueSize);

zltStrTree **zltStrTreeFindForInsert(zltStrTree **parent, const zltStrTree *root, const char *value, size_t valueSize);

zltStrTree *zltStrTreeBeforeErase(zltStrTree **root, zltStrTree *node);

#endif
