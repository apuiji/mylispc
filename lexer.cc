#include<algorithm>
#include<cctype>
#include<cstring>
#include<sstream>
#include"mylispc.hh"
#include"token.hh"

using namespace std;

namespace zlt::mylispc {
  using It = const char *;

  It hit(It it, It end) noexcept {
    if (it == end) [[unlikely]] {
      return end;
    }
    if (isspace(*it)) {
      return hit(it + 1, end);
    }
    if (*it == ';') {
      return hit(find(it + 1, end, '\n'), end);
    }
    return it;
  }

  using Prod = pair<int, It>;

  static Prod lexerStr(char &charval, string &strval, stringstream &ss, It start, It it, It end);
  static bool isRawChar(char c) noexcept;

  Prod lexer(double &numval, char &charval, string &strval, It it, It end) {
    if (it == end) [[unlikely]] {
      return { token::E0F, end };
    }
    if (*it == '(') {
      return { "("_token, it + 1 };
    }
    if (*it == ')') {
      return { ")"_token, it + 1 };
    }
    if (*it == '"' || *it == '\'') {
      stringstream ss;
      return lexerStr(charval, strval, ss, it, it + 1, end);
    }
    It it1 = find_if_not(it, end, isRawChar);
    if (it1 == it) {
      throw Bad(bad::UNRECOGNIZED_SYMBOL, it);
    }
    string_view raw(it, it1 - it);
    int t = token::ofRaw(numval, it, raw);
    return { t, it1 };
  }

  bool isRawChar(char c) noexcept {
    return !strchr("\"'();", c) && !isspace(c);
  }

  static pair<int, size_t> esch(It it, It end) noexcept;

  Prod lexerStr(char &charval, string &strval, stringstream &ss, It start, It it, It end) {
    if (it == end) [[unlikely]] {
      throw Bad(bad::UNTERMINATED_STRING, start);
    }
    if (It it1 = find_if(it, end, [q = *start] (char c) { return c == '\\' || c == q; }); it1 != it) {
      ss.write(it, it1 - it);
      return lexerStr(charval, strval, ss, start, it1, end);
    }
    if (*it == '\\') {
      auto [c, n] = esch(it + 1, end);
      ss.put(c);
      return lexerStr(charval, strval, ss, start, it + 1 + n, end);
    }
    strval = ss.str();
    return { token::STRING, it + 1 };
  }

  static bool esch1(int &c, size_t &n, It it, It end) noexcept;
  static bool esch8(int &c, size_t &n, It it, It end) noexcept;
  static bool esch16(int &c, size_t &n, It it, It end) noexcept;

  pair<int, size_t> esch(It it, It end) noexcept {
    int c = '\\';
    size_t n = 0;
    if (it != end) {
      esch1(c, n, it, end) || esch8(c, n, it, end) || esch16(c, n, it, end);
    }
    return { c, n };
  }

  bool esch1(int &c, size_t &n, It it, It end) noexcept {
    if (*it == '"' || *it == '\'' || *it == '\\') {
      c = *it;
    } else if (*it == 'n') {
      c = '\n';
    } else if (*it == 'r') {
      c = '\r';
    } else if (*it == 't') {
      c = '\t';
    } else {
      return false;
    }
    n = 1;
    return true;
  }

  bool esch8(int &c, size_t &n, It it, It end) noexcept {
    if (it + 2 < end && *it >= '0' && *it <= '3' && it[1] >= '0' && it[1] <= '7' && it[2] >= '0' && it[2] <= '7') {
      c = ((*it - '0') << 6) | ((it[1] - '0') << 3) | (it[2] - '0');
      n = 3;
      return true;
    }
    if (it + 1 < end && *it >= '0' && *it <= '7' && it[1] >= '0' && it[1] <= '7') {
      c = ((*it - '0') << 3) | (it[1] - '0');
      n = 2;
      return true;
    }
    if (*it >= '0' && *it <= '7') {
      c = *it - '0';
      n = 1;
      return true;
    }
    return false;
  }

  bool esch16(int &c, size_t &n, It it, It end) noexcept {
    if (it + 2 < end && *it == 'x' && isxdigit(it[1]) && isxdigit(it[2])) {
      c = stoi(string(it + 1, it + 3), nullptr, 16);
      n = 3;
      return true;
    }
    return false;
  }
}
