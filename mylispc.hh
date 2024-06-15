#pragma once

#include<cstdio>
#include"zlt/link.hh"
#include"zlt/set.hh"

namespace zlt::mylispc {
  struct Pos;

  struct Node {
    Link link;
    int clazz;
    const Pos *pos;
  };

  static inline Node makeNode(int clazz, const Pos *pos) noexcept {
    return (Node) { .link = link::make(nullptr), .clazz = clazz, .pos = pos };
  }

  void unmakeNode(void *node);
  void cleanNode(void *node);

  // symbols begin
  const String *addSymbol(Set<String> &dest, const String &symbol);
  const String *cloneAndAddSymbol(Symbols &dest, const String &symbol);
  // symbols end

  // positions begin
  struct Pos {
    Link link;
    const String *file;
    int li;
  };

  static inline Pos makePos(const String *file, int li, const void *next) noexcept {
    return (Pos) { .link = link::make(next), .file = file, .li = li };
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
      INV_PREPROC_ARG_ERR,
      MACRO_ALREADY_DEFINED_ERR,
      MACRO_UNDEFINED_ERR,
      UNEXPECTED_TRANS_TOKEN_ERR,
      FATAL = 0x300,
      OOM_FAT,
      UNEXPECTED_TOKEN_FAT,
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

  void compile(FILE *dest, const Node *&src);
}
