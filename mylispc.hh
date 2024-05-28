#pragma once

#include<list>
#include<ostream>
#include<set>
#include<string>
#include"zlt/myset.hh"

namespace zlt::mylispc {
  struct Pos;

  struct Node {
    const Pos *pos;
    Node(const Pos *pos = nullptr) noexcept: pos(pos) {}
    virtual ~Node() = default;
  };

  using UNode = std::unique_ptr<Node>;
  using UNodes = std::list<UNode>;

  // symbols begin
  using Symbols = MySet<std::string>;

  const std::string *addSymbol(Symbols &dest, std::string &&symbol);

  static inline const std::string *addSymbol(Symbols &dest, std::string_view symbol) {
    return addSymbol(dest, std::string(symbol));
  }
  // symbols end

  // positions begin
  struct Pos {
    const std::string *file;
    int li;
    Pos(const std::string *file, int li) noexcept: file(file), li(li) {}
  };

  struct PosSetComp {
    bool operator ()(const Pos &a, const Pos &b) const noexcept {
      return a.file < b.file || a.li < b.li;
    }
  };

  using PosSet = std::set<Pos, PosSetComp>;

  const Pos *addPos(PosSet &poss, const Pos &pos);
  // positions end

  // bads begin
  namespace bad {
    enum {
      NO_BAD,
      WARN = 0x100,
      NUMBER_LITERAL_OOR,
      ERROR = 0x200,
      CANNOT_INCLUDE,
      ILL_ASSIGN,
      ILL_FN_PARAM,
      ILL_MACRO_PARAM,
      INV_LHS,
      INV_PREPROC_ARG,
      MACRO_ALREADY_DEFINED,
      MACRO_UNDEFINED,
      UNEXPECTED_TRANS_TOKEN,
      FATAL = 0x300,
      UNEXPECTED_TOKEN,
      UNRECOGNIZED_SYMBOL,
      UNTERMINATED_STRING
    };

    static inline int level(int bad) noexcept {
      return bad & 0xf00;
    }

    void report(std::ostream &dest, int bad, const Pos &pos);

    static inline void report(std::ostream &dest, int bad, const Pos *pos) {
      report(dest, bad, *pos);
    }

    struct Fatal {
      // empty
    };
  }
  // bads end
}
