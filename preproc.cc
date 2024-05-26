#include<algorithm>
#include<filesystem>
#include<fstream>
#include<iterator>
#include<sstream>
#include"mylisp.hh"
#include"myutils/xyz.hh"
#include"nodes.hh"
#include"token.hh"

using namespace std;

namespace zlt::mylispc {
  using Context = PreprocContext;
  using It = UNodes::const_iterator;

  static void preprocList(UNodes &dest, Context &ctx, const Pos *pos, It it, It end);

  void preproc(UNodes &dest, Context &ctx, const UNode &src) {
    if (auto a = dynamic_cast<const List *>(src.get()); a) {
      preprocList(dest, ctx, a->pos, a->items.begin(), a->items.end());
      return;
    }
    dest.push_back({});
    clone(dest.back(), src);
  }

  static const Macro *isMacro(const Context &ctx, const UNode &src) noexcept;

  using Pound = void (UNodes &dest, Context &ctx, const Pos *pos, It it, It end);

  static Pound *isPound(const UNode &src) noexcept;

  void preprocList(UNodes &dest, Context &ctx, const Pos *pos, It it, It end) {
    if (it == end) [[unlikely]] {
      goto A;
    }
    if (auto m = isMacro(ctx, *it); m) {
      UNodes a;
      ExpandContext ec;
      expand(a, ec, *m, ++it, end);
      preproc(dest, ctx, a.begin(), a.end());
      return;
    }
    if (auto p = isPound(*it); p) {
      p(dest, ctx, src.pos, ++it, end);
      return;
    }
    A:
    dest.push_back({});
    clone(dest.back(), src);
  }

  const Macro *isMacro(const Context &ctx, const UNode &src) noexcept {
    auto id = dynamic_cast<const IDAtom *>(src.get());
    if (!id) {
      return nullptr;
    }
    auto it = ctx.macros.find(id->name);
    if (it == ctx.macros.end()) {
      return nullptr;
    }
    return &it->second;
  }

  static Pound pound;
  static Pound pound2;
  static Pound poundDef;
  static Pound poundIf;
  static Pound poundInclude;
  static Pound poundMovedef;
  static Pound poundUndef;

  Pound *isPound(const UNode &src) noexcept {
    auto t = dynamic_cast<const TokenAtom *>(src.get());
    if (!t) {
      return nullptr;
    }
    if (t->token == "#"_token) {
      return pound;
    }
    if (t->token == "##"_token) {
      return pound2;
    }
    if (t->token == "#def"_token) {
      return poundDef;
    }
    if (t->token == "#if"_token) {
      return poundIf;
    }
    if (t->token == "#include"_token) {
      return poundInclude;
    }
    if (t->token == "#movedef"_token) {
      return poundMovedef;
    }
    if (t->token == "#undef"_token) {
      return poundUndef;
    }
    return nullptr;
  }

  static bool tostr(string_view &dest, It it, It end) noexcept;

  void pound(UNodes &dest, Context &ctx, const Pos *pos, It it, It end) {
    string_view s;
    if (!tostr(s, it, end)) [[unlikely]] {
      bad::report(ctx.err, bad::INV_PREPROC_ARG, (**it).pos);
    }
    auto value = addSymbol(ctx.symbols, s);
    dest.push_back({});
    dest.back().reset(new StringAtom(pos, value));
  }

  bool tostr(string_view &dest, It it, It end) noexcept {
    if (it == end) [[unlikely]] {
      dest = "";
      return true;
    }
    if (auto a = dynamic_cast<const RawAtom *>(it->get()); a) {
      dest = a->raw();
      return true;
    }
    return false;
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
    preprocThen(dest, ctx, pos, ++it, end);
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
      preprocThen(dest, ctx, pos, ++it, end);
      return;
    }
    A:
    poundElse(dest, ctx, ++it, end);
  }

  void poundThen(UNodes &dest, Context &ctx, const Pos *pos, It it, It end) {
    It it1 = find(it, end, isTokenAtom<"#if"_token>);
    preproc(dest, ctx, it, it1);
  }

  static bool includeFile(string_view &dest, Context &ctx, const UNode &src);

  void poundInclude(ostream &dest, Context &ctx, const Pos *pos, It it, It end) {
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

  void poundMovedef(ostream &dest, Context &ctx, const Pos *pos, It it, It end) {
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

  void poundUndef(ostream &dest, Context &ctx, const Pos *pos, It it, It end) {
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
