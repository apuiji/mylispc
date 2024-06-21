#pragma once

#include<cstdio>
#include<cstdlib>
#include"zlt/link.hh"
#include"zlt/set.hh"
#include"zlt/uvw.hh"

namespace zlt::mylispc {
  struct Pos;

  struct Node {
    Link link;
    int clazz;
    const Pos *pos;
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

  void deleteNode(void *node) noexcept;

  static inline void cleanNode(void *node) noexcept {
    link::clean(node, nullptr, deleteNode);
  }

  struct CleanNodeGuard {
    void *&node;
    CleanNodeGuard(void *&node) noexcept: node(node) {}
    ~CleanNodeGuard() {
      cleanNode(node);
    }
  };

  // symbols begin
  /// @throw bad::Fatal
  const String *addSymbol(Set<String> &dest, const String &symbol);

  /// unless symbol already exists, clone and add it
  /// @throw bad::Fatal
  const String *addSymbol1(Set<String> &dest, const String &symbol);

  /// if symbol already exists, free data of param symbol
  /// @throw bad::Fatal
  const String *addSymbol(Set<String> &dest, String &&symbol);
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
