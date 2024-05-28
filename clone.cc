#include"nodes.hh"
#include"preproc.hh"

using namespace std;

namespace zlt::mylispc {
  void clone(UNode &dest, const NumberAtom &src) {
    dest.reset(new NumberAtom(src));
  }

  void clone(UNode &dest, const StringAtom &src) {
    dest.reset(new StringAtom(src));
  }

  void clone(UNode &dest, const IDAtom &src) {
    dest.reset(new IDAtom(src));
  }

  void clone(UNode &dest, const TokenAtom &src) {
    dest.reset(new TokenAtom(src));
  }

  void clone(UNode &dest, const List &src) {
    UNodes items(src.items.size());
    clone(items, src.items.begin(), src.items.end());
    dest.reset(new List(src.pos, std::move(items)));
  }

  void clone(UNode &dest, const UNode &src) {
    if (auto a = dynamic_cast<const NumberAtom *>(src.get()); a) {
      clone(dest, *a);
    } else if (auto a = dynamic_cast<const StringAtom *>(src.get()); a) {
      clone(dest, *a);
    } else if (auto a = dynamic_cast<const IDAtom *>(src.get()); a) {
      clone(dest, *a);
    } else if (auto a = dynamic_cast<const TokenAtom *>(src.get()); a) {
      clone(dest, *a);
    } else {
      clone(dest, static_cast<const List &>(*src));
    }
  }

  void clone(UNodes &dest, UNodes::const_iterator it, UNodes::const_iterator end) {
    for (; it != end; ++it) {
      dest.push_back({});
      clone(dest.back(), *it);
    }
  }
}
