#include"myutils/xyz.hh"
#include"mylisp.hh"
#include"nodes.hh"
#include"token.hh"

using namespace std;

namespace zlt::mylispc {
  using It = const char *;

  static It node(UNode &dest, It start0, It end);
  static It nodes(UNodes &_0, It end0, It end);

  static inline void expect(bool b, const char *start) {
    if (!b) {
      throw Bad(bad::UNEXPECTED_TOKEN, start);
    }
  }

  void parse(UNodes &dest, It it, It end) {
    It end0 = nodes(dest, it, end);
    It start1 = hit(end0, end);
    auto [_1, end1] = lexer(start1, end);
    expect(_1 == token::E0F, start1);
  }

  It nodes(UNodes &_0, It end0, It end) {
    for (;;) {
      UNode _1;
      It start1 = hit(end0, end);
      It end1 = node(_1, start1, end);
      if (!end1) {
        break;
      }
      _0.push_back(std::move(_1));
      end0 = end1;
    }
    return end0;
  }

  It node(UNode &dest, It start0, It end) {
    double d;
    char c;
    string s;
    auto [_0, end0] = lexer(d, c, s, start0, end);
    if (_0 == token::E0F || _0 == ")"_token) {
      return nullptr;
    }
    string_view raw0(start0, end0 - start0);
    if (_0 == token::NUMBER) {
      dest.reset(new NumberAtom(start0, raw0, d));
      return end0;
    }
    if (_0 == token::STRING) {
      auto value = mylisp::addString(std::move(s));
      dest.reset(new StringAtom(start0, value));
      return end0;
    }
    if (_0 == token::ID) {
      auto name = mylisp::addString(string(raw0));
      dest.reset(new IDAtom(start0, raw0, name));
      return end0;
    }
    if (_0 == "("_token) {
      UNodes _1;
      It end1 = nodes(_1, end0, end);
      It start2 = hit(end1, end);
      auto [_2, end2] = lexer(start2, end);
      if (_2 != ")"_token) {
        throw Bad(bad::UNTERMINATED_LIST, start0);
      }
      dest.reset(new List(start0, std::move(_1)));
      return end2;
    }
    dest.reset(new TokenAtom(start0, raw0, _0));
    return end0;
  }
}
