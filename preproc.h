#ifndef MYLISPC_PREPROC_H
#define MYLISPC_PREPROC_H

#include"mylispc.h"

typedef struct {
  mylispxPos pos;
  const zltString **params;
  size_t paramc;
  void *body;
} mylispcMacro;

static inline mylispcMacro mylispcMacroMake(mylispxPos pos, const zltString **params, size_t paramc, void *body) {
  return (mylispcMacro) { .pos = pos, .params = params, .paramc = paramc, .body = body };
}

void mylispcMacroClean(mylispcMacro *macro);

typedef struct {
  zltPtrTree ptrTree;
  mylispcMacro macro;
} mylispcMacroTree;

#define mylispcMacroTreeMemb(p, m) zltMemb(p, mylispcMacroTree, m)

static inline mylispcMacroTree mylispcMacroTreeMake(const void *parent, const zltString *name, const mylispcMacro *macro) {
  return (mylispcMacroTree) { .ptrTree = zltPtrTreeMake(parent, name), .macro = *macro };
}

static inline mylispcMacroTree *mylispcMacroTreeFind(const void *tree, const zltString *name) {
  return (mylispcMacroTree *) zltPtrTreeFind(tree, name);
}

static inline mylispcMacroTree **mylispcMacroTreeFindForInsert(void **parent, void **tree, const zltString *name) {
  return (mylispcMacroTree **) zltPtrTreeFindForInsert(parent, tree, name);
}

/// @return null when bad
void **mylispcPreproc(void **dest, mylispcContext *ctx, void **src);

#endif
