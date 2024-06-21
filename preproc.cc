#include"nodes.hh"
#include"parse.hh"
#include"preproc.hh"
#include"token.hh"
#include"zlt/xyz.hh"

using namespace std;

namespace zlt::mylispc {
  static void writeStr(FILE *dest, const char *data, size_t size) noexcept;

  static void writeStr(FILE *dest, const String &s) noexcept {
    fputc('\'', dest);
    writeStr(dest, s.data, s.size);
    fputc('\'', dest);
  }

  static inline void writeStr(FILE *dest, const String *s) noexcept {
    writeStr(dest, *s);
  }

  void write(FILE *dest, const Pos &pos) noexcept {
    writeStr(dest, pos.file);
    fprintf(dest, " %d", pos.li);
  }

  void writeStr(FILE *dest, const char *data, size_t size) noexcept {
    if (!size) [[unlikely]] {
      return;
    }
    if (*data == '\'') {
      io::write(dest, "\\'");
    } else if (*data == '\\') {
      io::write(dest, "\\\\");
    } else if (*data == '\n') {
      io::write(dest, "\\n");
    } else if (*data == '\r') {
      io::write(dest, "\\r");
    } else if (*data == '\t') {
      io::write(dest, "\\t");
    } else {
      fputc(*data, dest);
    }
    writeStr(dest, data + 1, size - 1);
  }

  void write(FILE *dest, const EOLAtom &src) noexcept {
    fputc('\n', dest);
  }

  void write(FILE *dest, const NumberAtom &src) noexcept {
    io::write(dest, *src.raw);
  }

  void write(FILE *dest, const StringAtom &src) noexcept {
    writeStr(dest, src.value);
  }

  void write(FILE *dest, const IDAtom &src) noexcept {
    io::write(dest, *src.name);
  }

  void write(FILE *dest, const TokenAtom &src) noexcept {
    io::write(dest, token::raw(src.token));
  }

  void write(FILE *dest, const List &src) noexcept {
    fputc('(', dest);
    writes(dest, src.first);
    fputc(')', dest);
  }

  void write(FILE *dest, const void *src) noexcept {
    int clazz = memberOf(src, &Node::clazz);
    if (clazz == EOL_ATOM_CLASS) {
      write(dest, pointTo<EOLAtom>(src));
    } else if (clazz == ID_ATOM_CLASS) {
      write(dest, pointTo<IDAtom>(src));
    } else if (clazz == NUM_ATOM_CLASS) {
      write(dest, pointTo<NumAtom>(src));
    } else if (clazz == LIST_CLASS) {
      write(dest, pointTo<List>(src));
    } else if (clazz == STR_ATOM_CLASS) {
      write(dest, pointTo<StringAtom>(src));
    } else {
      write(dest, pointTo<TokenAtom>(src));
    }
  }

  void writes(FILE *dest, const void *src) noexcept {
    if (!src) [[unlikely]] {
      return;
    }
    write(dest, src);
    fputc(' ', dest);
    writes(dest, memberOf(src, &Link::next));
  }

  using Context = PreprocContext;

  static void preprocList(FILE *dest, Context &ctx, const Pos *upPos, void *&src);

  void preproc(FILE *dest, Context &ctx, const Pos *upPos, void *&src) {
    if (!src) [[unlikely]] {
      return;
    }
    int clazz = memberOf(src, &Node::clazz);
    if (clazz == LIST_CLASS && memberOf(src, &List::first)) {
      preprocList(dest, ctx, upPos, src);
      return;
    }
    write(dest, src);
    deleteNode(link::pop(src));
    preproc(dest, ctx, upPos, src);
  }

  static const Macro *isMacro(const Context &ctx, const void *src) noexcept;

  using Pound = void (FILE *dest, Context &ctx, const Pos *upPos, void *&src);

  static Pound *isPound(const void *src) noexcept;

  void preprocList(FILE *dest, Context &ctx, const Pos *upPos, void *&src) {
    auto &first = memberOf(src, &Link::next);
    if (auto m = isMacro(ctx, first); m) {
      deleteNode(link::pop(first));
      auto ec = makeExpandContext(ctx.err);
      expand(dest, ec, *m, first);
      return;
    }
    if (auto p = isPound(first); p) {
      deleteNode(link::pop(first));
      p(dest, ctx, upPos, src);
      return;
    }
    preproc1(dest, ctx, upPos, src);
  }

  using MacroTree = map::Tree<const String *, Macro>;

  const Macro *isMacro(const Context &ctx, const void *src) noexcept {
    if (memberOf(src, &Node::clazz) != ID_ATOM_CLASS) {
      return nullptr;
    }
    auto a = map::find(ctx.macros, memberOf(src, &IDAtom::name));
    if (!a) {
      return nullptr;
    }
    return memberOf(a, &MacroTree::value);
  }

  static Pound poundDef;
  static Pound poundIf;
  static Pound poundInclude;
  static Pound poundMovedef;
  static Pound poundUndef;

  Pound *isPound(const void *src) noexcept {
    if (memberOf(src, &Node::clazz) != TOKEN_ATOM_CLASS) {
      return nullptr;
    }
    if (t == "#def"_token) {
      return poundDef;
    }
    if (t == "#if"_token) {
      return poundIf;
    }
    if (t == "#include"_token) {
      return poundInclude;
    }
    if (t == "#movedef"_token) {
      return poundMovedef;
    }
    if (t == "#undef"_token) {
      return poundUndef;
    }
    return nullptr;
  }

  static void macroParams(const String **&dest, size_t &size, Context &ctx, void *&src);

