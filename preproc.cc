#include<algorithm>
#include<cstring>
#include<fstream>
#include<iterator>
#include<sstream>
#include"mylisp/mylisp.hh"
#include"myutils/xyz.hh"
#include"nodes.hh"
#include"token.hh"

using namespace std;

namespace zlt::mylispc {
  using It = UNodes::const_iterator;

  static void clone(UNodes &dest, const UNode &src);

  static inline void clone(UNodes &dest, It it, It end) {
    for (; it != end; ++it) {
      clone(dest, *it);
    }
  }

  template<AnyOf<NumberAtom, CharAtom, StringAtom, IDAtom, TokenAtom> T>
  static inline void clone(UNodes &dest, const T &src) {
    dest.push_back({});
    dest.back().reset(new T(src));
  }

  static void clone(UNodes &dest, const List &src) {
    UNodes items;
    clone(items, src.items.begin(), src.items.end());
    dest.push_back({});
    dest.back().reset(new List(src.start, std::move(items)));
  }

  void clone(UNodes &dest, const UNode &src) {
    #define ifType(T) \
    if (auto a = dynamic_cast<const T *>(src.get()); a) { \
      clone(dest, *a); \
      return; \
    }
    ifType(NumberAtom);
    ifType(CharAtom);
    ifType(StringAtom);
    ifType(IDAtom);
    ifType(TokenAtom);
    #undef ifType
    clone(dest, static_cast<const List &>(*src));
  }

  static void preprocList(UNodes &dest, Sources &srcs, Macros &macros, const char *start, It it, It end);

  void preproc(UNodes &dest, Sources &srcs, Macros &macros, const UNode &src) {
    if (auto a = dynamic_cast<const List *>(src.get()); a && a->items.size()) {
      preprocList(dest, srcs, macros, a->start, a->items.begin(), a->items.end());
      return;
    }
    clone(dest, src);
  }

  using Preproc1 = void (UNodes &dest, Sources &srcs, Macros &macros, const char *start, It it, It end);

  template<int>
  Preproc1 preproc1;

  #define declPreproc1(T) \
  template<> \
  Preproc1 preproc1<T##_token>

  declPreproc1("#");
  declPreproc1("##");
  declPreproc1("#def");
  declPreproc1("#if");
  declPreproc1("#include");
  declPreproc1("#undef");

  #undef declPreproc1

  static Preproc1 *isPreproc1(const UNode &src) noexcept;

  static inline Macros::iterator findMacro(Macros &macros, const UNode &src) noexcept {
    auto id = dynamic_cast<const IDAtom *>(src.get());
    return id ? macros.find(id->name) : macros.end();
  }

  static void macroExpand(UNodes &dest, const Macro &macro, It it, It end);

  void preprocList(UNodes &dest, Sources &srcs, Macros &macros, const char *start, It it, It end) {
    if (auto preproc1 = isPreproc1(*it); preproc1) {
      preproc1(dest, srcs, macros, start, ++it, end);
      return;
    }
    if (auto itMacro = findMacro(macros, *it); itMacro != macros.end()) {
      UNodes a;
      macroExpand(a, itMacro->second, ++it, end);
      preproc(dest, srcs, macros, a.begin(), a.end());
      return;
    }
    UNodes items;
    preproc(items, srcs, macros, ++it, end);
    dest.push_back({});
    dest.back().reset(new List(start, std::move(items)));
  }

  Preproc1 *isPreproc1(const UNode &src) noexcept {
    auto ta = dynamic_cast<const TokenAtom *>(src.get());
    if (!ta) {
      return nullptr;
    }
    #define ifToken(T) \
    if (ta->token == T##_token) { \
      return preproc1<T##_token>; \
    }
    ifToken("#")
    ifToken("##")
    ifToken("#def")
    ifToken("#if")
    ifToken("#include")
    ifToken("#undef")
    #undef ifToken
    return nullptr;
  }

  using MacroExpandMap = map<const string *, It>;

  static void makeMacroExpandMap(MacroExpandMap &dest, Macro::ItParam itParam, Macro::ItParam endParam, It it, It end);
  static void macroExpand1(UNodes &dest, const MacroExpandMap &map, It endArg, const UNode &src);

  static inline void macroExpand1(UNodes &dest, const MacroExpandMap &map, It endArg, It it, It end) {
    for (; it != end; ++it) {
      macroExpand1(dest, map, endArg, *it);
    }
  }

  void macroExpand(UNodes &dest, const Macro &macro, It it, It end) {
    MacroExpandMap map;
    makeMacroExpandMap(map, macro.params.begin(), macro.params.end(), it, end);
    macroExpand1(dest, map, end, macro.body.begin(), macro.body.end());
  }

  static void makeMacroExpandMap1(MacroExpandMap &dest, Macro::ItParam itParam, Macro::ItParam endParam, It end);

