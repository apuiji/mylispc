#include<cctype>
#include"parse.hh"
#include"token.hh"
#include"zlt/guard.hh"

namespace zlt::mylispc {
  using Context = ParseContext;
  using It = const char *;

  static It lineComment(Pos &pos, It it, It end) noexcept;

  It hit(Pos &pos, It it, It end) noexcept {
    if (it == end) [[unlikely]] {
      return end;
    }
    if (*it == '\n') {
      ++pos.li;
      return hit(pos, it + 1, end);
    }
    if (isspace(*it)) {
      return hit(pos, it + 1, end);
    }
    if (*it == ';') {
      return lineComment(pos, it + 1, end);
    }
    return it;
  }

  It lineComment(Pos &pos, It it, It end) noexcept {
    if (it == end) [[unlikely]] {
      return end;
    }
    if (*it == '\n') {
      return hit(pos, it, end);
    }
    return lineComment(pos, it + 1, end);
  }

  static const char *lexerStr(string &strval, Context &ctx, int quot, It it, It end);
  static It consumeRaw(It it, It end) noexcept;

  It lexer(int &tokval, double &numval, string &strval, Context &ctx, It it, It end) {
    if (it == end) [[unlikely]] {
      tokval = token::E0F;
      return end;
    }
    if (*it == '(') {
      tokval = "("_token;
      return it + 1;
    }
    if (*it == ')') {
      tokval = ")"_token;
      return it + 1;
    }
    if (*it == '"' || *it == '\'') {
      tokval = token::STRING;
      rewind(ctx.strout);
      ctx.strsize = 0;
      return lexerStr(strval, ctx, *it, it + 1, end);
    }
    It it1 = consumeRaw(it, end);
    if (it1 == it) {
      throw bad::makeFat(bad::UNRECOGNIZED_SYMB_FAT, ctx.pos);
    }
    auto raw = string::make(it, it1);
    tokval = token::fromRaw(numval, ctx.err, ctx.pos, raw);
    return it1;
  }

  It consumeRaw(It it, It end) noexcept {
    while (it != end && !strchr("\"'();", *it) && !isspace(c)) {
      ++it;
    }
    return it;
  }

  static size_t esch(int &dest, It it, It end) noexcept;
  static It consumeStr(int quot, It it, It end) noexcept;

  It lexerStr(String &strval, Context &ctx, int quot, It it, It end) {
    if (it == end) [[unlikely]] {
      throw bad::makeFat(bad::UNTERMINATED_STR_FAT, ctx.pos);
    }
    if (*it == quot) {
      char *data = (char *) malloc(ctx.strsize);
      if (!data) {
        throw bad::makeFat(bad::OOM_FAT);
      }
      rewind(ctx.strout);
      fread(data, 1, ctx.strsize, ctx.strout);
      strval = string::make(data, ctx.strsize);
      return it + 1;
    }
    if (*it == '\\') {
      int c;
      size_t n = esch(c, it + 1, end);
      fputc(c, ctx.strout);
      return lexerStr(strval, ctx, quot, it + 1 + n, end);
    }
    It it1 = consumeStr(quot, it + 1, end);
    fwrite(it, 1, it1 - it, ctx.strout);
    return lexerStr(strval, ctx, quot, it1, end);
  }

  static size_t esch8(int &dest, It it, It end, size_t limit) noexcept;
  static size_t esch16(int &dest, It it, It end) noexcept;

  size_t esch(int &dest, It it, It end) noexcept {
    if (it == end) {
      dest = '\\';
      return 0;
    }
    if (*it == '"' || *it == '\'' || *it == '\\') {
      dest = *it;
      return 1;
    }
    if (*it == 'n') {
      dest = '\n';
      return 1;
    }
    if (*it == 'r') {
      dest = '\r';
      return 1;
    }
    if (*it == 't') {
      dest = '\t';
      return 1;
    }
    if (*it >= '0' && *it <= '3') {
      return esch8(dest, it, end, 3);
    }
    if (*it >= '4' && *it <= '7') {
      return esch8(dest, it, end, 2);
    }
    if (*it == 'x') {
      return esch16(dest, it, end);
    }
    dest = '\\';
    return 0;
  }

  size_t esch8(int &dest, It it, It end, size_t limit) noexcept {
    dest = 0;
    int n = 0;
    for (; it != end && n < limit && *it >= '0' && *it <= '7'; ++it, ++n) {
      dest = (dest << 3) | (*it - '0');
    }
    return n;
  }

  size_t esch16(int &dest, It it, It end) noexcept {
    if (end - it < 3) [[unlikely]] {
      goto A;
    }
    int a = isDigitChar(it[1]);
    int b = isDigitChar(it[2]);
    if (a < 0 || b < 0) {
      goto A;
    }
    dest = (a << 4) | b;
    return 3;
    A:
    dest = '\\';
    return 0;
  }

  It consumeStr(int quot, It it, It end) noexcept {
    while (it != end && *it != '\\' && *it != quot) {
      ++it;
    }
    return it;
  }

  It lexer(int &tokval, Context &ctx, It it, It end) {
    double d;
    auto s = string::make();
    FreeGuard fg(s.data);
    return lexer(tokval, d, s, ctx, it, end);
  }
}
