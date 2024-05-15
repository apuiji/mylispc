#pragma once

#include"rbtree.hh"

namespace zlt {
  namespace myset {
    template<class T>
    struct Node;
  }

  template<class T>
  struct MySet {
    using Node = myset::Node<T>;
    using Value = T;
    void *root;
  };
}

namespace zlt::myset {
  template<class T>
  struct Node: RBTree {
    using Value = T;
    T value;
    Node() = default;
    Node(const T &value): value(value) {}
    Node(T &&value) noexcept: value(std::move(value)) {}
  };

  template<class T, class Del = std::default_delete<Node<T>>>
  static inline void clean(MySet<T> &set, const Del &del = {}) noexcept {
    bitree::clean(set.root, del);
  }

  template<class T, class Cmp>
  struct ValueCompare {
    const Cmp &cmp;
    ValueCompare(const Cmp &cmp) noexcept: cmp(cmp) {}
    template<class U>
    auto operator ()(U &&u, const void *tree) const noexcept {
      return cmp(std::forward<U>(u), member(tree, &Node<T>::value));
    }
  };

  template<class T, class U, class Cmp = Compare>
  static inline void *find(const MySet<T> &set, U &&u, const Cmp &cmp = {}) noexcept {
    return bitree::find(set.root, ValueCompare<T, Cmp>(cmp), std::forward<U>(u));
  }

  template<class T, class U, class Cmp = Compare>
  static inline void *&findForInsert(void *&parent, MySet<T> &set, U &&u, const Cmp &cmp = {}) noexcept {
    return *bitree::findForInsert(parent, set.root, ValueCompare<T, Cmp>(cmp), std::forward<U>(u));
  }
}
