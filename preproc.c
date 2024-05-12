#include<string.h>
#include"preproc.h"

int mylispcMacroTreeCmpForFind(const void *data, const void *tree) {
  const zltString *name = &zltMemb(tree, mylispcMacroTree, name);
  if (zltMemb(data, zltString, size) < name->size) {
    return -1;
  }
  if (zltMemb(data, zltString, size) > name->size) {
    return 1;
  }
  return strncmp(zltMemb(data, zltString, data), name->data, name->size);
}

static void **preprocList(void **dest, mylispcPosStack *posk, mylispcMacroTree *macroTree, void **src, void **first);

void **mylispcPreproc(void **dest, mylispcPosStack *posk, mylispcMacroTree *macroTree, void **src) {
  if (!*src) {
    return dest;
  }
  int clazz = zltMemb(*src, mylispcNode, clazz);
  if (clazz == MYLISPC_LIST_ATOM_CLASS) {
    return preprocList(dest, posk, macroTree, src, &zltMemb(*src, mylispcListAtom, first));
  }
  if (clazz == MYLISPC_EOL_ATOM_CLASS) {
    ++posk->top->li;
  }
  void **next = zltLinkPush(dest, zltLinkPop(src));
  return mylispcPreproc(next, posk, macroTree, src);
}

static void **macroExpand(
  void **dest, mylispcPosStack *posk, mylispcMacroTree *macroTree, const mylispcMacro *macro, const void *src);

void **preprocList(void **dest, mylispcPosStack *posk, mylispcMacroTree *macroTree, void **src, void **first) {
  if (!*first) {
    void **next = zltLinkPush(dest, zltLinkPop(src));
    return mylispcPreproc(next, posk, macroTree, src);
  }
  int clazz = zltMemb(first, mylispcNode, clazz);
  if (clazz == MYLISPC_EOL_ATOM_CLASS) {
    void **next = zltLinkPush(dest, zltLinkPop(first));
    ++posk->top->li;
    return preprocList(next, posk, macroTree, src, first);
  }
  if (clazz == MYLISPC_ID_ATOM_CLASS) {
    mylispcMacroTree *mt = mylispcMacroTreeFind(macroTree, zltMemb(*first, mylispcIDAtom, raw));
    if (!mt) {
      goto A;
    }
    void *src1 = NULL;
    void **next = macroExpand(&src1, posk, macroTree, &mt->macro, zltMemb(*first, zltLink, next));
    if (!next) {
      mylispcNodeClean(src1, NULL);
      return NULL;
    }
    mylispcNodeDelete(zltLinkPop(src));
    *next = *src;
    *src = src1;
    return mylispcPreproc(dest, posk, macroTree, src);
  }
  if (clazz == MYLISPC_LIST_ATOM_CLASS) {
    void *first1 = NULL;
    void **next = preprocList(&first1, posk, macroTree, first, &zltMemb(*first, mylispcListAtom, first));
    if (!next) {
      mylispcNodeClean(first1, NULL);
      return NULL;
    }
    *next = *first;
    return preprocList(dest, posk, macroTree, src, first1);
  }
  A:
  void *first1 = NULL;
  if (!mylispcPreproc(&first1, posk, macroTree, first)) {
    mylispcNodeClean(first1, NULL);
    return NULL;
  }
  zltMemb(*src, mylispcListAtom, first) = first1;
  void **next = zltLinkPush(dest, zltLinkPop(src));
  return mylispcPreproc(next, posk, macroTree, src);
}

typedef struct {
  zltRBTree rbTree;
  zltString name;
  void *node;
} MacroExpTree;



void **macroExpand(
  void **dest, mylispcPosStack *posk, mylispcMacroTree *macroTree, const mylispcMacro *macro, const void *src) {
  ;
}

static bool cloneEmptyListAtom(void **dest);
static bool cloneEOLAtom(void **dest);
static bool cloneIDAtom(void **dest, const mylispcIDAtom *src);
static bool cloneListAtom(void **dest, const mylispcListAtom *src);
static bool cloneNumAtom(void **dest, const mylispcNumAtom *src);
static bool cloneStrAtom(void **dest, const mylispcStrAtom *src);
static bool cloneTokenAtom(void **dest, const mylispcTokenAtom *src);
static void **clones(void **dest, const void *src);

void **mylispcPreproc(void **dest, mylispcPosStack *posk, mylispcMacroTree **macroTree, const void *src) {
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
  void **dest, mylispcPosStack *posk, mylispcMacroTree *macroTree, mylispcMacro *macro, const void *src);
static void **preprocList1(void **dest, mylispcPosStack *posk, mylispcMacroTree *macroTree, const void *src);

void **preprocList(void **dest, mylispcPosStack *posk, mylispcMacroTree *macroTree, const void *src) {
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
