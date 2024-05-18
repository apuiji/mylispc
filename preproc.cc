#include<algorithm>
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
    if (auto a = dynamic_cast<const NumberAtom *>(src); a) {
      dest << *a->raw;
      return;
    }
    if (auto a = dynamic_cast<const StringAtom *>(src); a) {
      outString1(dest, *a->value);
      return;
    }
    if (auto a = dynamic_cast<const IDAtom *>(src); a) {
      dest << *a->name;
      return;
    }
    if (auto a = dynamic_cast<const TokenAtom *>(src); a) {
      dest << token::raw(a->token);
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

  static void macroExpand(ostream &dest, Context &ctx, const Macro &macro, It it, It end);

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
      macroExpand(dest, ctx, itMacro->second, src.begin(), src.end());
      return;
    }
    if (auto p = isPound(src.front()); p) {
      src.pop_front();
      p(dest, ctx, src);
      return;
    }
    A:
    preprocList1(dest, ctx, src);
  }

  static void outPos(ostream &dest, const Pos &pos) {
    outString1(dest, *pos.file);
    dest << ' ' << pos.li;
  }

  static void outPos1(ostream &dest, const Pos &pos) {
    dest << "($pos ";
    outPos(dest, pos);
    dest.put(')');
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
    Pos startPos = ctx.pos;
    MacroExpMap map;
    macroExpand1(map, ctx, macro.params.begin(), macro.params.end(), it, end);
    Pos endPos = ctx.pos;
    ctx.pos = macro.pos;
    stringstream ss;
    macroExpand2(ss, ctx, map, end, macro.body.begin(), macro.body.end());
    remove(map);
    auto s = ss.str();
    remove(ss);
    dest << "($pushpos)";
    outPos1(dest, macro.pos);
    pushPos(ctx.posk, startPos);
    ctx.pos = macro.pos;
    UNodes a;
    ParseContext pc(ctx.err, ctx.symbols, ctx.posk);
    parse(a, pc, s.data(), s.data() + s.size());
    remove(s);
    ctx.pos = macro.pos;
    preproc(dest, ctx, a);
    dest << "($poppos)";
    outPos1(dest, endPos);
    popPos(ctx.posk);
    ctx.pos = endPos;
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
    if (auto a = dynamic_cast<const NumberAtom *>(src.get()); a) {
      dest << *a->raw;
      return;
    }
    if (auto a = dynamic_cast<const StringAtom *>(src.get()); a) {
      outString1(dest, *a->value);
      return;
    }
    if (auto a = dynamic_cast<const IDAtom *>(src.get()); a) {
      auto itMap = map.find(a->name);
      if (itMap == map.end()) {
        dest << *a->name;
        return;
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
    if (auto a = dynamic_cast<const TokenAtom *>(src.get()); a) {
      dest << token::raw(a->token);
      return;
    }
    auto &items = static_cast<const ListAtom &>(*src).items;
    dest.put('(');
    macroExpand2(dest, map, endArg, item.begin(), items.end());
    dest.put(')');
  }

  void macroExpand3(ostream &dest, const UNode &src) {
    if (Dynamicastable<EOLAtom> {}(*src)) {
      dest << endl;
      return;
    }
    dest.put(' ');
    if (auto a = dynamic_cast<const NumberAtom *>(src.get()); a) {
      dest << *a->raw;
      return;
    }
    if (auto a = dynamic_cast<const StringAtom *>(src.get()); a) {
      outString1(dest, *a->value);
      return;
    }
    if (auto a = dynamic_cast<const IDAtom *>(src.get()); a) {
      dest << *a->name;
      return;
    }
    if (auto a = dynamic_cast<const TokenAtom *>(src.get()); a) {
      dest << token::raw(a->token);
      return;
    }
    auto &items = static_cast<const ListAtom &>(*src).items;
    dest.put('(');
    macroExpand3(dest, items.begin(), items.end());
    dest.put(')');
  }

  static void outPound(Context &ctx, const UNode &src) noexcept;

  static inline void outPound(Context &ctx, It it, It end) noexcept {
    for (; it != end; ++it) {
      outPound(*it);
    }
  }

  static inline void outPound(Context &ctx, UNodes &src) noexcept {
    outPound(ctx, src.begin(), src.end());
  }

  void pound(ostream &dest, Context &ctx, UNodes &src) {
    if (src.empty()) [[unlikely]] {
      dest << "''";
      return;
    }
    auto &a = src.front();
    if (Dynamicastable<EOLAtom> {}(*a)) {
      ++ctx.pos.li;
      src.pop_front();
      pound(dest, ctx, src);
      return;
    }
    dest.put('\'');
    if (auto b = dynamic_cast<NumberAtom *>(a.get()); b) {
      dest << *b->raw;
    } else if (auto b = dynamic_cast<IDAtom *>(a.get()); b) {
      dest << *b->name;
    } else if (auto b = dynamic_cast<TokenAtom *>(a.get()); b) {
      dest << token::raw(b->token);
    } else {
      reportBad(ctx.err, bad::INV_PREPROC_ARG, ctx.pos, ctx.posk);
    }
    dest.put('\'');
    outPound(ctx, src);
  }

  void outPound(Context &ctx, const UNode &src) noexcept {
    if (Dynamicastable<EOLAtom> {}(*src)) {
      ++ctx.pos.li;
      return;
    }
    if (auto a = dynamic_cast<const ListAtom *>(src.get()); a) {
      outPound(a->items);
      return;
    }
  }

  void pound2(ostream &dest, Context &ctx, UNodes &src) {
    if (src.empty()) [[unlikely]] {
      return;
    }
    auto &a = src.front();
    if (Dynamicastable<EOLAtom> {}(*a)) {
      ++ctx.pos.li;
      goto A;
    }
    if (auto b = dynamic_cast<NumberAtom *>(a.get()); b) {
      dest << *b->raw;
    } else if (auto b = dynamic_cast<IDAtom *>(a.get()); b) {
      dest << *b->name;
    } else if (auto b = dynamic_cast<TokenAtom *>(a.get()); b) {
      dest << token::raw(b->token);
    } else {
      reportBad(ctx.err, bad::INV_PREPROC_ARG, ctx.pos, ctx.posk);
    }
    A:
    src.pop_front();
    pound2(dest, ctx, src);
  }

  static void poundDef1(Macro &dest, Context &ctx, UNodes &src);

  void poundDef(ostream &dest, Context &ctx, UNodes &src) {
    if (src.empty()) [[unlikely]] {
      return;
    }
    auto &a = src.front();
    if (Dynamicastable<EOLAtom> {}(*a)) {
      ++ctx.pos.li;
      src.pop_front();
      poundDef(dest, ctx, src);
      return;
    }
    auto id = dynamic_cast<IDAtom *>(a.get());
    if (!id) {
      reportBad(ctx.err, bad::INV_PREPROC_ARG, ctx.pos, ctx.posk);
      goto A;
    }
    if (ctx.macros.find(id->name) != ctx.macros.end()) {
      reportBad(ctx.err, bad::MACRO_ALREADY_DEFINED, ctx.pos, ctx.posk);
      goto A;
    }
    src.pop_front();
    poundDef1(ctx.macros[name], ctx, src);
    return;
    A:
    outPound(ctx, src);
  }

  static void makeMacroParams(Macro::Params &dest, It it, It end);

  void poundDef1(Macro &dest, Context &ctx, UNodes &src) {
    if (src.empty()) [[unlikely]] {
      return;
    }
    auto &a = src.front();
    if (Dynamicastable<EOLAtom> {}(*a)) {
      ++ctx.pos.li;
      src.pop_front();
      poundDef1(dest, ctx, src);
      return;
    }
    auto ls = dynamic_cast<ListAtom *>(a.get());
    if (!ls) {
      reportBad(ctx, bad::INV_PREPROC_ARG, ctx.pos, ctx.posk);
      outPound(ctx, src);
      return;
    }
    makeMacroParams(dest, ls->items.begin(), ls->items.end());
    src.pop_front();
    dest.pos = ctx.pos;
    dest.body = std::move(src);
    outPound(ctx, dest.body);
  }

  void makeMacroParams(Macro::Params &dest, It it, It end) {
    if (it == end) [[unlikely]] {
      return;
    }
    if (auto a = dynamic_cast<const IDAtom *>(it->get()); a) {
      dest.push_back(a->name);
      makeMacroParams(dest, ++it, end);
      return;
    }
    if (auto a = dynamic_cast<const ListAtom *>(it->get()); a) {}
  }

  static Pound poundIf;
  static Pound poundInclude;
  static Pound poundMovedef;
  static Pound poundUndef;
}
