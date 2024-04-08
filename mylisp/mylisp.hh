#pragma once

#include<set>
#include<string>

namespace zlt::mylisp {
  extern std::set<std::string> bodies;
  extern std::set<std::string> strings;

  const std::string *addString(std::string &&s);
}
