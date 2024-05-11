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

static bool cloneEmptyListAtom(void **dest);
static bool cloneIDAtom(void **dest, const mylispcIDAtom *src);
static bool cloneListAtom(void **dest, const mylispcListAtom *src);
static bool cloneNumAtom(void **dest, const mylispcNumAtom *src);
static bool cloneStrAtom(void **dest, const mylispcStrAtom *src);
static bool cloneTokenAtom(void **dest, const mylispcTokenAtom *src);
static void **clones(void **dest, const void *src);
static void **preprocList(void **dest, mylispcPosStack *posk, mylispcMacroTree *macroTree, const void *src);

void **mylispcPreproc(void **dest, mylispcPosStack *posk, mylispcMacroTree **macroTree, const void *src) {
  if (!src) {
    return dest;
  }
  int clazz = zltMemb(src, mylispcNode, clazz);
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
    if (zltMemb(src, mylispcTokenAtom, token) == MYLISPC_EOL_TOKEN) {
      ++posk.top->li;
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

static void **preprocList1(void **dest, mylispcPosStack *posk, mylispcMacroTree *macroTree, const void *src);

void **preprocList(void **dest, mylispcPosStack *posk, mylispcMacroTree *macroTree, const void *src) {
  if (!src) {
    return cloneEmptyListAtom(dest) ? dest : NULL;
  }
  int clazz = zltMemb(src, mylispcNode, clazz);
  if (clazz == MYLISPC_ID_ATOM_CLASS) {
    mylispcMacroTree *mt = mylispcMacroTreeFind(macroTree, zltMemb(src, mylispcIDAtom, raw));
    if ()
  }
}
