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

  static void preprocList(ostream &dest, Context &ctx, UNodes &src);

  void preproc(ostream &dest, Context &ctx, UNode &src) {
    dest.put(' ');
    if (Dynamicastable<EOLAtom> {}(*src)) {
      dest << endl;
      ++ctx.pos.li;
      return;
    }
    if (auto a = dynamic_cast<const NumberAtom *>(src); a) {
      dest << a->value;
      return;
    }
    if (auto a = dynamic_cast<const StringAtom *>(src); a) {
      dest.put('\'');
      outString(dest, *a->value);
      dest.put('\'');
      return;
    }
    if (auto a = dynamic_cast<const IDAtom *>(src); a) {
      dest << a->name;
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

  static void preprocList1(ostream &dest, Context &ctx, UNodes &src);

  using It = UNodes::const_iterator;

  static void macroExpand(ostream &dest, Context &ctx, const Macro &macro, It it, It end);

  using Pound = void (ostream &dest, Context &ctx, UNodes &src);

  static Pound *isPound(const UNode &src) noexcept;

  void preprocList(ostream &dest, Context &ctx, UNodes &src) {
    if (src.empty()) [[unlikely]] {
      dest << " ()";
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

  void preprocList1(ostream &dest, Context &ctx, UNodes &src) {
    dest.put('(');
    preproc(dest, ctx, src);
    dest.put(')');
  }

  static void outPos(ostream &dest, const Pos &pos);

  using MacroExpMap = map<const string *, pair<Pos, It>>;

  static void macroExpand1(MacroExpMap &dest, Context &ctx, Macro::ItParam itParam, Macro::ItParam endParam, It it, It end);
  static void macroExpand2(ostream &dest, Context &ctx, MacroExpMap &map, It it, It end);

  void macroExpand(ostream &dest, Context &ctx, const Macro &macro, It it, It end) {
    Pos startPos = ctx.pos;
    MacroExpMap map;
    macroExpand1(map, ctx, macro.params.begin(), macro.params.end(), it, end);
    Pos endPos = ctx.pos;
    dest << "($pushpos) ($pos";
    outPos(dest, macro.pos);
    dest.put(')');
    pushPos(ctx.posk, startPos);
    ctx.pos = macro.pos;
    macroExpand2(dest, ctx, map, macro.body.begin(), macro.body.end());
    dest << "($poppos) ($pos";
    outPos(dest, endPos);
    dest.put(')');
    popPos(ctx.posk);
    ctx.pos = endPos;
  }

  void outPos(ostream &dest, const Pos &pos) {
    dest << " '";
    outString(dest, *pos.file);
    dest << ' ' << pos.li;
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

  void macroExpand2(ostream &dest, MacroExpMap &map, It it, It end) {
    if (it == end) [[unlikely]] {
      return;
    }
  }

  static Pound pound;
  static Pound pound2;
  static Pound poundDef;
  static Pound poundIf;
  static Pound poundInclude;
  static Pound poundMovedef;
  static Pound poundUndef;
}
