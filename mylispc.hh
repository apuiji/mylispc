#pragma once

#include<list>
#include<ostream>
#include<string>
#include"zlt/myset.hh"

namespace zlt::mylispc {
  struct Node {
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
  };

  struct PosStack {
    std::unique_ptr<Pos[]> data;
    Pos *top;
    size_t left;
  };

  void pushPos(PosStack &posk, const Pos &pos);
  Pos popPos(PosStack &posk);
  // positions end

  // bads begin
  void reportBad(std::ostream &dest, int bad, const Pos &pos, const PosStack &posk);

  struct Bad {
    // empty
  };

  namespace bad {
    enum {
      NO_BAD,
      WARN = 0x100,
      NUMBER_LITERAL_OOR,
      ERROR = 0x200,
      INV_PREPROC_ARG,
      MACRO_ALREADY_DEFINED,
      FATAL = 0x300,
      CANNOT_OPEN_FILE,
      ILL_FN_PARAM,
      ILL_MACRO_PARAM,
      UNEXPECTED_TOKEN,
      UNRECOGNIZED_SYMBOL,
      UNTERMINATED_STRING
    };

    static inline int level(int bad) noexcept {
      return bad & 0xf00;
    }
  }
  // bads end
}
