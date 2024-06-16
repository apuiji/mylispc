#include"nodes.hh"
#include"parse.hh"
#include"preproc.hh"
#include"token.hh"
#include"zlt/xyz.hh"

using namespace std;

namespace zlt::mylispc {
  using Context = PreprocContext;

  static void preprocList(void *&dest, Context &ctx, const Pos *upPos, void *&src);
  static void preproc1(void *&dest, Context &ctx, const Pos *upPos, void *&src);

  void preproc(void *&dest, Context &ctx, const Pos *upPos, void *&src) {
    if (!src) [[unlikely]] {
      return;
    }
    if (memberOf(src, &Node::clazz) == LIST_CLASS && memberOf(src, &List::first)) {
      preprocList(dest, ctx, upPos, src);
      return;
    }
    preproc1(dest, ctx, upPos, src);
  }

  void preproc1(void *&dest, Context &ctx, const Pos *upPos, void *&src) {
    auto &pos = memberOf(src, &Node::pos);
    if (upPos) {
      pos = addPos(ctx.poss, makePos(upPos, pos));
    }
    auto &next = link::push(dest, link::pop(src));
    preproc(next, ctx, upPos, src);
  }

  static const Macro *isMacro(const Context &ctx, const void *src) noexcept;

  using Pound = void (void *&dest, Context &ctx, const Pos *upPos, void *&src);

  static Pound *isPound(const void *src) noexcept;

  void preprocList(void *&dest, Context &ctx, const Pos *upPos, void *&src) {
    auto &first = memberOf(src, &List::next);
    if (auto m = isMacro(ctx, first); m) {
      deleteNode(link::pop(first));
      void *a = nullptr;
      CleanNodeGuard g(a);
      auto ec = makeExpandContext(ctx.err);
      expand(a, ec, *m, first);
      auto pos = addPos(ctx.poss, makePos(upPos, m->pos));
      preproc(dest, ctx, pos, a);
      a = nullptr;
      return;
    }
    if (auto p = isPound(first); p) {
      deleteNode(link::pop(first));
      p(dest, ctx, upPos, src);
      return;
    }
    preproc1(dest, ctx, upPos, src);
  }

