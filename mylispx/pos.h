#ifndef MYLISPX_POS_H
#define MYLISPX_POS_H

#include"zlt/string.h"

typedef struct {
  zltString file;
  int li;
} mylispxPos;

static inline mylispxPos mylispxPosMake(zltString file, int li) {
  return (mylispxPos) { .file = file, .li = li };
}

typedef struct {
  mylispxPos *data;
  mylispxPos *top;
  size_t left;
} mylispxPosStack;

static inline mylispxPosStack mylispxPosStackMake(mylispxPos *data, size_t size) {
  return (mylispxPosStack) { .data = data, .top = data, .left = size };
}

bool mylispxPosPush(mylispxPosStack *k, const mylispxPos *pos);
mylispxPos mylispxPosPop(mylispxPosStack *k);

#endif
