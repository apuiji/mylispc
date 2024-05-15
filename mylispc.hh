#pragma once

#include<filesystem>
#include<iostream>
#include<list>
#include<map>
#include<memory>
#include<set>
#include<string>
#include<type_traits>
#include<vector>
#include"zlt/myset.hh"

namespace zlt::mylispc {
  struct Node {
    virtual ~Node() = default;
  };

  using UNode = std::unique_ptr<Node>;
  using UNodes = std::list<UNode>;

  struct Pos {
    const std::string *file;
    int li;
  };

  struct PosStack {
    std::unique_ptr<Pos[]> data;
    Pos *top;
    size_t left;
  };

  void push(PosStack &posk, const Pos &pos);
  Pos pop(PosStack &posk);

  struct Macro {
    using Params = std::vector<const std::string *>;
    using ItParam = Params::const_iterator;
    Pos pos;
    Params params;
    UNodes body;
    Macro() = default;
  };

  struct Context {
    std::ostream &out;
    std::ostream &err;
    MySet<std::string> symbols;
    PosStack posk;
    Pos pos;
    std::map<const std::string *, Macro> macros;
    Context(std::ostream &out, std::ostream &err) noexcept: out(out), err(err) {}
    Context(std::ostream &out) noexcept: Context(out, out) {}
  };

  void reportBad(Context &ctx, int bad);

  const char *hit(const char *it, const char *end) noexcept;

  /// @param[out] numval when token is NUMBER
  /// @param[out] strval when token is STRING
  /// @return [token, end]
  std::pair<int, const char *> lexer(double &numval, std::string &strval, Context &ctx, const char *it, const char *end);

  static inline std::pair<int, const char *> lexer(Context &ctx, const char *it, const char *end) {
    double d;
    std::string s;
    return lexer(d, s, ctx, it, end);
  }

  void parse(UNodes &dest, Context &ctx, const char *it, const char *end);
  void preproc(UNodes &dest, Context &ctx, const UNode &src);

  static inline void preproc(UNodes &dest, Context &ctx, UNodes::const_iterator it, UNodes::const_iterator end) {
    for (; it != end; ++it) {
      preproc(dest, ctx, *it);
    }
  }

  void trans(Context &ctx, UNodes &src);
  void optimize(Context &ctx, UNode &src);

  template<class It>
  requires (std::is_same_v<std::iter_value_t<It>, UNode>)
  static inline void optimize(Context &ctx, It it, It end) {
    for (; it != end; ++it) {
      optimize(*it);
    }
  }

  /// @return terminated
  bool optimizeBody(UNodes &dest, Context &ctx, UNodes::iterator it, UNodes::iterator end);

  /// @param[out] dest main function
  void trans1(UNode &dest, Context &ctx, UNodes::iterator it, UNodes::iterator end);

  void trans2(Context &ctx, UNode &src);

  void compile(Context &ctx, const UNode &src);

  struct Bad {};

  namespace bad {
    enum {
      // warns begin
      WARN,
      NUMBER_LITERAL_OOR,
      // warns end
      ERROR = 0x100,
      CANNOT_OPEN_FILE,
      ILL_FN_PARAM,
      ILL_MACRO_PARAM,
      INV_PREPROC_ARG,
      MACRO_ALREADY_DEFINED,
      UNEXPECTED_TOKEN,
      UNRECOGNIZED_SYMBOL,
      UNTERMINATED_LIST,
      UNTERMINATED_STRING
    };
  }
}
