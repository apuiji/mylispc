#ifndef MYLISPC_PREPROC_H
#define MYLISPC_PREPROC_H

#include"mylispc.h"

typedef struct {
  mylispxPos pos;
  const zltString *params;
  size_t paramc;
  void *body;
} mylispcMacro;

static inline mylispcMacro mylispcMacroMake(mylispxPos pos, const zltString *params, size_t paramc, void *body) {
  return (mylispcMacro) { .pos = pos, .params = params, .paramc = paramc, .body = body };
}

void mylispcMacroClean(void *macro);

typedef struct {
  zltStrTree strTree;
  mylispcMacro macro;
} mylispcMacroTree;

#define mylispcMacroTreeMemb(p, m) zltMemb(p, mylispcMacroTree, m)

static inline mylispcMacroTree mylispcMacroTreeMake(const void *parent, zltString name, mylispcMacro macro) {
  return (mylispcMacroTree) { .strTree = zltStrTreeMake(parent, name), .macro = macro };
}

static inline mylispcMacroTree *mylispcMacroTreeFind(const void *tree, zltString name) {
  return (mylispcMacroTree *) zltStrTreeFind(tree, name);
}

static inline mylispcMacroTree **mylispcMacroTreeFindForInsert(void **parent, void **tree, zltString name) {
  return (mylispcMacroTree **) zltStrTreeFindForInsert(parent, tree, name);
}

/// @return null when bad
void **mylispcPreproc(void **dest, mylispxPosStack *posk, mylispcMacroTree **macroTree, void **src);

#endif
