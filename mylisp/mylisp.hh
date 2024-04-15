#pragma once

#include<set>

namespace zlt::mylisp {
  extern std::set<std::string> strings;

  const std::string *addString(std::string &&s);
}