  void poundDef(FILE *dest, Context &ctx, const Pos *upPos, void *&src) {
    auto &first = memberOf(src, &List::first);
    if (!first) [[unlikely]] {
      goto A;
    }
    if (memberOf(first, &Node::clazz) != ID_ATOM_CLASS) [[unlikely]] {
      auto pos = memberOf(first, &Node::pos);
      bad::report(ctx.err, bad::UNEXPECTED_TOKEN_ERR, makePos(upPos, pos));
      goto A;
    }
    auto name = memberOf(first, &IDAtom::name);
    deleteNode(link::pop(first));
    if (memberOf(first, &Node::clazz) != LIST_CLASS) [[unlikely]] {
      auto pos = memberOf(first, &Node::pos);
      bad::report(ctx.err, bad::UNEXPECTED_TOKEN_ERR, makePos(upPos, pos));
      goto A;
    }
    {
      Macro m = {};
      FreeGuard g(m.params);
      CleanNodeGuard g1(m.body);
      m.pos = memberOf(src, &Node::pos);
      macroParams(m.params, m.paramc, ctx, memberOf(first, &List::first));
      deleteNode(link::pop(first));
      m.body = first;
      first = nullptr;
      addMacro(ctx.macros, std::move(m));
    }
    A:
    deleteNode(link::pop(src));
    preproc(dest, ctx, upPos, src);
  }

  static void macroParams(FILE *dest, size_t &size, Context &ctx, void *&src);

  void macroParams(const String **&dest, size_t &size, Context &ctx, void *&src) {
    auto f = tmpfile();
    io::CloseGuard g(f);
    if (!size) [[unlikely]] {
      return;
    }
    dest = typeAlloc<const String *>(size);
    if (!dest) {
      throw bad::makeFat(bad::OOM_FAT);
    }
    rewind(f);
    io::read(dest, size, f);
  }

  void macroParams(FILE *dest, size_t &size, Context &ctx, void *&src) {
    if (!src) [[unlikely]] {
      return;
    }
    int clazz = memberOf(src, &Node::clazz);
    if (clazz == EOL_ATOM_CLASS) {
      goto A;
    }
    if (clazz == ID_ATOM_CLASS) {
      auto name = memberOf(src, &IDAtom::name);
      io::write(dest, name);
      ++size;
      goto A;
    }
    if (clazz == LIST_CLASS && !memberOf(src, &List::first)) {
      void *p = nullptr;
      io::write(dest, p);
      ++size;
      goto A;
    }
    bad::report(ctx.err, bad::UNEXPECTED_TOKEN_ERR, memberOf(src, &Node::pos));
    A:
    deleteNode(link::pop(src));
    macroParams(dest, size, src);
  }

  static void poundElse(FILE *dest, Context &ctx, void *&src);
  static void poundIfdef(FILE *dest, Context &ctx, const Pos *pos, It it, It end);
  static void poundThen(FILE *dest, Context &ctx, const Pos *pos, It it, It end);

  void poundIf(FILE *dest, Context &ctx, void *&src) {
    if (!src) [[unlikely]] {
      return;
    }
    int clazz = memberOf(src, &Node::clazz);
    if (clazz == TOKEN_ATOM_CLASS && memberOf(src, &TokenAtom::token) == "#def"_token) {
      deleteNode(link::pop(src));
      poundIfdef(dest, ctx, pos, src);
      return;
    }
    if (clazz == LIST_CLASS && !memberOf(src, &List::first)) {
      deleteNode(link::pop(src));
      poundElse(dest, ctx, src);
      return;
    }
    deleteNode(link::pop(src));
    poundThen(dest, ctx, src);
  }

  void poundElse(FILE *dest, Context &ctx, void *&src) {
    if (!src) [[unlikely]] {
      return;
    }
    int clazz = memberOf(src, &Node::clazz);
    if (clazz != TOKEN_ATOM_CLASS) {
      goto A;
    }
    if (memberOf(src, &TokenAtom::token) == "#if"_token) {
      deleteNode(link::pop(src));
      poundIf(dest, ctx, src);
      return;
    }
    A:
    deleteNode(link::pop(src));
    poundElse(dest, ctx, src);
  }

  void poundIfdef(FILE *dest, Context &ctx, void *&src) {
    if (!src) [[unlikely]] {
      return;
    }
    int clazz = memberOf(src, &Node::clazz);
    if (clazz != ID_ATOM_CLASS) [[unlikely]] {
      bad::report(ctx.err, bad::UNEXPECTED_TOKEN_ERR, memberOf(src, &Node::pos));
      goto A;
    }
    auto name = memberOf(src, &IDAtom::name);
    if (map::find(ctx.macros, name)) {
      deleteNode(link::pop(src));
      poundThen(dest, ctx, src);
      return;
    }
    A:
    deleteNode(link::pop(src));
    poundElse(dest, ctx, src);
  }

  static void poundThen1(void *&src) noexcept;

  void poundThen(FILE *dest, Context &ctx, void *&src) {
    poundThen1(src);
    preproc(dest, ctx, src);
  }

  void poundThen1(void *&src) noexcept {
    if (!src) [[unlikely]] {
      return;
    }
    int clazz = memberOf(src, &Node::clazz);
    if (clazz != TOKEN_ATOM_CLASS) {
      goto A;
    }
    if (memberOf(src, &TokenAtom::token) == "#if"_token) {
      cleanNode(src);
      src = nullptr;
      return;
    }
    A:
    auto &next = memberOf(src, &Link::next);
    poundThen1(next);
  }

  void poundInclude(FILE *dest, Context &ctx, void *&src) {}
}
