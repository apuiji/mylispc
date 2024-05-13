#ifndef ZLT_STACK_H
#define ZLT_STACK_H

#include<string.h>
#include"xyz.h"

typedef struct {
  void *data;
  void *top;
  size_t left;
} zltStack;

#define zltStackMemb(p, m) zltMemb(p, zltStack, m)

static inline zltStack zltStackMake(void *data, size_t size) {
  return (zltStack) { .data = data, .top = data, .left = size };
}

/// @param[out] dest requires not null
/// @param k requires not null and stored data size greater than param size
static inline void zltStackPeek(void *dest, size_t size, const void *k) {
  memcpy(dest, zltStackMemb(k, top) - size, size);
}

/// @param k requires not null
/// @return false when bad
bool zltStackPush(void *k, const void *data, size_t size);

/// @param k requires not null
/// @param size requires less than param k left
static inline void zltStackPop(void *k, size_t size) {
  zltStackMemb(k, top) -= size;
  zltStackMemb(k, left) += size;
}

/// @param k requires not null
/// @return false when bad
bool zltStackRealloc(void *k, size_t size);

#endif
