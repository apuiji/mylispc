#ifndef MYLISPC_H
#define MYLISPC_H

#include"strtree.h"

const char *mylispAddStr(const char *str, size_t size);

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

extern int mylispcLexerToken;
extern double mylispcLexerNumVal;
extern const char *mylispcLexerStrVal;
extern size_t mylispcLexerStrValSize;

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

/// @return null when bad
const char *mylispcLexer(const char *it, const char *end);

/// @return token
int mylispcRawToken(double *numVal, const char *raw, size_t size);

typedef struct mylispcNode mylispcNode;

struct mylispcNode {
  int clazz;
  const char *start;
  mylispcNode *next;
};

void *mylispcMalloc(size_t size);

#define mylispcTalloc(t) ((t *) mylispcMalloc(sizeof(t)))

static inline void mylispcMakeNode(mylispcNode *dest, int clazz, const char *start) {
  dest->clazz = clazz;
  dest->start = start;
  dest->next = NULL;
}

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
int mylispcParse(mylispcNode **dest, const char *it, const char *end);

typedef struct mylispcSource {
  zltStrTree base;
  mylispcNode *parsed;
} mylispcSource;

typedef struct mylispcMacro {
  zltStrTree base;
  mylispcNode *body;
} mylispcMacro;

static inline void mylispcMakeMacro(
  mylispcMacro *dest, zltStrTree *parent, const char *name, size_t nameSize, mylispcMacro *body) {
  zltMakeStrTree(&dest->base, parent, name, nameSize);
  dest->body = body;
}

/// @return non 0 when bad
int mylispcPreproc(mylispcNode **dest, mylispcMacro **macros, mylispcNode *src);

#endif
