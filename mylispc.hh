#pragma once

#include<cstdio>
#include<cstdlib>
#include"zlt/link.hh"
#include"zlt/set.hh"
#include"zlt/xyz.hh"

namespace zlt::mylispc {
  struct Pos;

  struct Node {
    int clazz;
    const Pos *pos;
    Link link;
  };

  enum {
    EOL_ATOM_CLASS,
    ID_ATOM_CLASS,
    NUM_ATOM_CLASS,
    LIST_CLASS,
    STR_ATOM_CLASS,
    TOKEN_ATOM_CLASS
  };

  static inline Node makeNode(int clazz, const Pos *pos) noexcept {
    return (Node) { .link = link::make(), .clazz = clazz, .pos = pos };
  }

  void deleteNode(Node *node) noexcept;
  void cleanNode(Node *node) noexcept;

  struct DeleteNodeGuard {
    Node *&node;
    DeleteNodeGuard(Node *&node) noexcept: node(node) {}
    ~DeleteNodeGuard() {
      deleteNode(node);
    }
  };

  struct CleanNodeGuard {
    Node *&node;
    CleanNodeGuard(Node *&node) noexcept: node(node) {}
    ~CleanNodeGuard() {
      cleanNode(node);
    }
  };

  static inline Node *nextNode(const Node *node) noexcept {
    return containerOf(node->link.next, &Node::link);
  }

  // symbols begin
  /// @throw bad::Fatal
  const String *addSymbol(Set<String> &dest, const String &symbol);

  /// unset symbol data when already exists
  /// @throw bad::Fatal
  const String *moveSymbol(Set<String> &dest, String &symbol);

  /// unset symbol data when already exists
  /// @throw bad::Fatal
  const String *moveSymbol(Set<String> &dest, String &&symbol) {
    FreeGuard fg(symbol.data);
    return moveSymbol(dest, symbol);
  }
  // symbols end

  // positions begin
  struct Pos {
    Link link;
    const String *file;
    int li;
  };

  static inline Pos makePos(const String *file, int li, const Pos *up = nullptr) noexcept {
    return (Pos) { .link = link::make(up), .file = file, .li = li };
  }

  static inline Pos makePos(const Pos *up, const Pos &pos) noexcept {
    return makePos(pos.file, pos.li, up);
  }

  static inline Pos makePos(const Pos *up, const Pos *pos) noexcept {
    return makePos(up, *pos);
  }

  const Pos *addPos(Set<Pos> &poss, const Pos &pos);
  // positions end

  // bads begin
  namespace bad {
    enum {
      NO_BAD,
      WARN = 0x100,
      NUM_LITERAL_OOR_WARN,
      ERROR = 0x200,
      CANNOT_INCLUDE_ERR,
      ILL_ASSIGN_ERR,
      ILL_FN_PARAM_ERR,
      ILL_MACRO_PARAM_ERR,
      INV_LHS_ERR,
      MACRO_ALREADY_DEFINED_ERR,
      MACRO_UNDEFINED_ERR,
      UNEXPECTED_TOKEN_ERR,
      UNEXPECTED_TRANS_TOKEN_ERR,
      FATAL = 0x300,
      OOM_FAT,
      UNEXPECTED_TOKEN_FAT,
      UNKNOWED_REASON_FAT,
      UNRECOGNIZED_SYMB_FAT,
      UNTERMINATED_STR_FAT
    };

    static inline int level(int bad) noexcept {
      return bad & 0xf00;
    }

    void report(FILE *dest, int bad, const Pos *pos = nullptr);

    static inline void report(FILE *dest, int bad, const Pos &pos) {
      report(dest, bad, &pos);
    }

    struct Fatal {
      int code;
      const Pos *pos;
    };

    static inline Fatal makeFat(int code, const Pos *pos = nullptr) noexcept {
      return (Fatal) { .code = code, .pos = pos };
    }
  }
  // bads end

  template<class T>
  static inline T *neo() {
    auto t = typeAlloc<T>();
    if (!t) [[unlikely]] {
      throw bad::Fatal(bad::OOM_FAT);
    }
    return t;
  }

  void compile(FILE *dest, const Node *&src);
}
