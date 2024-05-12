#ifndef ZLT_LINK_H
#define ZLT_LINK_H

#include"xyz.h"

typedef struct {
  void *next;
} zltLink;

#define zltLinkMemb(p, m) zltMemb(p, zltLink, m)

typedef void zltLinkDtor(void *link);

void zltLinkClean(void *link, const void *end, zltLinkDtor *dtor);

/// @param link requires not null
/// @param last requires not null
/// @return next slot of param last
void **zltLinkInsert(void **dest, void *link, void *last);

/// @param link requires not null
/// @return next slot of param link
static inline void **zltLinkPush(void **dest, void *link) {
  return zltLinkInsert(dest, link, link);
}

/// @param link requires not empty
/// @param last requires not null
void *zltLinkErase(void **link, void *last);

/// @param link requires not empty
static inline void *zltLinkPop(void **link) {
  return zltLinkErase(link, *link);
}

typedef struct {
  void *next;
  void *prev;
} zltDbLink;

#define zltDbLinkMemb(p, m) zltMemb(p, zltDbLink, m)

#endif
