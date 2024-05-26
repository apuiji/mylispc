#include"preproc.hh"

using namespace std;

namespace zlt::mylispc {
  using Context = ExpandContext;
  using It = UNodes::const_iterator;

  static void makeMap(Context &ctx, Macro::ItParam itParam, Macro::ItParam endParam, It it, It end);
  static void expand1(UNodes &dest, Context &ctx, It endArg, const UNode &src);

  static inline void expand1(UNodes &dest, Context &ctx, It endArg, It it, It end) {
    for (; it != end; ++it) {
      expand1(dest, ctx, endArg, *it);
    }
  }

  void expand(UNodes &dest, Context &ctx, const Macro &macro, It it, It end) {
    makeMap(ctx, macro.params.begin(), macro.params.end(), it, end);
    expand1(dest, ctx, end, macro.body.begin(), macro.body.end());
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
    if (*itParam) {
      ctx.map[*itParam] = it;
    }
    makeMap(ctx, ++itParam, endParam, ++it, end);
  }

  void makeMap1(Context &ctx, Macro::ItParam itParam, Macro::ItParam endParam, It end) {
    for (; itParam != endParam; ++itParam) {
      if (*itParam) {
        ctx.map[*itParam] = end;
      }
    }
  }

  void expand1(UNodes &dest, Context &ctx, It endArg, const UNode &src) {
    if (auto a = dynamic_cast<const IDAtom *>(src.get()); a) {
      auto itMap = ctx.map.find(a->name);
      if (itMap == ctx.map.end()) {
        goto A;
      }
      if (itMap->second == endArg) {
        return;
      }
      if (string_view(*a->name).starts_with("...")) {
        clone(dest, itMap->second, endArg);
      } else {
        clone(dest, *itMap->second);
      }
      return;
    }
    if (auto a = dynamic_cast<const List *>(src.get()); a) {
      UNodes items;
      expand1(items, ctx, endArg, a->items.begin(), a->items.end());
      dest.push_back({});
      dest.back().reset(new List(a->pos, std::move(items)));
      return;
    }
    A:
    dest.push_back({});
    clone(dest.back(), src);
  }
}
