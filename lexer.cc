#include<cctype>
#include"parse.hh"
#include"token.hh"
#include"zlt/io.hh"

namespace zlt::mylispc {
  using Context = ParseContext;

  static void lineComment(FILE *src) noexcept;

  void hit(FILE *src) noexcept {
    int c = fgetc(src);
    if (c == EOF) [[unlikely]] {
      return;
    }
    if (c == '\n') {
      goto A;
    }
    if (isspace(c)) {
      hit(src);
      return;
    }
    if (c == ';') {
      lineComment(src);
      return;
    }
    A:
    ungetc(c, src);
  }

  void lineComment(FILE *src) noexcept {
    int c = fgetc(src);
    if (c == EOF) [[unlikely]] {
      return;
    }
    if (c == '\n') {
      ungetc(c, src);
      return;
    }
    lineComment(src);
  }

  /// @throw bad::Fatal
  static void lexerStr(String &strval, Context &ctx, int quot, FILE *src);

  static void consumeRaw(size_t &size, FILE *src) noexcept;

  int lexer(double &numval, String &strval, const String *&raw, Context &ctx, FILE *src) {
    if (feof(src)) [[unlikely]] {
      return token::E0F;
    }
    int c = fgetc(src);
    if (c == '\n') {
      return token::EOL;
    }
    if (c == '(') {
      return "("_token;
    }
    if (c == ')') {
      return ")"_token;
    }
    if (c == '"' || c == '\'') {
      lexerStr(strval, ctx, c, src);
      return token::STRING;
    }
    size_t n = 0;
    consumeRaw(n, src);
    if (!n) {
      throw bad::makeFat(bad::UNRECOGNIZED_SYMB_FAT, ctx.pos);
    }
    auto data = typeAlloc<char>(n);
    if (!data) {
      throw bad::makeFat(bad::OOM_FAT);
    }
    FreeGuard g(data);
    auto raw1 = string::make(data, n);
    int t = token::fromRaw(numval, ctx.err, ctx.pos, raw1);
    if (t == token::ID || t == token::NUMBER) {
      raw = addSymbol(ctx.symbols, std::move(raw1));
    }
    return t;
  }

  void consumeRaw(size_t &size, FILE *src) noexcept {
    int c = fgetc(src);
    if (c == EOF || strchr("\"'();", c) || isspace(c)) {
      return;
    }
    ++size;
    consumeRaw(size, src);
  }

  /// @throw bad::Fatal
  static void lexerStr(FILE *dest, size_t &size, Context &ctx, int quot, FILE *src);

  void lexerStr(String &strval, Context &ctx, int quot, FILE *src) {
    auto sb = tmpfile();
    io::CloseGuard g(sb);
    size_t size = 0;
    lexerStr(sb, size, ctx, quot, src);
    rewind(sb);
    auto data = typeAlloc<char>(size);
    if (!data) {
      throw bad::makeFat(bad::OOM_FAT);
    }
    FreeGuard g1(data);
    io::read(data, size, sb);
    strval.data = data;
    strval.size = size;
    data = nullptr;
  }

  static void esch(int &dest, FILE *src) noexcept;

  void lexerStr(FILE *dest, size_t &size, Context &ctx, int quot, FILE *src) {
    if (feof(src)) [[unlikely]] {
      throw bad::makeFat(bad::UNTERMINATED_STR_FAT, ctx.pos);
    }
    int c = fgetc(src);
    if (c == quot) {
      return;
    }
    if (c == '\\') {
      esch(c, src);
    }
    fputc(c, dest);
    ++size;
    lexerStr(dest, size, ctx, quot, src);
  }

  static void esch8(int &dest, FILE *src, size_t limit) noexcept;
  static void esch16(int &dest, FILE *src) noexcept;

  void esch(int &dest, FILE *src) noexcept {
    if (feof(src)) [[unlikely]] {
      dest = '\\';
      return;
    }
    int c = fgetc(src);
    if (c == '"' || c == '\'' || c == '\\') {
      dest = c;
      return;
    }
    if (c == 'n') {
      dest = '\n';
      return;
    }
    if (c == 'r') {
      dest = '\r';
      return;
    }
    if (c == 't') {
      dest = '\t';
      return;
    }
    if (c >= '0' && c <= '3') {
      dest = c - '0';
      esch8(dest, src, 2);
      return;
    }
    if (c >= '4' && c <= '7') {
      dest = c - '0';
      esch8(dest, src, 1);
      return;
    }
    if (*it == 'x') {
      esch16(dest, src);
      return;
    }
    dest = '\\';
  }

  void esch8(int &dest, FILE *src, size_t limit) noexcept {
    if (feof(src) || !limit) [[unlikely]] {
      return;
    }
    int c = isDigitChar(fgetc(src), 8);
    if (c < 0) {
      ungetc(c, src);
      return;
    }
    dest = (dest << 3) | c;
    esch8(dest, src, limit - 1);
  }

  void esch16(int &dest, FILE *src) noexcept {
    if (feof(src)) {
      goto A;
    }
    int c = isDigitChar(fgetc(src), 16);
    if (c < 0 || feof(src)) {
      goto B;
    }
    int c1 = isDigitChar(fgetc(src), 16);
    if (c1 < 0) {
      goto C;
    }
    dest = (c << 4) | c1;
    return;
    C:
    ungetc(c1, src);
    B:
    ungetc(c, src);
    A:
    ungetc('x', src);
    dest = '\\';
  }

  int lexer(Context &ctx, FILE *src) {
    double d;
    auto s = string::make();
    FreeGuard fg(s.data);
    const String *raw;
    return lexer(d, s, raw, ctx, src);
  }
}
