#include"preproc.hh"

using namespace std;

namespace zlt::mylispc::preproc_output {
  static inline bool spechar(unsigned int c) noexcept {
    return c < 32 || c == '\'' || c == '\\' || c >= 128;
  }

  void outputStr(ostream &dest, const unsigned char *it, const unsigned char *end) {
    if (it == end) [[unlikely]] {
      return;
    }
    if (auto it1 = find_if(it, end, spechar); it1 != it) {
      dest.write(it, it1 - it);
      outputStr(dest, it1, end);
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
    outputStr(dest, it + 1, end);
  }

  void outputPos(ostream &dest, const Pos &pos) {
    outputStr1(dest, *pos.file);
    dest << ' ' << pos.li;
  }

  void outputPos1(ostream &dest, const Pos &pos) {
    dest << "($pos ";
    outputPos(dest, pos);
    dest.put(')');
  }
}
