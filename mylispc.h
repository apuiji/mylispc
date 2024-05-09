#ifndef MYLISPC_H
#define MYLISPC_H

#include"zlt/link.h"
#include"zlt/rbtree.h"
#include"zlt/string.h"

zltString mylispAddStr(zltString s);

extern int mylispcBad;
extern const char *mylispcBadStart;

enum {
  MYLISPC_NO_BAD,
  MYLISPC_OOM_BAD,
  MYLISPC_STR_TOO_LONG_BAD,
  MYLISPC_UNEXPECTED_TOKEN_BAD,
  MYLISPC_UNTERMINATED_STR_BAD,
  MYLISPC_UNRECOGNIZED_SYMB_BAD
};

const char *mylispcHit(const char *it, const char *end);

enum {
  MYLISPC_EOF_TOKEN,
  MYLISPC_ID_TOKEN,
  MYLISPC_NUM_TOKEN,
  MYLISPC_STR_TOKEN,
  // keywords begin
  MYLISPC_CALLEE_TOKEN,
  MYLISPC_DEF_TOKEN,
  MYLISPC_DEFER_TOKEN,
  MYLISPC_FORWARD_TOKEN,
  MYLISPC_GUARD_TOKEN,
  MYLISPC_IF_TOKEN,
  MYLISPC_LENGTH_TOKEN,
  MYLISPC_RETURN_TOKEN,
  MYLISPC_THROW_TOKEN,
  MYLISPC_TRY_TOKEN,
  // keywords end
  // preproc operations begin
  MYLISPC_POUND_TOKEN,
  MYLISPC_POUND2_TOKEN,
  MYLISPC_POUND_DEF_TOKEN,
  MYLISPC_POUND_IF_TOKEN,
  MYLISPC_POUND_INCLUDE_TOKEN,
  MYLISPC_POUND_MOVE_TOKEN,
  MYLISPC_POUND_UNDEF_TOKEN,
  // preproc operations end
  // symbols begin
  MYLISPC_EXCLAM_TOKEN,
  MYLISPC_PERCENT_TOKEN,
  MYLISPC_AMP2_TOKEN,
  MYLISPC_AMP_TOKEN,
  MYLISPC_LPAREN_TOKEN,
  MYLISPC_RPAREN_TOKEN,
  MYLISPC_ASTERISK2_TOKEN,
  MYLISPC_ASTERISK_TOKEN,
  MYLISPC_PLUS_TOKEN,
  MYLISPC_COMMA_TOKEN,
  MYLISPC_MINUS_TOKEN,
  MYLISPC_DOT_TOKEN,
  MYLISPC_SLASH_TOKEN,
  MYLISPC_LT2_TOKEN,
  MYLISPC_LT_EQ_GT_TOKEN,
  MYLISPC_LT_EQ_TOKEN,
  MYLISPC_LT_TOKEN,
  MYLISPC_EQ2_TOKEN,
  MYLISPC_EQ_TOKEN,
  MYLISPC_GT_EQ_TOKEN,
  MYLISPC_GT3_TOKEN,
  MYLISPC_GT2_TOKEN,
  MYLISPC_GT_TOKEN,
  MYLISPC_AT_TOKEN,
  MYLISPC_CARET2_TOKEN,
  MYLISPC_CARET_TOKEN,
  MYLISPC_VERTICAL2_TOKEN,
  MYLISPC_VERTICAL_TOKEN,
  MYLISPC_TILDE_TOKEN
  // symbols end
};

typedef struct {
  int token;
  union {
    double numVal;
    zltString strVal;
  };
} mylispcLexerDest;

/// @return null when bad
const char *mylispcLexer(mylispcLexerDest *dest, const char *it, const char *end);

void mylispcLexerRaw(mylispcLexerDest *dest, zltString raw);

typedef struct {
  zltLink link;
  int clazz;
  const char *start;
} mylispcNode;

static inline mylispcNode mylispcNodeMake(int clazz, const char *start) {
  return (mylispcNode) { .link = zltLinkMake(NULL), .clazz = clazz, start = start };
}

void mylispcNodeDelete(void *node);

enum {
  // parse productions begin
  MYLISPC_RAW_ATOM_CLASS = 0x100,
  MYLISPC_ID_ATOM_CLASS,
  MYLISPC_NUM_ATOM_CLASS,
  MYLISPC_TOKEN_ATOM_CLASS,
  MYLISPC_LIST_ATOM_CLASS = 1,
  MYLISPC_STR_ATOM_CLASS
  // parse productions end
};

/// @return non 0 when bad
int mylispcParse(void **dest, const char *it, const char *end);

typedef struct {
  zltRBTree rbTree;
  zltString name;
  const zltString *params;
  size_t paramc;
  void *body;
} mylispcMacro;

static inline mylispcMacro mylispcMacroMake(void *parent, zltString name, const zltString *params, size_t paramc, void *body) {
  return (mylispcMacro) { .rbtree = zltRBTreeMake(parent), .name = name, .params = params, .paramc = paramc, .body = body };
}

/// @return non 0 when bad
int mylispcPreproc(void **dest, mylispcMacro **macros, void *src);

#endif
