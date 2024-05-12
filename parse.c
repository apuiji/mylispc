#include<stdlib.h>
#include"mylisp/mylisp.h"
#include"mylispc.h"
#include"nodes.h"
#include"token.h"

typedef const char *It;

/// @return node end when successful, null when unmatched, -1 when bad
static It node(void **dest, mylispcPos *pos, It start, It end);
/// @return next start when successful, -1 when bad
static It nodes(void **dest, mylispcPos *pos, It it, It end);

static inline void freeIfStr(mylispcLexerDest *dest) {
  if (dest->token == MYLISPC_STR_TOKEN) {
    free(dest->strVal.data);
  }
}

bool mylispcParse(void **dest, mylispcPos *pos, It it, It end) {
  It start2 = nodes(dest, pos, it, end);
  if (start2 == (It) -1) {
    return false;
  }
  mylispcLexerDest prod2 = {};
  It end2 = mylispcLexer(&prod2, start2, end);
  if (!end2) {
    return false;
  }
  if (prod2.token != MYLISPC_EOF_TOKEN) {
    freeIfStr(&prod2);
    mylispBad = MYLISPC_UNEXPECTED_TOKEN_BAD;
    return false;
  }
  return true;
}

/// @return node end when successful, -1 when bad
static It listAtom(void **dest, mylispcPos *pos, It it, It end);

It node(void **dest, mylispcPos *pos, It start, It end) {
  mylispcLexerDest prod1 = {};
  It end1 = mylispcLexer(&prod1, start, end);
  if (prod1.token == MYLISPC_EOF_TOKEN || prod1.token == MYLISPC_RPAREN_TOKEN) {
    return NULL;
  }
  if (prod1.token == MYLISPC_EOL_TOKEN) {
    mylispcNode *a = zltTypeAlloc(mylispcNode);
    if (!a) {
      mylispBad = MYLISP_OOM_BAD;
      return (It) -1;
    }
    *a = mylispcNodeMake(MYLISPC_EOL_ATOM_CLASS);
    *dest = a;
    ++pos->li;
    return end1;
  }
  if (prod1.token == MYLISPC_ID_TOKEN) {
    mylispcIDAtom *a = zltTypeAlloc(mylispcIDAtom);
    if (!a) {
      mylispBad = MYLISP_OOM_BAD;
      return (It) -1;
    }
    zltString id = mylispAddStr(zltStrMakeBE(start, end1));
    if (!id.data) {
      return (It) -1;
    }
    *a = mylispcIDAtomMake(id);
    *dest = a;
    return end1;
  }
  if (prod1.token == MYLISPC_NUM_TOKEN) {
    mylispcNumAtom *a = zltTypeAlloc(mylispcNumAtom);
    if (!a) {
      mylispBad = MYLISP_OOM_BAD;
      return (It) -1;
    }
    zltString raw = mylispAddStr(zltStrMakeBE(start, end1));
    if (!raw.data) {
      return (It) -1;
    }
    *a = mylispcNumAtomMake(raw, prod1.numVal);
    *dest = a;
    return end1;
  }
  if (prod1.token == MYLISPC_STR_TOKEN) {
    mylispcStrAtom *a = zltTypeAlloc(mylispcStrAtom);
    if (!a) {
      mylispBad = MYLISP_OOM_BAD;
      free(prod1.strVal.data);
      return (It) -1;
    }
    zltString s = mylispAddStr(prod1.strVal);
    if (!s.data) {
      free(prod1.strVal.data);
      return (It) -1;
    }
    *a = mylispcStrAtomMake(s);
    *dest = a;
    return end1;
  }
  if (prod1.token == MYLISPC_LPAREN_TOKEN) {
    return listAtom(dest, pos, end1, end);
  }
  mylispcTokenAtom *a = zltTypeAlloc(mylispcTokenAtom);
  if (!a) {
    mylispBad = MYLISP_OOM_BAD;
    return (It) -1;
  }
  *a = mylispcTokenAtomMake(prod1.token);
  *dest = a;
  return end1;
}

It listAtom(void **dest, mylispcPos *pos, It it, It end) {
  mylispcListAtom *a = zltTypeAlloc(mylispcListAtom);
  if (!a) {
    mylispBad = MYLISP_OOM_BAD;
    goto BAD1;
  }
  void *first = NULL;
  It start2 = nodes(&first, pos, it, end);
  if (start2 == (It) -1) {
    goto BAD2;
  }
  mylispcLexerDest prod2 = {};
  It end2 = mylispcLexer(&prod2, start2, end);
  if (!end2) {
    goto BAD2;
  }
  if (prod2.token != MYLISPC_RPAREN_TOKEN) {
    mylispBad = MYLISPC_UNEXPECTED_TOKEN_BAD;
    freeIfStr(&prod2);
    goto BAD2;
  }
  *a = mylispcListAtomMake(first);
  *dest = a;
  return end2;
  BAD2:
  mylispcNodeClean(first, NULL);
  free(a);
  BAD1:
  return (It) -1;
}

It nodes(void **dest, mylispcPos *pos, It it, It end) {
  It start1 = mylispcHit(it, end);
  It end1 = node(dest, pos, start1, end);
  if (!end1) {
    return start1;
  }
  if (end1 == (It) -1) {
    return (It) -1;
  }
  return nodes(&zltMemb(*dest, zltLink, next), pos, end1, end);
}
