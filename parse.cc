#include"nodes.hh"
#include"parse.hh"
#include"token.hh"

namespace zlt::mylispc {
  using Context = ParseContext;

  static void node(void *&dest, Context &ctx, FILE *src);
  static void nodes(void *&dest, Context &ctx, FILE *src);

  void parse(void *&dest, Context &ctx, FILE *src) {
    nodes(dest, ctx, src);
    int token1 = lexer(ctx, src);
    if (token1 != token::E0F) {
      throw bad::makeFat(bad::UNEXPECTED_TOKEN_FAT, ctx.pos);
    }
  }

  void nodes(void *&dest, Context &ctx, FILE *src) {
    hit(src);
    node(dest, ctx, src);
    if (!dest) {
      return;
    }
    auto &next = memberOf(dest, &Link::next);
    nodes(next, ctx, src);
  }

  void node(void *&dest, Context &ctx, FILE *src) {
    double numval0;
    auto strval0 = string::make();
    const String *raw0;
    FreeGuard g(strval0.data);
    int token0 = lexer(numval0, strval0, raw0, ctx, src);
    if (token0 == token::E0F || token0 == ")"_token) [[unlikely]] {
      return;
    }
    auto pos0 = addPos(ctx.poss, ctx.pos);
    if (token0 == token::EOL) {
      dest = neo<EOLAtom>();
      pointTo<EOLAtom>(dest) = makeEOLAtom(pos0);
      ++ctx.pos.li;
      return;
    }
    if (token0 == token::NUMBER) {
      dest = neo<NumberAtom>();
      pointTo<NumberAtom>(dest) = makeNumberAtom(pos0, raw0, numval0);
      return;
    }
    if (token0 == token::STRING) {
      auto value = addSymbol(ctx.symbols, std::move(strval0));
      dest = neo<StringAtom>();
      pointTo<StringAtom>(dest) = makeStringAtom(pos0, value);
      return;
    }
    if (token0 == token::ID) {
      dest = neo<IDAtom>();
      pointTo<IDAtom>(dest) = makeIDAtom(pos0, raw0);
      return;
    }
    if (token0 == "("_token) {
      void *first1 = nullptr;
      CleanNodeGuard g1(first1);
      nodes(first1, ctx, src);
      int token2 = lexer(ctx, src);
      if (token2 != ")"_token) {
        throw bad::makeFat(bad::UNEXPECTED_TOKEN_FAT, ctx.pos);
      }
      dest = neo<List>();
      pointTo<List>(dest) = makeList(pos0, first1);
      first1 = nullptr;
      return;
    }
    dest = neo<TokenAtom>();
    pointTo<TokenAtom>(dest) = makeTokenAtom(pos0, token0);
  }
}
