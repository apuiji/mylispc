#include"nodes.hh"
#include"parse.hh"
#include"token.hh"

namespace zlt::mylispc {
  using Context = ParseContext;
  using It = const char *;

  static It node(void *&dest, Context &ctx, It start0, It end);
  static It nodes(void *&dest, Context &ctx, It end0, It end);

  void parse(void *&dest, Context &ctx, It it, It end) {
    int token1;
    It start1 = nodes(dest, ctx, it, end);
    It end1 = lexer(token1, ctx, start1, end);
    if (token1 != token::E0F) {
      throw bad::makeFat(bad::UNEXPECTED_TOKEN_FAT, ctx.pos);
    }
  }

  It nodes(void *&dest, Context &ctx, It end0, It end) {
    It start1 = hit(ctx.pos, end0, end);
    It end1 = node(dest, ctx, start1, end);
    if (!end1) {
      return start1;
    }
    return nodes(memberOf(dest, &Link::next), ctx, end1, end);
  }

  It node(void *&dest, Context &ctx, It start0, It end) {
    int token0;
    double numval0;
    auto strval0 = string::make();
    FreeGuard fg(strval0.data);
    It end0 = lexer(token0, numval0, strval0, ctx, start0, end);
    if (token0 == token::E0F || token0 == ")"_token) [[unlikely]] {
      return nullptr;
    }
    auto pos0 = addPos(ctx.poss, ctx.pos);
    auto raw0 = string::make(start0, end0);
    if (token0 == token::NUMBER) {
      auto raw = addSymbol1(ctx.symbols, raw0);
      dest = neo<NumberAtom>();
      pointTo<NumberAtom>(dest) = makeNumberAtom(pos0, raw, numval0);
      return end0;
    }
    if (token0 == token::STRING) {
      auto value = addSymbol(ctx.symbols, strval0);
      strval0.data = nullptr;
      dest = neo<StringAtom>();
      pointTo<StringAtom>(dest) = makeStringAtom(pos0, value);
      return end0;
    }
    if (token0 == token::ID) {
      auto name = addSymbol1(ctx.symbols, raw0);
      dest = neo<IDAtom>();
      pointTo<IDAtom>(dest) = makeIDAtom(pos0, name);
      return end0;
    }
    if (token0 == "("_token) {
      void *first1 = nullptr;
      CleanNodeGuard g(first1);
      It start2 = nodes(first1, ctx, end0, end);
      int token2;
      It end2 = lexer(token2, ctx, start2, end);
      if (token2 != ")"_token) {
        throw bad::makeFat(bad::UNEXPECTED_TOKEN, ctx.pos);
      }
      dest = neo<List>();
      pointTo<List>(dest) = makeList(pos0, first);
      first1 = nullptr;
      return end2;
    }
    dest = neo<TokenAtom>();
    pointTo<TokenAtom>(dest) = makeTokenAtom(pos0, token0);
    return end0;
  }
}
