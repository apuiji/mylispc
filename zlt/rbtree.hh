#pragma once

#include"bitree.hh"
#include"rbtree.h"

namespace zlt {
  using RBTree = zltRBTree;
}

namespace zlt::rbtree {
  /// @see zltRBTreeMake
  static inline RBTree make(const void *parent) noexcept {
    return zltRBTreeMake(parent);
  }

  /// @see zltRBTreeSwap
  static inline void swap(void *&root, void *a, void *b) noexcept {
    zltRBTreeSwap(&root, a, b);
  }

  /// @see zltRBTreeAfterInsert
  static inline void afterInsert(void *&root, void *node) noexcept {
    zltRBTreeAfterInsert(&root, node);
  }

  /// @see zltRBTreeBeforeErase
  static inline void beforeErase(void *&root, void *node) noexcept {
    zltRBTreeBeforeErase(&root, node);
  }

  /// @see zltRBTreeErase
  static inline void erase(void *&root, void *node) noexcept {
    return zltRBTreeErase(&root, node);
  }
}
