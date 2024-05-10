#ifndef MYLISP_BAD_H
#define MYLISP_BAD_H

#include"zlt/string.h"

extern int mylispBad;

enum {
  MYLISP_NO_BAD,
  MYLISP_OOM_BAD,
  MYLISP_STR_TOO_LONG_BAD,
  MYLISPC_UNEXPECTED_TOKEN_BAD,
  MYLISPC_UNTERMINATED_STR_BAD,
  MYLISPC_UNRECOGNIZED_SYMB_BAD
};

zltString mylispAddStr(zltString s);

#endif
