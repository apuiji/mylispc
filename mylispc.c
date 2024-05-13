#include"mylispc.h"
#include"nodes.h"
#include"preproc.h"
#include"zlt/io.h"

static void deleteSymbol(void *p) {
  free(zltStrTreeMemb(p, value).data);
  free(p);
}

static void deleteMacro(void *p) {
  mylispcMacroClean(&mylispcMacroTreeMemb(p, macro));
  free(p);
}

void mylispcCtxClean(mylispcContext *ctx) {
  zltBiTreeClean(ctx->symbols, deleteSymbol);
  zltBiTreeClean(ctx->macros, deleteMacro);
  free(ctx->posk.data);
}

static const char *badWhat(int bad);
static void reportBadPos(FILE *dest, const mylispxPos *pos);
static void reportBadPosk(FILE *dest, const mylispxPos *pos, const mylispxPos *end, size_t limit);

void mylispcReportBad(const mylispcContext *ctx, int bad) {
  fputs(badWhat(bad), ctx->err);
  reportBadPos(ctx->err, &ctx->pos);
  reportBadPosk(ctx->err, (const mylispxPos *) ctx->posk.top, (const mylispxPos *) ctx->posk.data, ctx->reportBadPoskLimit);
}

const char *badWhat(int bad) {
  if (bad == MYLISPC_OOM_BAD) {
    return "out of memory";
  }
  if (bad == MYLISPC_MACRO_ALREADY_DEFINED_BAD) {
    return "macro already defined";
  }
  if (bad == MYLISPC_POS_STACK_SIZE_OVER_LIMIT_BAD) {
    return "position stack size over limit";
  }
  if (bad == MYLISPC_STR_LTRL_SIZE_OVER_LIMIT_BAD) {
    return "string literal size over limit";
  }
  if (bad == MYLISPC_UNEXPECTED_TOKEN_BAD) {
    return "unexpected token";
  }
  if (bad == MYLISPC_UNTERMINATED_STR_BAD) {
    return "unterminated string constant literal";
  }
  if (bad == MYLISPC_UNRECOGNIZED_SYMB_BAD) {
    return "unrecognized symbol";
  }
  return "unknown reason";
}

void reportBadPos(FILE *dest, const mylispxPos *pos) {
  fputs(" at ", dest);
  zltFputs(dest, pos->file);
  fputc(':', dest);
  fprintf(dest, "%d\n", pos->li);
}

void reportBadPosk(FILE *dest, const mylispxPos *pos, const mylispxPos *end, size_t limit) {
  if (pos == end) {
    return;
  }
  if (!limit) {
    fputs("...\n", dest);
    return;
  }
  reportBadPos(dest, pos);
  reportBadPosk(dest, pos - 1, end, limit - 1);
}

const zltString *mylispcCtxAddSymbol(mylispcContext *ctx, zltString symbol) {
  void *parent = NULL;
  void **slot = zltStrTreeFindForInsert(&parent, &ctx->symbols, symbol);
  if (*slot) {
    free(symbol.data);
    return &zltStrTreeMemb(*slot, value);
  }
  zltStrTree *a = zltTypeAlloc(zltStrTree);
  if (!a) {
    mylispcReportBad(ctx, MYLISPC_OOM_BAD);
    return NULL;
  }
  *a = zltStrTreeMake(parent, symbol);
  *slot = a;
  return &a->value;
}

const zltString *mylispcCtxAddSymbolClone(mylispcContext *ctx, zltString symbol) {
  char *s = (char *) malloc(symbol.size);
  if (!s) {
    mylispcReportBad(ctx, MYLISPC_OOM_BAD);
    return NULL;
  }
  strncpy(s, symbol.data, symbol.size);
  const zltString *s1 = mylispcCtxAddSymbol(ctx, zltStrMake(s, symbol.size));
  if (!s1) {
    free(s);
    return NULL;
  }
  return s1;
}

bool mylispcCtxAddMacro(mylispcContext *ctx, const zltString *name, const void *macro) {
  void *parent = NULL;
  mylispcMacroTree **slot = mylispcMacroTreeFindForInsert(&parent, &ctx->macros, name);
  if (*slot) {
    mylispcReportBad(ctx, MYLISPC_MACRO_ALREADY_DEFINED_BAD);
    return false;
  }
  mylispcMacroTree *a = zltTypeAlloc(mylispcMacroTree);
  if (!a) {
    mylispcReportBad(ctx, MYLISPC_OOM_BAD);
    return false;
  }
  *a = mylispcMacroTreeMake(parent, name, macro);
  *slot = a;
  return true;
}

void mylispcNodeDelete(void *node) {
  int clazz = mylispcNodeMemb(node, clazz);
  if (clazz == MYLISPC_LIST_ATOM_CLASS) {
    mylispcNodeClean(mylispcListAtomMemb(node, first), NULL);
  }
  free(node);
}
