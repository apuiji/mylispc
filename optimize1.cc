#include"myutils/xyz.hh"
#include"nodes1.hh"

using namespace std;

namespace zlt::mylispc {
  using It = UNodes::iterator;

  static bool isTerminated(const UNode &src) noexcept;

  bool optimizeBody(UNodes &dest, It it, It end) {
    if (it == end) [[unlikely]] {
      return true;
    }
    It last = prev(end);
    for (; it != last; ++it) {
      if (Dynamicastable<CharAtom, Function, ID, Number, StringAtom> {}(**it)) {
        continue;
      }
      if (auto a = dynamic_cast<SequenceOper *>(it->get()); a) {
        if (!optimizeBody(dest, a->items.begin(), a->items.end())) {
          break;
        }
        continue;
      }
      dest.push_back(std::move(*it));
      if (isTerminated(*it)) {
        return false;
      }
    }
    dest.push_back(std::move(*last));
    return true;
  }

  bool isTerminated(const UNode &src) noexcept {
    if (Dynamicastable<Forward, GlobalForward, GlobalReturn, Return, Throw> {}(*src)) {
      return true;
    }
    if (auto a = dynamic_cast<const If *>(src.get()); a) {
      return isTerminated(a->then) && isTerminated(a->elze);
    }
    return false;
  }
}
