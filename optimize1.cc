#include"nodes1.hh"
#include"trans.hh"
#include"zlt/xyz.hh"

using namespace std;

namespace zlt::mylispc {
  using It = UNodes::iterator;

  static bool isTerminated(const UNode &src) noexcept;

  bool optimizeBody(UNodes &dest, UNodes &src) {
    if (src.empty()) [[unlikely]] {
      return true;
    }
    for (; src.size() > 1; src.pop_front()) {
      if (Dynamicastable<Function, ID, Number, StringAtom> {}(*src.front())) {
        continue;
      }
      if (auto a = dynamic_cast<SequenceOper *>(src.front().get()); a) {
        if (!optimizeBody(dest, a->items)) {
          break;
        }
        continue;
      }
      dest.push_back(std::move(src.front()));
      if (isTerminated(dest.back())) {
        return false;
      }
    }
    dest.push_back(std::move(src.front()));
    return true;
  }

  bool isTerminated(const UNode &src) noexcept {
    if (Dynamicastable<Forward, Return, Throw> {}(*src)) {
      return true;
    }
    if (auto a = dynamic_cast<const If *>(src.get()); a) {
      return isTerminated(a->then) && isTerminated(a->elze);
    }
    return false;
  }
}
