#include"nodes.hh"
#include"token.hh"

using namespace std;

namespace zlt::mylispc {
  using It = const char *;

  static It node(UNode &dest, Context &ctx, It start0, It end);
  static It nodes(UNodes &_0, Context &ctx, It end0, It end);

  void parse(UNodes &dest, Context &ctx, It it, It end) {
    It start1 = nodes(dest, ctx, it, end);
    auto [_1, end1] = lexer(ctx, start1, end);
    if (_1 != token::E0F) {
      reportBad(ctx, bad::UNEXPECTED_TOKEN);
      throw Bad();
    }
  }

  It nodes(UNodes &_0, Context &ctx, It end0, It end) {
    for (;;) {
      UNode _1;
      It start1 = hit(end0, end);
      It end1 = node(_1, ctx, start1, end);
      if (!end1) {
        return start1;
      }
      _0.push_back(std::move(_1));
      end0 = end1;
    }
  }

  It node(UNode &dest, Context &ctx, It start0, It end) {
    double d;
    string s;
    auto [_0, end0] = lexer(d, s, ctx, start0, end);
    if (_0 == token::E0F || _0 == ")"_token) [[unlikely]] {
      return nullptr;
    }
    if (_0 == token::EOL) {
      dest.reset(new EOLAtom);
      ++ctx.pos.li;
      return end0;
    }
    string_view raw0(start0, end0 - start0);
    if (_0 == token::NUMBER) {
      dest.reset(new NumberAtom(addSymbol(ctx, raw0), d));
      return end0;
    }
    if (_0 == token::STRING) {
      auto value = addSymbol(ctx, std::move(s));
      dest.reset(new StringAtom(value));
      return end0;
    }
    if (_0 == token::ID) {
      auto name = addSymbol(ctx, raw0);
      dest.reset(new IDAtom(name));
      return end0;
    }
    if (_0 == "("_token) {
      UNodes _1;
      It start2 = nodes(_1, ctx, end0, end);
      auto [_2, end2] = lexer(ctx, start2, end);
      if (_2 != ")"_token) {
        reportBad(ctx, bad::UNEXPECTED_TOKEN);
        throw Bad();
      }
      dest.reset(new List(std::move(_1)));
      return end2;
    }
    dest.reset(new TokenAtom(_0));
    return end0;
  }
}
