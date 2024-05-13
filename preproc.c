#include<string.h>
#include"nodes.h"
#include"preproc.h"

void mylispcMacroClean(mylispcMacro *macro) {
  free(macro->params);
  mylispcNodeClean(macro->body, NULL);
}

static void **preprocList(void **dest, mylispcContext *ctx, void **src, void **first);

void **mylispcPreproc(void **dest, mylispcContext *ctx, void **src) {
  if (!*src) {
    return dest;
  }
  int clazz = mylispcNodeMemb(*src, clazz);
  if (clazz == MYLISPC_LIST_ATOM_CLASS) {
    return preprocList(dest, ctx, src, &mylispcListAtomMemb(*src, first));
  }
  if (clazz == MYLISPC_EOL_ATOM_CLASS) {
    ++ctx->pos.li;
  }
  void **next = zltLinkPush(dest, zltLinkPop(src));
  return mylispcPreproc(next, ctx, src);
}

static void **macroExpand(void **dest, mylispcContext *ctx, const mylispcMacro *macro, const void *src);

void **preprocList(void **dest, mylispcContext *ctx, void **src, void **first) {
  if (!*first) {
    void **next = zltLinkPush(dest, zltLinkPop(src));
    return mylispcPreproc(next, ctx, src);
  }
  int clazz = mylispcNodeMemb(first, clazz);
  if (clazz == MYLISPC_EOL_ATOM_CLASS) {
    void **next = zltLinkPush(dest, zltLinkPop(first));
    ++ctx->pos.li;
    return preprocList(next, ctx, src, first);
  }
  if (clazz == MYLISPC_ID_ATOM_CLASS) {
    mylispcMacroTree *mt = mylispcMacroTreeFind(ctx->macros, mylispcIDAtomMemb(*first, raw));
    if (!mt) {
      goto A;
    }
    if (!mylispxPosStackPush(&ctx->posk, &ctx->pos)) {
      mylispcReportBad(ctx, MYLISPC_POS_STACK_SIZE_OVER_LIMIT_BAD);
      return NULL;
    }
    ctx->pos = mt->macro.pos;
    void *src1 = NULL;
    void **next = macroExpand(&src1, ctx, &mt->macro, zltLinkMemb(*first, next));
    if (!next) {
      mylispcNodeClean(src1, NULL);
      return NULL;
    }
    ctx->pos = mylispxPosStackPeek(&ctx->posk);
    mylispxPosStackPop(&ctx->posk);
    mylispcNodeDelete(zltLinkPop(src));
    *next = *src;
    *src = src1;
    return mylispcPreproc(dest, ctx, src);
  }
  if (clazz == MYLISPC_LIST_ATOM_CLASS) {
    void *first1 = NULL;
    void **next = preprocList(&first1, ctx, first, &mylispcListAtomMemb(*first, first));
    if (!next) {
      mylispcNodeClean(first1, NULL);
      return NULL;
    }
    *next = *first;
    return preprocList(dest, ctx, src, first1);
  }
  A:
  {
    void *first1 = NULL;
    if (!mylispcPreproc(&first1, ctx, first)) {
      mylispcNodeClean(first1, NULL);
      return NULL;
    }
    mylispcListAtomMemb(*src, first) = first1;
    void **next = zltLinkPush(dest, zltLinkPop(src));
    return mylispcPreproc(next, ctx, src);
  }
}

typedef struct {
  zltPtrTree ptrTree;
  void *node;
} MacroExpTree;

static inline MacroExpTree macroExpTreeMake(const void *parent, const zltString *name, void *node) {
  return (MacroExpTree) { .ptrTree = zltPtrTreeMake(parent, name), .node = node };
}

static bool macroExpTreeMake1(MacroExpTree **dest, const void *src);

void **macroExpand(void **dest, mylispcContext *ctx, const mylispcMacro *macro, const void *src) {
  ;
}

bool macroExpTreeMake1(MacroExpTree **dest, const void *src) {}

static bool cloneEmptyListAtom(void **dest);
static bool cloneEOLAtom(void **dest);
static bool cloneIDAtom(void **dest, const mylispcIDAtom *src);
static bool cloneListAtom(void **dest, const mylispcListAtom *src);
static bool cloneNumAtom(void **dest, const mylispcNumAtom *src);
static bool cloneStrAtom(void **dest, const mylispcStrAtom *src);
static bool cloneTokenAtom(void **dest, const mylispcTokenAtom *src);
static void **clones(void **dest, const void *src);

void **mylispcPreproc(void **dest, mylispxPosStack *posk, mylispcMacroTree **macroTree, const void *src) {
  if (!src) {
    return dest;
  }
  int clazz = zltMemb(src, mylispcNode, clazz);
  if (clazz == MYLISPC_EOL_ATOM_CLASS) {
    if (!cloneEOLAtom(dest)) {
      return NULL;
    }
    ++posk->top->li;
  }
  if (clazz == MYLISPC_ID_ATOM_CLASS) {
    if (!cloneIDAtom(dest, (const mylispcIDAtom *) src)) {
      return NULL;
    }
  } else if (clazz == MYLISPC_NUM_ATOM_CLASS) {
    if (!cloneNumAtom(dest, (const mylispcNumAtom *) src)) {
      return NULL;
    }
  } else if (clazz == MYLISPC_STR_ATOM_CLASS) {
    if (!cloneStrAtom(dest, (const mylispcStrAtom *) src)) {
      return NULL;
    }
  } else if (clazz == MYLISPC_TOKEN_ATOM_CLASS) {
    if (!cloneTokenAtom(dest, (const mylispcTokenAtom *) src)) {
      return NULL;
    }
  } else {
    void **next = preprocList(dest, posk, macroTree, zltMemb(src, mylispcListAtom, first));
    return next ? mylispcPreproc(next, posk, macroTree, zltMemb(src, zltLink, next)) : NULL;
  }
  return mylispcPreproc(&zltMemb(*dest, zltLink, next), posk, macroTree, zltMemb(src, zltLink, next));
}

