#ifndef MYLISPC_NODES_H
#define MYLISPC_NODES_H

#include"mylispc.h"

typedef struct {
  mylispcNode node;
  zltString raw;
} mylispcIDAtom;

static inline mylispcIDAtom mylispcIDAtomMake(zltString raw) {
  return (mylispcIDAtom) { .node = mylispcNodeMake(MYLISPC_ID_ATOM_CLASS), .raw = raw };
}

typedef struct {
  mylispcNode node;
  void *first;
} mylispcListAtom;

static inline mylispcListAtom mylispcListAtomMake(void *first) {
  return (mylispcListAtom) { .node = mylispcNodeMake(MYLISPC_LIST_ATOM_CLASS), .first = first };
}

typedef struct {
  mylispcNode node;
  zltString raw;
  double value;
} mylispcNumAtom;

static inline mylispcNumAtom mylispcNumAtomMake(zltString raw, double value) {
  return (mylispcNumAtom) { .node = mylispcNodeMake(MYLISPC_NUM_ATOM_CLASS), .raw = raw, .value = value };
}

typedef struct {
  mylispcNode node;
  zltString value;
} mylispcStrAtom;

static inline mylispcStrAtom mylispcStrAtomMake(zltString value) {
  return (mylispcStrAtom) { .node = mylispcNodeMake(MYLISPC_STR_ATOM_CLASS), .value = value };
}

typedef struct {
  mylispcNode node;
  int token;
} mylispcTokenAtom;

static inline mylispcTokenAtom mylispcTokenAtomMake(int token) {
  return (mylispcTokenAtom) { .node = mylispcNodeMake(MYLISPC_TOKEN_ATOM_CLASS), .token = token };
}

#endif
