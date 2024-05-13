#ifndef MYLISPC_NODES_H
#define MYLISPC_NODES_H

#include"mylispc.h"

typedef struct {
  mylispcNode node;
  const zltString *raw;
} mylispcIDAtom;

#define mylispcIDAtomMemb(p, m) zltMemb(p, mylispcIDAtom, m)

static inline mylispcIDAtom mylispcIDAtomMake(const zltString *raw) {
  return (mylispcIDAtom) { .node = mylispcNodeMake(MYLISPC_ID_ATOM_CLASS), .raw = raw };
}

typedef struct {
  mylispcNode node;
  void *first;
} mylispcListAtom;

#define mylispcListAtomMemb(p, m) zltMemb(p, mylispcListAtom, m)

static inline mylispcListAtom mylispcListAtomMake(void *first) {
  return (mylispcListAtom) { .node = mylispcNodeMake(MYLISPC_LIST_ATOM_CLASS), .first = first };
}

typedef struct {
  mylispcNode node;
  const zltString *raw;
  double value;
} mylispcNumAtom;

#define mylispcNumAtomMemb(p, m) zltMemb(p, mylispcNumAtom, m)

static inline mylispcNumAtom mylispcNumAtomMake(const zltString *raw, double value) {
  return (mylispcNumAtom) { .node = mylispcNodeMake(MYLISPC_NUM_ATOM_CLASS), .raw = raw, .value = value };
}

typedef struct {
  mylispcNode node;
  const zltString *value;
} mylispcStrAtom;

#define mylispcStrAtomMemb(p, m) zltMemb(p, mylispcStrAtom, m)

static inline mylispcStrAtom mylispcStrAtomMake(const zltString *value) {
  return (mylispcStrAtom) { .node = mylispcNodeMake(MYLISPC_STR_ATOM_CLASS), .value = value };
}

typedef struct {
  mylispcNode node;
  int token;
} mylispcTokenAtom;

#define mylispcTokenAtomMemb(p, m) zltMemb(p, mylispcTokenAtom, m)

static inline mylispcTokenAtom mylispcTokenAtomMake(int token) {
  return (mylispcTokenAtom) { .node = mylispcNodeMake(MYLISPC_TOKEN_ATOM_CLASS), .token = token };
}

#endif
