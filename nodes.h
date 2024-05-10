#ifndef MYLISPC_NODES_H
#define MYLISPC_NODES_H

#include"mylispc.h"

typedef struct {
  mylispcNode node;
  zltString raw;
} mylispcRawAtom;

static inline mylispcRawAtom mylispcRawAtomMake(int clazz, zltString raw) {
  return (mylispcRawAtom) { .node = mylispcNodeMake(clazz), .raw = raw };
}

typedef mylispcRawAtom mylispcIDAtom;

static inline mylispcIDAtom mylispcIDAtomMake(zltString raw) {
  return mylispcRawAtomMake(MYLISPC_ID_ATOM_CLASS, raw);
}

typedef struct {
  mylispcNode node;
  void *first;
} mylispcListAtom;

static inline mylispcListAtom mylispcListAtomMake(void *first) {
  return (mylispcListAtom) { .node = mylispcNodeMake(MYLISPC_LIST_ATOM_CLASS), .first = first };
}

typedef struct {
  mylispcRawAtom rawAtom;
  double value;
} mylispcNumAtom;

static inline mylispcNumAtom mylispcNumAtomMake(zltString raw, double value) {
  return (mylispcNumAtom) { .rawAtom = mylispcRawAtomMake(MYLISPC_NUM_ATOM_CLASS, raw), .value = value };
}

typedef struct {
  mylispcNode node;
  zltString value;
} mylispcStrAtom;

static inline mylispcStrAtom mylispcStrAtomMake(zltString value) {
  return (mylispcStrAtom) { .node = mylispcNodeMake(MYLISPC_STR_ATOM_CLASS), .value = value };
}

typedef struct {
  mylispcRawAtom rawAtom;
  int token;
} mylispcTokenAtom;

mylispcTokenAtom mylispcTokenAtomMake(int token);

#endif