  void makeMacroExpandMap(MacroExpandMap &dest, Macro::ItParam itParam, Macro::ItParam endParam, It it, It end) {
    for (; itParam != endParam; ++itParam, ++it) {
      if (it == end) [[unlikely]] {
        makeMacroExpandMap1(dest, itParam, endParam, end);
        return;
      }
      if (*itParam) {
        dest[*itParam] = it;
      }
    }
  }

  void makeMacroExpandMap1(MacroExpandMap &dest, Macro::ItParam itParam, Macro::ItParam endParam, It end) {
    for (; itParam != endParam; ++itParam) {
      if (*itParam) {
        dest[*itParam] = end;
      }
    }
  }

  static void macroExpand2(UNodes &dest, const MacroExpandMap &map, It endArg, const IDAtom &src);
  static void macroExpand2(UNodes &dest, const MacroExpandMap &map, It endArg, const List &src);

  void macroExpand1(UNodes &dest, const MacroExpandMap &map, It endArg, const UNode &src) {
    if (auto a = dynamic_cast<const IDAtom *>(src.get()); a) {
      macroExpand2(dest, map, endArg, *a);
      return;
    }
    if (auto a = dynamic_cast<const List *>(src.get()); a) {
      macroExpand2(dest, map, endArg, *a);
      return;
    }
    clone(dest, src);
  }

  void macroExpand2(UNodes &dest, const MacroExpandMap &map, It endArg, const IDAtom &src) {
    auto it = map.find(src.name);
    if (it == map.end()) {
      clone(dest, src);
      return;
    }
    if (strncmp(src.name->data(), "...", 3)) {
      clone(dest, *it->second);
    } else {
      clone(dest, it->second, endArg);
    }
  }

  void macroExpand2(UNodes &dest, const MacroExpandMap &map, It endArg, const List &src) {
    UNodes items;
    macroExpand1(items, map, endArg, src.items.begin(), src.items.end());
    dest.push_back({});
    dest.back().reset(new List(src.start, std::move(items)));
  }

  template<>
  void preproc1<"#"_token>(UNodes &dest, Sources &srcs, Macros &macros, const char *start, It it, It end) {
    string s;
    if (it == end) {
    } else if (auto a = dynamic_cast<const RawAtom *>(it->get()); a) {
      s = a->raw;
    } else {
      throw Bad(bad::INV_PREPROC_ARG, (**it).start);
    }
    auto value = mylisp::addString(std::move(s));
    dest.push_back({});
    dest.back().reset(new StringAtom(start, value));
  }

  static void rawCat(stringstream &dest, const UNode &src);

  template<>
  void preproc1<"##"_token>(UNodes &dest, Sources &srcs, Macros &macros, const char *start, It it, It end) {
    stringstream ss;
    for (; it != end; ++it) {
      rawCat(ss, *it);
    }
    auto &raw = *mylisp::addString(ss.str());
    remove(ss);
    dest.push_back({});
    double d;
    int t = token::ofRaw(d, start, raw);
    if (t == token::NUMBER) {
      dest.back().reset(new NumberAtom(start, raw, d));
    } else if (t == token::ID) {
      dest.back().reset(new IDAtom(start, raw, &raw));
    } else {
      dest.back().reset(new TokenAtom(start, raw, t));
    }
  }

  void rawCat(stringstream &dest, const UNode &src) {
    auto a = dynamic_cast<const RawAtom *>(src.get());
    if (!a) {
      throw Bad(bad::INV_PREPROC_ARG, src->start);
    }
    dest << a->raw;
  }

  static void makeMacroParams(Macro::Params &dest, It it, It end);

  template<>
  void preproc1<"#def"_token>(UNodes &dest, Sources &srcs, Macros &macros, const char *start, It it, It end) {
    if (it == end) [[unlikely]] {
      return;
    }
    auto id = dynamic_cast<const IDAtom *>(it->get());
    if (!id) {
      throw Bad(bad::INV_PREPROC_ARG, (**it).start);
    }
    if (macros.find(id->name) != macros.end()) {
      throw Bad(bad::MACRO_ALREADY_DEFINED, start);
    }
    if (++it == end) {
      macros[id->name] = {};
      return;
    }
    auto ls = dynamic_cast<const List *>(it->get());
    if (!ls) {
      throw Bad(bad::INV_PREPROC_ARG, (**it).start);
    }
    Macro::Params params;
    makeMacroParams(params, ls->items.begin(), ls->items.end());
    UNodes body;
    clone(body, ++it, end);
    macros[id->name] = Macro(std::move(params), std::move(body));
  }

