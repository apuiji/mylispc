#ifndef MYLISPC_PREPROC_H
#define MYLISPC_PREPROC_H

#include"mylispc.h"
#include"zlt/rbtree.h"

typedef struct {
  mylispcPos pos;
  const zltString *params;
  size_t paramc;
  void *body;
} mylispcMacro;

static inline mylispcMacro mylispcMacroMake(mylispcPos pos, const zltString *params, size_t paramc, void *body) {
  return (mylispcMacro) { .pos = pos, .params = params, .paramc = paramc, .body = body };
}

typedef struct {
  zltRBTree rbTree;
  zltString name;
  mylispcMacro macro;
} mylispcMacroTree;

static inline mylispcMacroTree mylispcMacroTreeMake(const void *parent, zltString name, mylispcMacro macro) {
  return (mylispcMacroTree) { .rbTree = zltRBTreeMake(parent), .name = name, .macro = macro };
}

int mylispcMacroTreeCmpForFind(const void *data, const void *tree);

static inline mylispcMacroTree *mylispcMacroTreeFind(const void *tree, zltString name) {
  return (mylispcMacroTree *) zltBiTreeFind(tree, mylispcMacroTreeCmpForFind, &name);
}

static inline mylispcMacroTree **mylispcMacroTreeFindForInsert(void **parent, void **tree, zltString name) {
  return (mylispcMacroTree **) zltBiTreeFind(parent, tree, mylispcMacroTreeCmpForFind, &name);
}

/// @return null when bad
void **mylispcPreproc(void **dest, mylispcPosStack *posk, mylispcMacroTree **macroTree, void **src);

#endif
