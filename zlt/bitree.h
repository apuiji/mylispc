#ifndef ZLT_BITREE_H
#define ZLT_BITREE_H

#include"xyz.h"

#ifdef __cplusplus

extern "C" {

#endif

typedef struct {
  void *parent;
  void *children[0];
  void *lchd;
  void *rchd;
} zltBiTree;

#define zltBiTreeMemb(p, m) zltMemb(p, zltBiTree, m)

// constructors and destructors begin
static inline zltBiTree zltBiTreeMake(const void *parent) {
  return (zltBiTree) { .parent = (void *) parent, .lchd = NULL, .rchd = NULL };
}

/// @param a requires not null
/// @param b requires not null
void zltBiTreeSwap(void **root, void *a, void *b);

typedef void zltBiTreeDtor(void *tree);

void zltBiTreeClean(void *tree, zltBiTreeDtor *dtor);
// constructors and destructors end

// iterators begin
/// @param tree requires not null
/// @param side -1 top, 0 left, 1 right
void *zltBiTreeMostSide(const void *tree, int side);

/// @param tree requires not null
/// @param xy 0 NLR, 1 NRL
void *zltBiTreeNXY(const void *tree, int xy);

/// @param tree requires not null
/// @param xy 0
void *zltBiTreeXNY(const void *tree, int xy);

/// @param tree requires not null
/// @param xy 0 LRN, 1 RLN
void *zltBiTreeXYN(const void *tree, int xy);
// iterators end

/// @param tree requires not null and has child on side rightabout rotation
void *zltBiTreeRotate(void *tree, bool right);

// find operations begin
typedef int zltBiTreeCmpForFind(const void *data, const void *tree);

void *zltBiTreeFind(const void *tree, zltBiTreeCmpForFind *cmp, const void *data);

/// @param[out] parent parent node of inserted, requires not null, usually initialized by null
/// @return insert slot
void **zltBiTreeFindForInsert(void **parent, void **tree, zltBiTreeCmpForFind *cmp, const void *data);
// find operations end

#ifdef __cplusplus

}

#endif

#endif