  void makeMacroParams(Macro::Params &dest, It it, It end) {
    if (it == end) [[unlikely]] {
      return;
    }
    if (auto id = dynamic_cast<const IDAtom *>(it->get()); id) {
      dest.push_back(id->name);
      if (!strncmp(id->name->data(), "...", 3)) {
        return;
      }
    } else if (auto ls = dynamic_cast<const List *>(it->get()); ls && ls->items.empty()) {
      dest.push_back(nullptr);
    } else {
      throw Bad(bad::ILL_MACRO_PARAM, (**it).start);
    }
    makeMacroParams(dest, ++it, end);
  }

  static void preprocIf(UNodes &dest, Sources &srcs, Macros &macros, const char *start, It it, It elze, It end);

  template<>
  void preproc1<"#if"_token>(UNodes &dest, Sources &srcs, Macros &macros, const char *start, It it, It end) {
    It elze = it;
    for (; elze != end && !isTokenAtom<"#if"_token>(*elze); ++elze);
    preprocIf(dest, srcs, macros, start, it, elze, end);
  }

  static void preprocElse(UNodes &dest, Sources &srcs, Macros &macros, It it, It end);
  static void preprocIfdef(UNodes &dest, Sources &srcs, Macros &macros, const char *start, It it, It elze, It end);

  void preprocIf(UNodes &dest, Sources &srcs, Macros &macros, const char *start, It it, It elze, It end) {
    if (it == elze) [[unlikely]] {
      preprocElse(dest, srcs, macros, elze, end);
      return;
    }
    if (isTokenAtom<"#def"_token>(*it)) {
      preprocIfdef(dest, srcs, macros, start, ++it, elze, end);
      return;
    }
    if (auto ls = dynamic_cast<const List *>(it->get()); ls && ls->items.empty()) {
      preprocElse(dest, srcs, macros, elze, end);
      return;
    }
    preproc(dest, srcs, macros, ++it, elze);
  }

  void preprocElse(UNodes &dest, Sources &srcs, Macros &macros, It it, It end) {
    if (it != end) {
      auto start = (**it).start;
      preproc1<"#if"_token>(dest, srcs, macros, start, ++it, end);
    }
  }

  void preprocIfdef(UNodes &dest, Sources &srcs, Macros &macros, const char *start, It it, It elze, It end) {
    if (it == end) [[unlikely]] {
      return;
    }
    auto id = dynamic_cast<const IDAtom *>(it->get());
    if (!id) {
      throw Bad(bad::INV_PREPROC_ARG, (**it).start);
    }
    if (macros.find(id->name) == macros.end()) {
      preprocElse(dest, srcs, macros, elze, end);
      return;
    }
    preproc(dest, srcs, macros, ++it, elze);
  }

  static UNodes &include(Sources &srcs, const char *start, const UNode &src);

  template<>
  void preproc1<"#include"_token>(UNodes &dest, Sources &srcs, Macros &macros, const char *start, It it, It end) {
    if (it == end) [[unlikely]] {
      return;
    }
    auto &a = include(srcs, start, *it);
    preproc(dest, srcs, macros, a.begin(), a.end());
  }

  UNodes &include(Sources &srcs, const char *start, const UNode &src) {
    filesystem::path path;
    if (auto a = dynamic_cast<const RawAtom *>(src.get()); a) {
      path = a->raw;
    } else if (auto a = dynamic_cast<const CharAtom *>(src.get()); a) {
      path = string(&a->value, 1);
    } else if (auto a = dynamic_cast<const StringAtom *>(src.get()); a) {
      path = *a->value;
    } else {
      throw Bad(bad::INV_PREPROC_ARG, src->start);
    }
    if (path.is_relative()) {
      auto it = where(srcs, start);
      if (it != srcs.end()) {
        path = it->first.parent_path() / path;
      }
    }
    try {
      path = filesystem::canonical(path);
    } catch (...) {
      throw Bad(bad::CANNOT_OPEN_FILE, start);
    }
    auto it = srcs.find(path);
    if (it != srcs.end()) {
      return it->second.body;
    }
    string raw;
    {
      ifstream ifs(path);
      stringstream ss;
      copy(istreambuf_iterator<char>(ifs), istreambuf_iterator<char>(), ostreambuf_iterator<char>(ss));
      raw = ss.str();
    }
    UNodes body;
    parse(body, raw.data(), raw.data() + raw.size());
    Sources::value_type a(std::move(path), Source(std::move(raw), std::move(body)));
    auto it1 = srcs.insert(it, std::move(a));
    return it1->second.body;
  }

  template<>
  void preproc1<"#undef"_token>(UNodes &dest, Sources &srcs, Macros &macros, const char *start, It it, It end) {
    if (it == end) [[unlikely]] {
      return;
    }
    auto it1 = findMacro(macros, *it);
    if (it1 != macros.end()) {
      macros.erase(it1);
    }
  }
}