bool cloneEmptyListAtom(void **dest) {
  mylispcListAtom *a = zltTypeAlloc(mylispcListAtom);
  if (!a) {
    mylispBad = MYLISP_OOM_BAD;
    return false;
  }
  *a = mylispcListAtomMake(NULL);
  *dest = a;
  return true;
}

bool cloneEOLAtom(void **dest) {
  mylispcEOLAtom *eol = zltTypeAlloc(mylispcEOLAtom);
  if (!a) {
    mylispBad = MYLISP_OOM_BAD;
    return false;
  }
  *a = mylispcNodeMake(MYLISPC_EOL_ATOM_CLASS);
  *dest = a;
  return true;
}

bool cloneIDAtom(void **dest, const mylispcIDAtom *src) {
  mylispcIDAtom *a = zltTypeAlloc(mylispcIDAtom);
  if (!a) {
    mylispBad = MYLISP_OOM_BAD;
    return false;
  }
  *a = mylispcIDAtomMake(src->raw);
  *dest = a;
  return true;
}

bool cloneListAtom(void **dest, const mylispcListAtom *src) {
  mylispcListAtom *a = zltTypeAlloc(mylispcListAtom);
  if (!a) {
    mylispBad = MYLISP_OOM_BAD;
    return false;
  }
  void *first = NULL;
  if (!clones(&first, src->first)) {
    mylispcNodeClean(first);
    return false;
  }
  *a = mylispcListAtomMake(first);
  *dest = a;
  return true;
}

bool cloneNumAtom(void **dest, const mylispcNumAtom *src) {
  mylispcNumAtom *a = zltTypeAlloc(mylispcNumAtom);
  if (!a) {
    mylispBad = MYLISP_OOM_BAD;
    return false;
  }
  *a = mylispcNumAtomMake(src->raw, src->value);
  *dest = a;
  return true;
}

bool cloneStrAtom(void **dest, const mylispcStrAtom *src) {
  mylispcStrAtom *a = zltTypeAlloc(mylispcStrAtom);
  if (!a) {
    mylispBad = MYLISP_OOM_BAD;
    return false;
  }
  *a = mylispcStrAtomMake(src->value);
  *dest = a;
  return true;
}

bool cloneTokenAtom(void **dest, const mylispcTokenAtom *src) {
  mylispcTokenAtom *a = zltTypeAlloc(mylispcTokenAtom);
  if (!a) {
    mylispBad = MYLISP_OOM_BAD;
    return false;
  }
  *a = mylispcTokenAtomMake(src->token);
  *dest = a;
  return true;
}

void **clones(void **dest, const void *src) {
  int clazz = zltMemb(src, mylispcNode, clazz);
  if (clazz == MYLISPC_EOL_ATOM_CLASS) {
    if (!cloneEOLAtom(dest)) {
      return NULL;
    }
  }
  if (clazz == MYLISPC_ID_ATOM_CLASS) {
    if (!cloneIDAtom(dest, (const mylispcIDAtom *) src)) {
      return NULL;
    }
  } else if (clazz == MYLISPC_LIST_ATOM_CLASS) {
    if (!cloneListAtom(dest, (const mylispcListAtom *) src)) {
      return NULL;
    }
  } else if (clazz == MYLISPC_NUM_ATOM_CLASS) {
    if (!cloneNumAtom(dest, (const mylispcNumAtom *) src)) {
      return NULL;
    }
  } else if (clazz == MYLISPC_STR_ATOM_CLASS) {
    if (!cloneStrAtom(dest, (const mylispcStrAtom *) src)) {
      return NULL;
    }
  } else {
    if (!cloneTokenAtom(dest, (const mylispcTokenAtom *) src)) {
      return NULL;
    }
  }
  return clones(&zltMemb(*dest, zltLink, next), zltMemb(src, zltLink, next));
}

static void **macroExpand(
  void **dest, mylispxPosStack *posk, mylispcMacroTree *macroTree, mylispcMacro *macro, const void *src);
static void **preprocList1(void **dest, mylispxPosStack *posk, mylispcMacroTree *macroTree, const void *src);

void **preprocList(void **dest, mylispxPosStack *posk, mylispcMacroTree *macroTree, const void *src) {
  if (!src) {
    return cloneEmptyListAtom(dest) ? dest : NULL;
  }
  int clazz = zltMemb(src, mylispcNode, clazz);
  if (clazz == MYLISPC_EOL_ATOM_CLASS) {
    cloneEOLAtom(dest);
    ++posk->top->li;
    return preprocList(&zltMemb(*dest, zltLink, next), posk, macroTree, zltMemb(src, zltLink, next));
  }
  if (clazz == MYLISPC_ID_ATOM_CLASS) {
    mylispcMacroTree *mt = mylispcMacroTreeFind(macroTree, zltMemb(src, mylispcIDAtom, raw));
    if (!mt) {
      goto A;
    }
    return macroExpand(dest, posk, macroTree, &mt->macro, zltMemb(src, zltLink, next));
  }
  if (clazz == MYLISPC_LIST_ATOM_CLASS) {}
  if (clazz == MYLISPC_NUM_ATOM_CLASS) {}
  A:
  return preprocList1(dest, posk, macroTree, src);
}

typedef struct {
  zltRBTree rbTree;
  zltString name;
  void *atom;
} MacroExpansionMap;
