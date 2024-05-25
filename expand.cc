#include"preproc.hh"

using namespace std;

namespace zlt::mylispc {
  using Context = ExpandContext;
  using It = UNodes::const_iterator;

  static void makeMap(Context &ctx, Macro::ItParam itParam, Macro::ItParam endParam, UNodes &src, It it, It end);
  static void expand1(ostream &dest, Context &ctx, It endArg, const UNode &src);

  static inline void expand1(ostream &dest, Context &ctx, It endArg, It it, It end) {
    for (; it != end; ++it) {
      expand1(dest, ctx, endArg, *it);
    }
  }

  void expand(ostream &dest, Context &ctx, const Macro &macro, UNodes &src) {
    makeMap(ctx, macro.params.begin(), macro.params.end(), src, src.begin(), src.end());
    dest << "($pushpos)";
    preproc_output::outputPos1(dest, macro.pos);
    expand1(dest, ctx, src.end(), macro.body.begin(), macro.body.end());
    dest << "($poppos)";
  }

  static void makeMap1(Context &ctx, Macro::ItParam itParam, Macro::ItParam endParam, It end);

  void makeMap(Context &ctx, Macro::ItParam itParam, Macro::ItParam endParam, UNodes &src, It it, It end) {
    if (itParam == endParam) [[unlikely]] {
      return;
    }
    if (it == end) [[unlikely]] {
      makeMap1(ctx, itParam, endParam, end);
      return;
    }
    if (Dynamicastable<EOLAtom> {}(**it)) {
      ++ctx.pos.li;
      makeMap(ctx, itParam, endParam, src, src.erase(it), end);
      return;
    }
    if (*itParam) {
      ctx.map[*itParam] = make_pair(ctx.pos, it);
    }
    makeMap(ctx, ++itParam, endParam, src, ++it, end);
  }

  void makeMap1(Context &ctx, Macro::ItParam itParam, Macro::ItParam endParam, It end) {
    for (; itParam != endParam; ++itParam) {
      if (*itParam) {
        ctx.map[*itParam] = make_pair(ctx.pos, end);
      }
    }
  }

  static void expand2(ostream &dest, const UNode &src);

  static inline void expand2(ostream &dest, It it, It end) {
    for (; it != end; ++it) {
      expand2(dest, *it);
    }
  }

  void expand1(ostream &dest, Context &ctx, It endArg, const UNode &src) {
    if (Dynamicastable<EOLAtom> {}(*src)) {
      dest << endl;
      ++ctx.pos.li;
      return;
    }
    dest.put(' ');
    if (auto a = dynamic_cast<const StringAtom *>(src.get()); a) {
      preproc_output::outputStr1(dest, *a->value);
      return;
    }
    if (auto a = dynamic_cast<const IDAtom *>(src.get()); a) {
      auto itMap = ctx.map.find(a->name);
      if (itMap == ctx.map.end()) {
        goto A;
      }
      if (itMap->second == endArg) {
        return;
      }
      dest << "($pushpos)";
      preproc_output::outputPos1(dest, itMap->first);
      if (string_view(*a->name).starts_with("...")) {
        expand2(dest, itMap->second, endArg);
      } else {
        expand2(dest, *itMap->second);
      }
      dest << "($poppos)";
      preproc_output::outputPos1(dest, ctx.pos);
      return;
    }
    if (auto a = dynamic_cast<const List *>(src.get()); a) {
      dest.put('(');
      expand1(dest, ctx, endArg, a->item.begin(), a->items.end());
      dest.put(')');
    }
    A:
    dest << static_cast<const RawAtom &>(*src).raw();
  }

  void expand2(ostream &dest, const UNode &src) {
    if (Dynamicastable<EOLAtom> {}(*src)) {
      dest << endl;
      return;
    }
    dest.put(' ');
    if (auto a = dynamic_cast<const StringAtom *>(src.get()); a) {
      preproc_output::outputStr1(dest, *a->value);
      return;
    }
    if (auto a = dynamic_cast<const List *>(src.get()); a) {
      dest.put('(');
      expand2(dest, a->items.begin(), a->items.end());
      dest.put(')');
      return;
    }
    dest << static_cast<const RawAtom &>(*src).raw();
  }
}
