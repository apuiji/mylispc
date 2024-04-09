#pragma once

#include<list>
#include<set>
#include<string>

namespace zlt::mylisp {
  struct Object;

  using Value = std::variant<std::monostate, double, char, const std::string *, Object *, void (*)(void *, void *)>;

  struct Coroutine {
    Value ax;
    Value *bp;
    Value *sp;
    Value *tdsp;
    const char *pc;
    struct {
      Value *data;
      size_t size;
    } valuek;
  };

  using Coroutines = std::list<Coroutine>;
  using ItCoroutine = Coroutines::iterator;

  extern Coroutines coroutines;
  extern ItCoroutine itCoroutine;
  extern std::set<std::string> bodies;
  extern std::set<std::string> strings;

  const std::string *addString(std::string &&s);
  void exec();
}
