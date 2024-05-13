#ifndef MYLISPX_POS_H
#define MYLISPX_POS_H

#include"zlt/stack.h"
#include"zlt/string.h"

typedef struct {
  zltString file;
  int li;
} mylispxPos;

static inline mylispxPos mylispxPosMake(zltString file, int li) {
  return (mylispxPos) { .file = file, .li = li };
}

static inline zltStack mylispxPosStackMake(void *data, size_t size) {
  return zltStackMake(data, sizeof(mylispxPos) * size);
}

static inline mylispxPos mylispxPosStackPeek(const void *k) {
  mylispxPos pos;
  zltStackPeek(&pos, sizeof(mylispxPos), k);
  return pos;
}

static inline bool mylispxPosStackPush(void *k, const mylispxPos *pos) {
  return zltStackPush(k, pos, sizeof(mylispxPos));
}

static inline void mylispxPosStackPop(void *k) {
  zltStackPop(k, sizeof(mylispxPos));
}

static inline bool mylispcxPosStackRealloc(void *k, size_t size) {
  return zltStackRealloc(k, sizeof(mylispxPos) * size);
}

#endif
