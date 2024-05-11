#ifndef ZLT_LINK_H
#define ZLT_LINK_H

#include"xyz.h"

typedef struct {
  void *next;
} zltLink;

#define zltLinkMemb(p, m) zltMemb(p, zltLink, m)

typedef void zltLinkDtor(void *link);

void zltLinkClean(void *link, zltLinkDtor *dtor);

typedef struct {
  void *next;
  void *prev;
} zltDbLink;

#define zltDbLinkMemb(p, m) zltMemb(p, zltDbLink, m)

#endif