  const Macro *isMacro(const Context &ctx, const void *src) noexcept {
    if (memberOf(src, &Node::clazz) != ID_ATOM_CLASS) {
      return nullptr;
    }
    auto a = map::find(ctx.macros, memberOf(src, &IDAtom::name));
    if (!a) {
      return nullptr;
    }
    return memberOf(a, &map::Tree<const String *, Macro>::value);
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

  void poundDef(void *&dest, Context &ctx, const Pos *upPos, void *&src) {
    auto &first = memberOf(src, &List::first);
    if (!first) [[unlikely]] {
      goto A;
    }
    auto &pos = memberOf(src, &Node::pos);
    if (memberOf(first, &Node::clazz) != ID_ATOM_CLASS) [[unlikely]] {
      auto pos1 = memberOf(first, &Node::pos);
      bad::report(ctx.err, bad::UNEXPECTED_TOKEN_ERR, makePos(upPos, pos1));
      goto A;
    }
    auto name = memberOf(first, &IDAtom::name);
    deleteNode(link::pop(first));
    if (memberOf(first, &Node::clazz) != LIST_CLASS) [[unlikely]] {
      auto pos1 = memberOf(first, &Node::pos);
      bad::report(ctx.err, bad::UNEXPECTED_TOKEN_ERR, makePos(upPos, pos1));
      goto A;
    }
    A:
    deleteNode(link::pop(src));
    preproc(dest, ctx, upPos, src);
  }

  static String tostr(const void *src) noexcept;

  void pound(void *&dest, Context &ctx, const Pos *upPos, void *&src) {
    auto pos = memberOf(src, &Node::pos);
    pos = addPos(ctx.poss, makePos(upPos, *pos));
    auto s = tostr(memberOf(src, &List::first));
    const String *value;
    if (s.data) {
      value = addSymbol1(ctx.symbols, s);
    } else {
      bad::report(ctx.err, bad::INV_PREPROC_ARG_ERR, pos1);
      value = addSymbol1(ctx.symbols, string::make(""));
    }
    deleteNode(link::pop(src));
    dest = neo<StringAtom>();
    pointTo<StringAtom>(dest) = makeStringAtom(pos, value);
    preproc(memberOf(dest, &Link::next), ctx, upPos, src);
  }

  String tostr(const void *src) noexcept {
    if (!src) [[unlikely]] {
      return string::make();
    }
    int clazz = memberOf(src, &Node::clazz);
    if (clazz == ID_ATOM_CLASS) {
      return *memberOf(src, &IDAtom::name);
    }
    if (clazz == NUM_ATOM_CLASS) {
      return *memberOf(src, &NumAtom::raw);
    }
    if (clazz == TOKEN_ATOM_CLASS) {
      return token::raw(memberOf(src, &TokenAtom::token));
    }
    return string::make();
  }

  static void idcat(ostream &dest, Context &ctx, It it, It end);

  void pound2(UNodes &dest, Context &ctx, const Pos *pos, It it, It end) {
    stringstream ss;
    idcat(ss, ctx, it, end);
    auto s = ss.str();
    remove(ss);
    if (s.empty()) {
      return;
    }
    double d;
    int i = token::isNumber(d, s);
    if (i) {
      if (i < 0) {
        bad::report(ctx.err, bad::NUMBER_LITERAL_OOR, pos);
      }
      auto raw = addSymbol(ctx.symbols, s);
      dest.push_back({});
      dest.back().reset(new NumberAtom(pos, raw, d));
      return;
    }
    int t = token::fromRaw(s);
    if (t == token::ID) {
      auto name = addSymbol(ctx.symbols, s);
      dest.push_back({});
      dest.back().reset(new IDAtom(pos, name));
      return;
    }
    dest.push_back({});
    dest.back().reset(new TokenAtom(pos, t));
  }

  void idcat(ostream &dest, Context &ctx, It it, It end) {
    for (; it != end; ++it) {
      if (auto a = dynamic_cast<const RawAtom *>(it->get()); a) {
        dest << a->raw();
      } else {
        bad::report(ctx.err, bad::INV_PREPROC_ARG, (**it).pos);
      }
    }
  }

  static void poundDef1(Macro &dest, Context &ctx, It it, It end);

  void poundDef(UNodes &dest, Context &ctx, const Pos *pos, It it, It end) {
    if (it == end) [[unlikely]] {
      return;
    }
    auto id = dynamic_cast<const IDAtom *>(it->get());
    if (!id) {
      bad::report(ctx.err, bad::INV_PREPROC_ARG, (**it).pos);
      return;
    }
    if (ctx.macros.find(id->name) != ctx.macros.end()) {
      bad::report(ctx.err, bad::MACRO_ALREADY_DEFINED, pos);
      return;
    }
    poundDef1(ctx.macros[id->name], ctx, ++it, end);
  }

  static void makeMacroParams(Macro::Params &dest, Context &ctx, It it, It end);

  void poundDef1(Macro &dest, Context &ctx, It it, It end) {
    if (it == end) [[unlikely]] {
      return;
    }
    auto ls = dynamic_cast<const List *>(it->get());
    if (!ls) {
      bad::report(ctx.err, bad::INV_PREPROC_ARG, (**it).pos);
      return;
    }
    makeMacroParams(dest.params, ctx, ls->items.begin(), ls->items.end());
    dest.params.shrink_to_fit();
    clone(dest.body, ++it, end);
  }

  void makeMacroParams(Macro::Params &dest, Context &ctx, It it, It end) {
    if (it == end) [[unlikely]] {
      return;
    }
    if (auto a = dynamic_cast<const IDAtom *>(it->get()); a) {
      dest.push_back(a->name);
      if (string_view(*a->name).starts_with("...")) {
        return;
      }
    } else if (auto a = dynamic_cast<const List *>(it->get()); a && a->items.empty()) {
      dest.push_back(nullptr);
    } else {
      bad::report(ctx.err, bad::ILL_MACRO_PARAM, (**it).pos);
    }
    makeMacroParams(dest, ctx, ++it, end);
  }

  static void poundElse(UNodes &dest, Context &ctx, It it, It end);
  static void poundIfdef(UNodes &dest, Context &ctx, const Pos *pos, It it, It end);
  static void poundThen(UNodes &dest, Context &ctx, const Pos *pos, It it, It end);

  void poundIf(UNodes &dest, Context &ctx, const Pos *pos, It it, It end) {
    if (it == end) [[unlikely]] {
      return;
    }
    if (isTokenAtom<"#def"_token>(*it)) {
      poundIfdef(dest, ctx, pos, ++it, end);
      return;
    }
    if (auto a = dynamic_cast<const List *>(it->get()); a && a->items.empty()) {
      poundElse(dest, ctx, ++it, end);
      return;
    }
    poundThen(dest, ctx, pos, ++it, end);
  }

  void poundElse(UNodes &dest, Context &ctx, It it, It end) {
    for (; it != end; ++it) {
      if (isTokenAtom<"#if"_token>(*it)) {
        poundIf(dest, ctx, (**it).pos, ++it, end);
        return;
      }
    }
  }

  void poundIfdef(UNodes &dest, Context &ctx, const Pos *pos, It it, It end) {
    if (it == end) [[unlikely]] {
      return;
    }
    auto id = dynamic_cast<const IDAtom *>(it->get());
    if (!id) {
      bad::report(ctx.err, bad::INV_PREPROC_ARG, (**it).pos);
      goto A;
    }
    if (ctx.macros.find(id->name) != ctx.macros.end()) {
      poundThen(dest, ctx, pos, ++it, end);
      return;
    }
    A:
    poundElse(dest, ctx, ++it, end);
  }

  void poundThen(UNodes &dest, Context &ctx, const Pos *pos, It it, It end) {
    It it1 = find_if(it, end, isTokenAtom<"#if"_token>);
    preproc(dest, ctx, it, it1);
  }

  static bool includeFile(string_view &dest, Context &ctx, const UNode &src);

  void poundInclude(UNodes &dest, Context &ctx, const Pos *pos, It it, It end) {
    if (it == end) [[unlikely]] {
      return;
    }
    string_view file;
    if (!includeFile(file, ctx, *it)) {
      bad::report(ctx.err, bad::INV_PREPROC_ARG, (**it).pos);
      return;
    }
    filesystem::path path(file);
    if (path.is_relative()) {
      path = filesystem::path(*pos->file).parent_path() / path;
    }
    string s;
    try {
      path = filesystem::canonical(path);
      stringstream ss;
      ifstream ifs(path);
      copy(istreambuf_iterator(ifs), istreambuf_iterator<char>(), ostreambuf_iterator(ss));
      s = ss.str();
    } catch (...) {
      bad::report(ctx.err, bad::CANNOT_INCLUDE, pos);
      return;
    }
    auto file1 = addSymbol(ctx.symbols, (string) path);
    ParseContext pc(ctx.err, ctx.symbols, ctx.poss, Pos(file1, 0));
    UNodes a;
    parse(a, pc, s.data(), s.data() + s.size());
    remove(s);
    preproc(dest, ctx, a.begin(), a.end());
  }

  bool includeFile(string_view &dest, Context &ctx, const UNode &src) {
    if (auto a = dynamic_cast<const RawAtom *>(src.get()); a) {
      dest = a->raw();
      return true;
    }
    if (auto a = dynamic_cast<const StringAtom *>(src.get()); a) {
      dest = *a->value;
      return true;
    }
    return false;
  }

  void poundMovedef(UNodes &dest, Context &ctx, const Pos *pos, It it, It end) {
    if (it == end) [[unlikely]] {
      return;
    }
    const string *from;
    if (auto a = dynamic_cast<const IDAtom *>(it->get()); a) {
      from = a->name;
    } else {
      bad::report(ctx.err, bad::INV_PREPROC_ARG, (**it).pos);
      return;
    }
    auto itMacro = ctx.macros.find(from);
    if (itMacro == ctx.macros.end()) {
      bad::report(ctx.err, bad::MACRO_UNDEFINED, pos);
      return;
    }
    if (++it == end) [[unlikely]] {
      return;
    }
    const string *to;
    if (auto a = dynamic_cast<const IDAtom *>(it->get()); a) {
      to = a->name;
    } else {
      bad::report(ctx.err, bad::INV_PREPROC_ARG, (**it).pos);
      return;
    }
    if (ctx.macros.find(to) != ctx.macros.end()) {
      bad::report(ctx.err, bad::MACRO_ALREADY_DEFINED, pos);
      return;
    }
    ctx.macros[to] = std::move(itMacro->second);
    ctx.macros.erase(itMacro);
  }

  void poundUndef(UNodes &dest, Context &ctx, const Pos *pos, It it, It end) {
    if (it == end) [[unlikely]] {
      return;
    }
    auto id = dynamic_cast<const IDAtom *>(it->get());
    if (!id) {
      bad::report(ctx.err, bad::INV_PREPROC_ARG, (**it).pos);
      return;
    }
    auto itMacro = ctx.macros.find(id->name);
    if (itMacro != ctx.macros.end()) {
      ctx.macros.erase(itMacro);
    }
  }
}
