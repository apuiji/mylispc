#ifndef MYLISPC_H
#define MYLISPC_H

#include"zlt/link.h"
#include"zlt/string.h"

// positions begin
typedef struct {
  zltString file;
  int li;
} mylispcPos;

static inline mylispcPos mylispcPosMake(zltString file, int li) {
  return (mylispcPos) { .file = file, .li = li };
}

typedef struct {
  mylispcPos *data;
  mylispcPos *top;
  size_t left;
} mylispcPosStack;

static inline mylispcPosStack mylispcPosStackMake(mylispcPos *data, size_t size) {
  return (mylispcPosStack) { .data = data, .top = data, .left = size };
}

bool mylispcPosPush(mylispcPosStack *k, const mylispcPos *pos);
mylispcPos mylispcPosPop(mylispcPosStack *k);
// positions end

const char *mylispcHit(const char *it, const char *end);

typedef struct {
  int token;
  double numVal;
  zltString strVal;
} mylispcLexerDest;

/// @return null when bad
const char *mylispcLexer(mylispcLexerDest *dest, const char *it, const char *end);

void mylispcLexerRaw(mylispcLexerDest *dest, zltString raw);

typedef struct {
  zltLink link;
  int clazz;
} mylispcNode;

static inline mylispcNode mylispcNodeMake(int clazz) {
  return (mylispcNode) { .clazz = clazz };
}

void mylispcNodeDelete(void *node);
void mylispcNodeClean(void *node);

enum {
  // parse productions begin
  MYLISPC_ID_ATOM_CLASS,
  MYLISPC_LIST_ATOM_CLASS,
  MYLISPC_NUM_ATOM_CLASS,
  MYLISPC_STR_ATOM_CLASS,
  MYLISPC_TOKEN_ATOM_CLASS
  // parse productions end
};

bool mylispcParse(void **dest, mylispcPos *pos, const char *it, const char *end);

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

/// @return null when bad
void **mylispcPreproc(void **dest, mylispcPosStack *posk, mylispcMacroTree **macroTree, const void *src);

#endif
