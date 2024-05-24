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

  static void outString(ostream &dest, const unsigned char *it, const unsigned char *end);

  static inline void outString(ostream &dest, string_view s) {
    outString(dest, (unsigned char *) s.data(), (unsigned char *) s.data() + s.size());
  }

  static inline void outString1(ostream &dest, string_view s) {
    dest.put('\'');
    outString(dest, s);
    dest.put('\'');
  }

  static void preprocList(ostream &dest, Context &ctx, UNodes &src);

  void preproc(ostream &dest, Context &ctx, UNode &src) {
    if (Dynamicastable<EOLAtom> {}(*src)) {
      dest << endl;
      ++ctx.pos.li;
      return;
    }
    dest.put(' ');
    if (auto a = dynamic_cast<const RawAtom *>(src); a) {
      dest << a->raw();
      return;
    }
    if (auto a = dynamic_cast<const StringAtom *>(src); a) {
      outString1(dest, *a->value);
      return;
    }
    preprocList(dest, ctx, static_cast<ListAtom &>(*src).items);
  }

  static inline bool spechar(unsigned int c) noexcept {
    return c < 32 || c == '\'' || c == '\\' || c >= 128;
  }

  void outString(ostream &dest, const unsigned char *it, const unsigned char *end) {
    if (auto it1 = find_if(it, end, spechar); it1 != it) {
      dest.write(it, it1 - it);
      outString(dest, it1, end);
      return;
    }
    dest.put('\\');
    if (*it == '\n') {
      dest.put('n');
    } else if (*it == '\r') {
      dest.put('r');
    } else if (*it == '\t') {
      dest.put('t');
    } else if (*it == '\'' || *it == '\\') {
      dest.put(*it);
    } else {
      dest << 'x' << hex << setw(2) << setfill('0') << *it;
    }
    outString(dest, it + 1, end);
  }

  static inline void preprocList1(ostream &dest, Context &ctx, UNodes &src) {
    dest.put('(');
    preproc(dest, ctx, src);
    dest.put(')');
  }

  using It = UNodes::const_iterator;

  static void getEndPos(Pos &dest, const UNode &src) noexcept;

  static inline void getEndPos(Pos &dest, It it, It end) noexcept {
    for (; it != end; ++it) {
      getEndPos(dest, *it);
    }
  }

  static void macroExpand(ostream &dest, Context &ctx, const Macro &macro, It it, It end);

  static void outPos(ostream &dest, const Pos &pos) {
    outString1(dest, *pos.file);
    dest << ' ' << pos.li;
  }

  static void outPos1(ostream &dest, const Pos &pos) {
    dest << "($pos ";
    outPos(dest, pos);
    dest.put(')');
  }

  using Pound = void (ostream &dest, Context &ctx, UNodes &src);

  static Pound *isPound(const UNode &src) noexcept;

  void preprocList(ostream &dest, Context &ctx, UNodes &src) {
    if (src.empty()) [[unlikely]] {
      dest << "()";
      return;
    }
    if (Dynamicastable<EOLAtom> {}(*src.front())) {
      dest << endl;
      ++ctx.pos.li;
      src.pop_front();
      preprocList(dest, ctx, src);
      return;
    }
    if (auto a = dynamic_cast<IDAtom *>(src.front().get()); a) {
      auto itMacro = ctx.macros.find(a->name);
      if (itMacro == ctx.macros.end()) {
        goto A;
      }
      src.pop_front();
      auto &m = itMacro->second;
      Pos endPos = ctx.pos;
      getEndPos(endPos, src.begin(), src.end());
      dest << "($pushpos)";
      outPos1(dest, m.pos);
      pushPos(ctx.posk, m.pos);
      macroExpand(dest, ctx, m, src.begin(), src.end());
      popPos(ctx.posk);
      dest << "($poppos)";
      outPos1(dest, endPos);
      ctx.pos = endPos;
      return;
    }
    if (auto p = isPound(src.front()); p) {
      src.pop_front();
      Pos endPos = ctx.pos;
      getEndPos(endPos, src.begin(), src.end());
      p(dest, ctx, src);
      outPos1(dest, endPos);
      ctx.pos = endPos;
      return;
    }
    A:
    preprocList1(dest, ctx, src);
  }

  void getEndPos(Pos &dest, const UNode &src) noexcept {
    if (Dynamicastable<EOLAtom> {}(*src)) {
      ++dest.li;
    } else if (auto a = dynamic_cast<const ListAtom *>(src.get()); a) {
      getEndPos(dest, a->items.begin(), a->items.end());
    }
  }

  using MacroExpMap = map<const string *, pair<Pos, It>>;

  static void macroExpand1(MacroExpMap &dest, Context &ctx, Macro::ItParam itParam, Macro::ItParam endParam, It it, It end);
  static void macroExpand2(ostream &dest, Context &ctx, MacroExpMap &map, It endArg, const UNode &src);

  static inline void macroExpand2(ostream &dest, Context &ctx, MacroExpMap &map, It endArg, It it, It end) {
    for (; it != end; ++it) {
      macroExpand2(dest, ctx, map, endArg, *it);
    }
  }

  void macroExpand(ostream &dest, Context &ctx, const Macro &macro, It it, It end) {
    MacroExpMap map;
    macroExpand1(map, ctx, macro.params.begin(), macro.params.end(), it, end);
    ctx.pos = macro.pos;
    stringstream ss;
    macroExpand2(ss, ctx, map, end, macro.body.begin(), macro.body.end());
    remove(map);
    auto s = ss.str();
    remove(ss);
    UNodes a;
    ParseContext pc(ctx.err, ctx.symbols, macro.pos, ctx.posk);
    parse(a, pc, s.data(), s.data() + s.size());
    remove(s);
    ctx.pos = macro.pos;
    preproc(dest, ctx, a);
  }

  static void macroExpand1a(MacroExpMap &dest, Context &ctx, Macro::ItParam itParam, Macro::ItParam endParam, It end);

  void macroExpand1(MacroExpMap &dest, Context &ctx, Macro::ItParam itParam, Macro::ItParam endParam, It it, It end) {
    if (itParam == endParam) [[unlikely]] {
      return;
    }
    if (it == end) [[unlikely]] {
      macroExpand1a(dest, ctx, itParam, endParam, end);
      return;
    }
    if (Dynamicastable<EOLAtom> {}(*it)) {
      ++ctx.pos.li;
      macroExpand1(dest, ctx, itParam, endParam, ++it, end);
      return;
    }
    if (*itParam) {
      dest[*itParam] = make_pair(ctx.pos, it);
    }
    macroExpand1(dest, ctx, ++itParam, endParam, ++it, end);
  }

  void macroExpand1a(MacroExpMap &dest, Context &ctx, Macro::ItParam itParam, Macro::ItParam endParam, It end) {
    if (itParam == endParam) [[unlikely]] {
      return;
    }
    if (*itParam) {
      dest[*itParam] = make_pair(ctx.pos, end);
    }
    macroExpand1a(dest, ctx, ++itParam, endParam, end);
  }

  static void macroExpand3(ostream &dest, const UNode &src);

  static inline void macroExpand3(ostream &dest, It it, It end) {
    for (; it != end; ++it) {
      macroExpand3(dest, *it);
    }
  }

  void macroExpand2(ostream &dest, MacroExpMap &map, It endArg, const UNode &src) {
    if (Dynamicastable<EOLAtom> {}(**it)) {
      dest << endl;
      ++ctx.pos.li;
      return;
    }
    dest.put(' ');
    if (auto a = dynamic_cast<const StringAtom *>(src.get()); a) {
      outString1(dest, *a->value);
      return;
    }
    if (auto a = dynamic_cast<const IDAtom *>(src.get()); a) {
      auto itMap = map.find(a->name);
      if (itMap == map.end()) {
        goto A;
      }
      if (itMap->second == endArg) {
        return;
      }
      dest << "($pushpos)";
      outPos1(itMap->first);
      if (string_view(*a->name).starts_with("...")) {
        macroExpand3(dest, itMap->second, endArg);
      } else {
        macroExpand3(dest, *itMap->second);
      }
      dest << "($poppos)";
      outPos1(ctx.pos);
      return;
    }
    if (auto a = dynamic_cast<const ListAtom *>(src.get()); a) {
      dest.put('(');
      macroExpand2(dest, map, endArg, a->item.begin(), a->items.end());
      dest.put(')');
    }
    A:
    dest << static_cast<const RawAtom &>(*src).raw();
  }

  void macroExpand3(ostream &dest, const UNode &src) {
    if (Dynamicastable<EOLAtom> {}(*src)) {
      dest << endl;
      return;
    }
    dest.put(' ');
    if (auto a = dynamic_cast<const RawAtom *>(src.get()); a) {
      dest << a->raw();
      return;
    }
    if (auto a = dynamic_cast<const StringAtom *>(src.get()); a) {
      outString1(dest, *a->value);
      return;
    }
    auto &items = static_cast<const ListAtom &>(*src).items;
    dest.put('(');
    macroExpand3(dest, items.begin(), items.end());
    dest.put(')');
  }

  static void hitPoundArg(ostream &dest, Context &ctx, UNodes &src);

  void pound(ostream &dest, Context &ctx, UNodes &src) {
    hitPoundArg(dest, ctx, src);
    if (src.empty()) [[unlikely]] {
      dest << "''";
      return;
    }
    dest.put('\'');
    if (auto a = dynamic_cast<RawAtom *>(src.front().get()); a) {
      dest << a->raw();
    } else {
      reportBad(ctx.err, bad::INV_PREPROC_ARG, ctx.pos, ctx.posk);
    }
    dest.put('\'');
  }

  void hitPoundArg(ostream &dest, Context &ctx, UNodes &src) {
    for (; src.size() && Dynamicastable<EOLAtom> {}(*src.front()); src.pop_front()) {
      dest << endl;
    }
  }

  void pound2(ostream &dest, Context &ctx, UNodes &src) {
    hitPoundArg(dest, ctx, src);
    if (src.empty()) [[unlikely]] {
      return;
    }
    if (auto a = dynamic_cast<RawAtom *>(src.front().get()); a) {
      dest << a->raw();
    } else {
      reportBad(ctx.err, bad::INV_PREPROC_ARG, ctx.pos, ctx.posk);
      getEndPos(ctx.pos, a);
    }
    src.pop_front();
    pound2(dest, ctx, src);
  }

  static void poundDef1(Macro &dest, Context &ctx, UNodes &src);

  void poundDef(ostream &dest, Context &ctx, UNodes &src) {
    hitPoundArg(dest, ctx, src);
    if (src.empty()) [[unlikely]] {
      return;
    }
    auto &a = src.front();
    auto id = dynamic_cast<IDAtom *>(a.get());
    if (!id) {
      reportBad(ctx.err, bad::INV_PREPROC_ARG, ctx.pos, ctx.posk);
      return;
    }
    if (ctx.macros.find(id->name) != ctx.macros.end()) {
      reportBad(ctx.err, bad::MACRO_ALREADY_DEFINED, ctx.pos, ctx.posk);
      return;
    }
    src.pop_front();
    poundDef1(ctx.macros[id->name], ctx, src);
  }

  static void makeMacroParams(Macro::Params &dest, Context &ctx, UNodes &src);

  void poundDef1(Macro &dest, Context &ctx, UNodes &src) {
    hitPoundArg(dest, ctx, src);
    if (src.empty()) [[unlikely]] {
      return;
    }
    auto ls = dynamic_cast<ListAtom *>(src.front().get());
    if (!ls) {
      reportBad(ctx, bad::INV_PREPROC_ARG, ctx.pos, ctx.posk);
      return;
    }
    makeMacroParams(dest, ctx, ls->items);
    src.pop_front();
    hitPoundArg(dest, ctx, src);
    dest.params.shrink_to_fit();
    dest.pos = ctx.pos;
    dest.body = std::move(src);
  }

  void makeMacroParams(Macro::Params &dest, Context &ctx, UNodes &src) {
    hitPoundArg(dest, ctx, src);
    if (src.empty()) [[unlikely]] {
      return;
    }
    if (auto a = dynamic_cast<const IDAtom *>(src.front().get()); a) {
      dest.push_back(a->name);
      if (string_view(*a->name).starts_with("...")) {
        return;
      }
    } else if (auto a = dynamic_cast<const ListAtom *>(src.front().get()); a) {
      dest.push_back(nullptr);
      Pos pos = ctx.pos;
      hitPoundArg(dest, ctx, a->items);
      if (a->items.size()) {
        reportBad(ctx.err, bad::ILL_MACRO_PARAM, pos, ctx.posk);
      }
    } else {
      reportBad(ctx.err, bad::ILL_MACRO_PARAM, ctx.pos, ctx.posk);
    }
    src.pop_front();
    makeMacroParams(dest, cts, src);
  }

  static void poundElse(ostream &dest, Context &ctx, UNodes &src);
  static void poundIfdef(ostream &dest, Context &ctx, UNodes &src);
  static void poundThen(ostream &dest, Context &ctx, UNodes &src);

  void poundIf(ostream &dest, Context &ctx, UNodes &src) {
    hitPoundArg(dest, ctx, src);
    if (src.empty()) [[unlikely]] {
      return;
    }
    if (auto a = dynamic_cast<ListAtom *>(src.front().get()); a) {
      hitPoundArg(dest, ctx, a->items);
      getEndPos(ctx.pos, a->items);
      if (a->items.size()) {
        goto A;
      }
      src.pop_front();
      poundElse(dest, ctx, src);
      return;
    }
    if (auto a = dynamic_cast<TokenAtom *>(src.front().get()); a && a->token == "#def"_token) {
      src.pop_front();
      poundIfdef(dest, ctx, src);
      return;
    }
    A:
    src.pop_front();
    preprocThen(dest, ctx, src);
  }

  void poundElse(ostream &dest, Context &ctx, UNodes &src) {
    hitPoundArg(dest, ctx, src);
    if (src.empty()) [[unlikely]] {
      return;
    }
    if (auto a = dynamic_cast<ListAtom *>(src.front().get()); a) {
      getEndPos(ctx.pos, a->items);
      goto A;
    }
    if (auto a = dynamic_cast<TokenAtom *>(src.front().get()); a && a->token == "#if"_token) {
      src.pop_front();
      poundIf(dest, ctx, src);
      return;
    }
    A:
    src.pop_front();
    poundElse(dest, ctx, src);
  }

  void poundIfdef(ostream &dest, Context &ctx, UNodes &src) {
    hitPoundArg(dest, ctx, src);
    if (src.empty()) [[unlikely]] {
      return;
    }
    auto id = dynamic_cast<IDAtom *>(src.front().get());
    if (!id) {
      reportBad(ctx.err, bad::INV_PREPROC_ARG, ctx.pos, ctx.posk);
      if (auto a = dynamic_cast<ListAtom *>(src.front().get()); a) {
        getEndPos(ctx.pos, a->items);
      }
      goto A;
    }
    if (ctx.macros.find(id->name) != ctx.macros.end()) {
      src.pop_front();
      preprocThen(dest, ctx, src);
      return;
    }
    A:
    src.pop_front();
    poundElse(dest, ctx, src);
  }

  void poundThen(ostream &dest, Context &ctx, UNodes &src) {
    for (; src.size(); src.pop_front()) {
      if (auto a = dynamic_cast<TokenAtom *>(src.front().get()); a && a->token == "#if"_token) {
        break;
      }
      preproc(dest, ctx, src.front());
    }
  }

  static bool includeFile(string_view &dest, Context &ctx, const UNode &src);

  void poundInclude(ostream &dest, Context &ctx, UNodes &src) {
    hitPoundArg(dest, ctx, src);
    if (src.empty()) [[unlikely]] {
      return;
    }
    string_view file;
    if (!includeFile(file, ctx, src.front())) {
      reportBad(ctx.err, bad::INV_PREPROC_ARG, ctx.pos, ctx.posk);
      return;
    }
    filesystem::path path(file);
    if (path.is_relative()) {
      path = filesystem::path(*ctx.pos.file).parent_path() / path;
    }
    string s;
    try {
      path = filesystem::canonical(path);
      stringstream ss;
      ifstream ifs(path);
      copy(istreambuf_iterator(ifs), istreambuf_iterator<char>(), ostreambuf_iterator(ss));
      s = ss.str();
    } catch (...) {
      reportBad(ctx.err, bad::CANNOT_INCLUDE, ctx.pos, ctx.posk);
      return;
    }
    pushPos(ctx.posk, ctx.pos);
    ctx.pos = Pos(addSymbol(ctx.symbols, std::move(file)), 0);
    dest << "($pushpos)";
    outPos1(dest, ctx.pos);
    ParseContext pc(ctx.err, ctx.symbols, ctx.pos, ctx.posk);
    UNodes a;
    parse(a, pc, s.data(), s.data() + s.size());
    preproc(dest, ctx, a);
    dest << "($poppos)";
    popPos(ctx.posk);
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

  void poundMovedef(ostream &dest, Context &ctx, UNodes &src) {
    hitPoundArg(dest, ctx, src);
    if (src.empty()) [[unlikely]] {
      return;
    }
    const string *from;
    if (auto a = dynamic_cast<IDAtom *>(src.front().get()); a) {
      from = a->name;
    } else {
      reportBad(ctx.err, bad::INV_PREPROC_ARG, ctx.pos, ctx.posk);
      return;
    }
    auto itMacro = ctx.macros.find(from);
    if (itMacro == ctx.macros.end()) {
      reportBad(ctx.err, bad::MACRO_UNDEFINED, ctx.pos, ctx.posk);
      return;
    }
    src.pop_front();
    hitPoundArg(dest, ctx, src);
    if (src.empty()) [[unlikely]] {
      return;
    }
    const string *to;
    if (auto a = dynamic_cast<IDAtom *>(src.front().get()); a) {
      to = a->name;
    } else {
      reportBad(ctx.err, bad::INV_PREPROC_ARG, ctx.pos, ctx.posk);
      return;
    }
    if (ctx.macros.find(to) != ctx.macros.end()) {
      reportBad(ctx.err, bad::MACRO_ALREADY_DEFINED, ctx.pos, ctx.posk);
      return;
    }
    ctx.macros[to] = std::move(itMacro->second);
    ctx.macros.erase(itMacro);
  }

  void poundUndef(ostream &dest, Context &ctx, UNodes &src) {
    hitPoundArg(dest, ctx, src);
    if (src.empty()) [[unlikely]] {
      return;
    }
    auto id = dynamic_cast<IDAtom *>(src.front().get());
    if (!id) {
      reportBad(ctx.err, bad::INV_PREPROC_ARG, ctx.pos, ctx.posk);
      return;
    }
    auto itMacro = ctx.macros.find(id->name);
    if (itMacro != ctx.macros.end()) {
      ctx.macros.erase(itMacro);
    }
  }
}
