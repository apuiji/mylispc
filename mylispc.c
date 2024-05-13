#include"mylispc.h"
#include"preproc.h"

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
static void reportBadPosk(FILE *dest, const mylispxPos *pos, const mylispxPos *end, size_t n);

void mylispcReportBad(const mylispcCtx *ctx, int bad) {
  fputs(badWhat(bad), ctx->err);
  reportBadPos(ctx->err, &ctx->pos);
  reportBadPosk(ctx->err, (const mylispxPos *) ctx->posk->top, (const mylispxPos *) ctx->posk->data, ctx->reportBadPoskLimit);
}

const char *badWhat(int bad) {
  if (bad == MYLISPC_OOM_BAD) {
    return "out of memory";
  }
  if (bad == MYLISPC_STR_TOO_LONG_BAD) {
    return "string constant literal too long";
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
}

static void reportBadPosk(FILE *dest, const mylispxPos *pos, const mylispxPos *end, size_t n);
