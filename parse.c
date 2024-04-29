#include<stdlib.h>
#include"mylispc.h"
#include"nodes.h"

typedef const char *It;

typedef mylispcNode Node;

/// @return node end when successful, null when unmatched, -1 when bad
static It node(Node **dest, It start, It end);
/// @return next start when successful, -1 when bad
static It nodes(Node **dest, It it, It end);

int mylispcParse(Node **dest, It it, It end) {
  It end1 = nodes(dest, it, end);
  if (end1 == (It) -1) {
    return -1;
  }
  It start2 = mylispcHit(end1, end);
  mylispcLexerStrVal = NULL;
  It end2 = mylispcLexer(start2, end);
  free(mylispcLexerStrVal);
  if (!end2) {
    return -1;
  }
  if (mylispcLexerToken != MYLISPC_EOF_TOKEN) {
    mylispcBad = MYLISPC_UNEXPECTED_TOKEN_BAD;
    mylispcBadStart = start2;
    return -1;
  }
  return 0;
}

/// @return node end when successful, -1 when bad
static It listAtom(Node **dest, It start, It it, It end);

/// @return node end when successful, -1 when bad
static It strAtom(Node **dest, It start, It end);

It node(Node **dest, It start, It end) {
  It end1 = mylispcLexer(start, end);
  if (mylispcLexerToken == MYLISPC_EOF_TOKEN || mylispcLexerToken == MYLISPC_RPAREN_TOKEN) {
    return NULL;
  }
  if (mylispcLexerToken == MYLISPC_ID_TOKEN) {
    mylispcIDAtom *a = mylispcTalloc(mylispcIDAtom);
    if (!a) {
      return (It) -1;
    }
    mylispcMakeIDAtom(a, start, end1 - start);
    *dest = (Node *) a;
    return end1;
  }
  if (mylispcLexerToken == MYLISPC_NUM_TOKEN) {
    mylispcNumAtom *a = mylispcTalloc(mylispcNumAtom);
    if (!a) {
      return (It) -1;
    }
    mylispcMakeNumAtom(a, start, end1 - start, mylispcLexerNumVal);
    *dest = (Node *) a;
    return end1;
  }
  if (mylispcLexerToken == MYLISPC_STR_TOKEN) {
    return strAtom(dest, start, end1);
  }
  if (mylispcLexerToken == MYLISPC_LPAREN_TOKEN) {
    return listAtom(dest, start, end1, end);
  }
  mylispcTokenAtom *a = mylispcTalloc(mylispcTokenAtom);
  if (!a) {
    return (It) -1;
  }
  mylispcMakeTokenAtom(a, start, end1 - start, mylispcLexerToken);
  *dest = (Node *) a;
  return end1;
}

It listAtom(Node **dest, It start, It it, It end) {
  mylispcListAtom *a = mylispcTalloc(mylispcListAtom);
  if (!a) {
    mylispcBad = MYLISPC_OOM_BAD;
    goto BAD1;
  }
  Node *first = NULL;
  It end1 = nodes(&first, it, end);
  if (end1 == (It) -1) {
    goto BAD2;
  }
  It start2 = mylispcHit(end1, end);
  mylispcLexerStrVal = NULL;
  It end2 = mylispcLexer(start2, end);
  free(mylispcLexerStrVal);
  if (!end2) {
    goto BAD2;
  }
  if (mylispcLexerToken != MYLISPC_RPAREN_TOKEN) {
    mylispcBad = MYLISPC_UNEXPECTED_TOKEN_BAD;
    mylispcBadStart = start2;
    goto BAD2;
  }
  mylispcMakeListAtom(a, start, first);
  *dest = (Node *) a;
  return end2;
  BAD2:
  mylispcNodeClean(first);
  free(a);
  BAD1:
  return (It) -1;
}

It strAtom(Node **dest, It start, It end) {
  mylispcStrAtom *a = mylispcTalloc(mylispcStrAtom);
  if (!a) {
    mylispcBad = MYLISPC_OOM_BAD;
    goto BAD1;
  }
  const char *value = mylispAddStr(mylispcLexerStrVal, mylispcLexerStrValSize);
  if (!value) {
    goto BAD2;
  }
  mylispcMakeStrAtom(a, value, mylispcLexerStrValSize);
  *dest = (Node *) a;
  return end;
  BAD2:
  free(a);
  BAD1:
  free(mylispcLexerStrVal);
  return (It) -1;
}

It nodes(Node **dest, It it, It end) {
  It start1 = mylispcHit(it, end);
  It end1 = node(dest, start1, end);
  if (!end1) {
    return start1;
  }
  if (end1 == (It) -1) {
    return (It) -1;
  }
  return nodes(&(**dest).next, end1, end);
}
