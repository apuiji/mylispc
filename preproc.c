#include<string.h>
#include"nodes.h"
#include"preproc.h"

typedef mylispcContext Context;

void mylispcMacroClean(mylispcMacro *macro) {
  free(macro->params);
  mylispcNodeClean(macro->body, NULL);
}

static void **preprocList(void **dest, Context *ctx, void **src, void **first);

void **mylispcPreproc(void **dest, Context *ctx, void **src) {
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

static void **macroExpand(void **dest, Context *ctx, const mylispcMacro *macro, const void *src);

void **preprocList(void **dest, Context *ctx, void **src, void **first) {
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
  if (clazz == MYLISPC_NUM_ATOM_CLASS || clazz == MYLISPC_STR_ATOM_CLASS) {
    goto A;
  }
  int token = mylispcTokenAtomMemb(src, token);
  if (token == MYLISPC_POUND_TOKEN) {
    // TODO
  } else if (token == MYLISPC_POUND2_TOKEN) {
    // TODO
  } else if (token == MYLISPC_POUND_DEF_TOKEN) {
    // TODO
  } else if (token == MYLISPC_POUND_IF_TOKEN) {
    // TODO
  } else if (token == MYLISPC_POUND_INCLUDE_TOKEN) {
    // TODO
  } else if (token == MYLISPC_POUND_MOVE_TOKEN) {
    // TODO
  } else if (token == MYLISPC_POUND_POP_TOKEN) {
    // TODO
  } else if (token == MYLISPC_POUND_PUSH_TOKEN) {
    // TODO
  } else if (token == MYLISPC_POUND_UNDEF_TOKEN) {
    // TODO
  } else {
    goto A;
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
  const void *node;
} MacroExpTree;

#define macroExpTreeMemb(p, m) zltMemb(p, MacroExpTree, m)

static inline MacroExpTree macroExpTreeMake(const void *parent, const zltString *name, const void *node) {
  return (MacroExpTree) { .ptrTree = zltPtrTreeMake(parent, name), .node = node };
}

static bool macroExpTreeMake1(MacroExpTree **dest, Context *ctx, const zltString **param, size_t paramc, const void *src);

static void **macroExpand1(void **dest, Context *ctx, const MacroExpTree *expTree, const void *src);

void **macroExpand(void **dest, mylispcContext *ctx, const mylispcMacro *macro, const void *src) {
  MacroExpTree *expTree = NULL;
  if (!macroExpTreeMake1(&expTree, ctx, macro->params, macro->paramc, src)) {
    zltBiTreeClean(expTree, free);
    return NULL;
  }
  void **next = macroExpand1(dest, ctx, expTree, macro->body);
  zltBiTreeClean(expTree, free);
  return next;
}

bool macroExpTreeMake1(MacroExpTree **dest, Context *ctx, const zltString **param, size_t paramc, const void *src) {
  if (!paramc) {
    return true;
  }
  if (!*param) {
    goto A;
  }
  void *parent = NULL;
  void **slot = zltPtrTreeFindForInsert(&parent, dest, *param);
  if (*slot) {
    macroExpTreeMemb(*slot, node) = src;
  } else {
    MacroExpTree *a = zltTypeAlloc(MacroExpTree);
    if (!a) {
      mylispcReportBad(ctx, MYLISPC_OOM_BAD);
      return false;
    }
    *a = macroExpTreeMake(parent, *param, src);
    *slot = a;
  }
  A:
  return macroExpTreeMake1(dest, ctx, param + 1, param - 1, zltLinkMemb(src, next));
}

static bool clone(void **dest, Context *ctx, const void *src);
static void **clones(void **dest, Context *ctx, const void *src);
static bool cloneEOLAtom(void **dest, Context *ctx);
static bool cloneIDAtom(void **dest, Context *ctx, const mylispcIDAtom *src);
static bool cloneListAtom(void **dest, Context *ctx, const mylispcListAtom *src);
static bool cloneNumAtom(void **dest, Context *ctx, const mylispcNumAtom *src);
static bool cloneStrAtom(void **dest, Context *ctx, const mylispcStrAtom *src);
static bool cloneTokenAtom(void **dest, Context *ctx, const mylispcTokenAtom *src);

static inline bool isDot3(const void *s) {
  return zltStrMemb(s, size) >= 3 && !strncmp(zltStrMemb(s, data), "...", 3);
}

void **macroExpand1(void **dest, Context *ctx, const MacroExpTree *expTree, const void *src) {
  if (!src) {
    return dest;
  }
  int clazz = mylispcNodeMemb(src, clazz);
  if (clazz == MYLISPC_EOL_ATOM_CLASS) {
    if (!cloneEOLAtom(dest, ctx)) {
      return NULL;
    }
  } else if (clazz == MYLISPC_ID_ATOM_CLASS) {
    void *et = zltPtrTreeFind(expTree, mylispcIDAtomMemb(src, raw));
    if (!et) {
      if (!cloneIDAtom(dest, ctx, (const mylispcIDAtom *) src)) {
        return NULL;
      }
    } else if (isDot3(zltPtrTreeMemb(et, value))) {
      void **next = clones(dest, ctx, macroExpTreeMemb(et, node));
      if (!next) {
        return NULL;
      }
      return macroExpand1(next, ctx, expTree, zltLinkMemb(src, next));
    } else {
      if (!clone(dest, ctx, macroExpTreeMemb(et, node))) {
        return NULL;
      }
    }
  } else if (clazz == MYLISPC_LIST_ATOM_CLASS) {
    mylispcListAtom *a = zltTypeAlloc(mylispcListAtom);
    if (!a) {
      mylispcReportBad(ctx, MYLISPC_OOM_BAD);
      return NULL;
    }
    void *first = NULL;
    if (!macroExpand1(&first, ctx, expTree, mylispcListAtomMemb(src, first))) {
      mylispcNodeClean(first, NULL);
      return false;
    }
    *a = mylispcListAtomMake(first);
    *dest = a;
  } else if (clazz == MYLISPC_NUM_ATOM_CLASS) {
    if (!cloneNumAtom(dest, ctx, (const mylispcNumAtom *) src)) {
      return NULL;
    }
  } else if (clazz == MYLISPC_STR_ATOM_CLASS) {
    if (!cloneStrAtom(dest, ctx, (const mylispcStrAtom *) src)) {
      return NULL;
    }
  } else {
    if (!cloneTokenAtom(dest, ctx, (const mylispcTokenAtom *) src)) {
      return NULL;
    }
  }
  return macroExpand1(&zltLinkMemb(*dest, next), ctx, expTree, zltLinkMemb(src, next));
}

bool clone(void **dest, Context *ctx, const void *src) {
  int clazz = mylispcNodeMemb(src, clazz);
  if (clazz == MYLISPC_EOL_ATOM_CLASS) {
    if (!cloneEOLAtom(dest, ctx)) {
      return false;
    }
  } else if (clazz == MYLISPC_ID_ATOM_CLASS) {
    if (!cloneIDAtom(dest, ctx, (const mylispcIDAtom *) src)) {
      return false;
    }
  } else if (clazz == MYLISPC_LIST_ATOM_CLASS) {
    if (!cloneListAtom(dest, ctx, (const mylispcListAtom *) src)) {
      return false;
    }
  } else if (clazz == MYLISPC_NUM_ATOM_CLASS) {
    if (!cloneNumAtom(dest, ctx, (const mylispcNumAtom *) src)) {
      return false;
    }
  } else if (clazz == MYLISPC_STR_ATOM_CLASS) {
    if (!cloneStrAtom(dest, ctx, (const mylispcStrAtom *) src)) {
      return false;
    }
  } else {
    if (!cloneTokenAtom(dest, ctx, (const mylispcTokenAtom *) src)) {
      return false;
    }
  }
  return true;
}

void **clones(void **dest, Context *ctx, const void *src) {
  if (!src) {
    return dest;
  }
  if (!clone(dest, ctx, src)) {
    return NULL;
  }
  return clones(&zltLinkMemb(*dest, next), ctx, zltLinkMemb(src, next));
}

bool cloneEOLAtom(void **dest, Context *ctx) {
  mylispcEOLAtom *a = zltTypeAlloc(mylispcEOLAtom);
  if (!a) {
    mylispcReportBad(ctx, MYLISPC_OOM_BAD);
    return false;
  }
  *dest = a;
  return true;
}

bool cloneIDAtom(void **dest, Context *ctx, const mylispcIDAtom *src) {
  mylispcIDAtom *a = zltTypeAlloc(mylispcIDAtom);
  if (!a) {
    mylispcReportBad(ctx, MYLISPC_OOM_BAD);
    return false;
  }
  *a = mylispcIDAtomMake(src->raw);
  *dest = a;
  return true;
}

bool cloneListAtom(void **dest, Context *ctx, const mylispcListAtom *src) {
  mylispcListAtom *a = zltTypeAlloc(mylispcListAtom);
  if (!a) {
    mylispcReportBad(ctx, MYLISPC_OOM_BAD);
    return false;
  }
  void *first = NULL;
  if (!clones(&first, ctx, src->first)) {
    mylispcNodeClean(first, NULL);
    return false;
  }
  *a = mylispcListAtomMake(first);
  *dest = a;
  return true;
}

bool cloneNumAtom(void **dest, Context *ctx, const mylispcNumAtom *src) {
  mylispcNumAtom *a = zltTypeAlloc(mylispcNumAtom);
  if (!a) {
    mylispcReportBad(ctx, MYLISPC_OOM_BAD);
    return false;
  }
  *a = mylispcNumAtomMake(src->raw, src->value);
  *dest = a;
  return true;
}

bool cloneStrAtom(void **dest, Context *ctx, const mylispcStrAtom *src) {
  mylispcStrAtom *a = zltTypeAlloc(mylispcStrAtom);
  if (!a) {
    mylispcReportBad(ctx, MYLISPC_OOM_BAD);
    return false;
  }
  *a = mylispcStrAtomMake(src->value);
  *dest = a;
  return true;
}

bool cloneTokenAtom(void **dest, Context *ctx, const mylispcTokenAtom *src) {
  mylispcTokenAtom *a = zltTypeAlloc(mylispcTokenAtom);
  if (!a) {
    mylispcReportBad(ctx, MYLISPC_OOM_BAD);
    return false;
  }
  *a = mylispcTokenAtomMake(src->token);
  *dest = a;
  return true;
}
