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
      bad::report(ctx.err, bad::UNEXPECTED_TOKEN_FAT, ctx.pos);
      throw bad::Fatal();
    }
  }

  It nodes(void *&dest, Context &ctx, It end0, It end) {
    It start1 = hit(ctx.pos, end0, end);
    It end1 = node(dest, ctx, start1, end);
    if (!end1) {
      return start1;
    }
    return nodes(member(dest, &Link::next), ctx, end1, end);
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
      auto raw = cloneAndAddSymbol(ctx.symbols, raw0);
      dest.reset(new NumberAtom(pos0, raw, d));
      return end0;
    }
    if (_0 == token::STRING) {
      auto value = addSymbol(ctx.symbols, std::move(s));
      dest.reset(new StringAtom(pos0, value));
      return end0;
    }
    if (_0 == token::ID) {
      auto name = addSymbol(ctx.symbols, raw0);
      dest.reset(new IDAtom(pos0, name));
      return end0;
    }
    if (_0 == "("_token) {
      UNodes _1;
      It start2 = nodes(_1, ctx, end0, end);
      auto [_2, end2] = lexer(ctx, start2, end);
      if (_2 != ")"_token) {
        bad::report(ctx.err, bad::UNEXPECTED_TOKEN, ctx.pos);
        throw bad::Fatal();
      }
      dest.reset(new List(pos0, std::move(_1)));
      return end2;
    }
    dest.reset(new TokenAtom(pos0, _0));
    return end0;
  }
}
