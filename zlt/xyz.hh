#pragma once

#include<compare>
#include<concepts>
#include<memory>
#include"xyz.h"

namespace zlt {
  template<class T, class U>
  static inline constexpr T &memberOf(void *p, T U::*m) noexcept {
    return ((U *) p)->*m;
  }

  template<class T, class U>
  static inline constexpr const T &memberOf(const void *p, T U::*m) noexcept {
    return ((const U *) p)->*m;
  }

  template<class T, class U>
  static inline constexpr size_t offsetOf(T U::*m) noexcept {
    return (size_t) ((T *) nullptr)->*m;
  }

  template<class U, class T>
  static inline constexpr U &containerOf(T &t, T U::*m) noexcept {
    return *(U *) ((void *) &t - offsetOf(m));
  }

  template<class U, class T>
  static inline constexpr const U &containerOf(const T &t, T U::*m) noexcept {
    return *(const U *) ((void *) &t - offsetOf(m));
  }

  static inline void memSwap(void *a, void *b, size_t size) noexcept {
    zltMemSwap(a, b, size);
  }

  #ifdef __cpp_lib_three_way_comparison

  using Compare = std::compare_three_way;

  #else

  struct Compare {
    template<class T, class U>
    auto operator ()(T &&t, U &&u) const {
      return t <=> u;
    }
  };

  #endif

  // type cast begin
  template<class T, class ...U>
  static constexpr bool isAnyOf = (std::is_same_v<T, U> || ...);

  template<class T, class ...U>
  concept AnyOf = isAnyOf<T, U...>;

  template<class ...T>
  struct Dynamicastable {
    template<class U>
    requires (std::is_pointer_v<U>)
    bool operator ()(const U u) noexcept {
      return u && (dynamic_cast<const T*>(u) || ...);
    }
    template<class U>
    requires (!std::is_pointer_v<U>)
    bool operator ()(const U &u) noexcept {
      return operator ()(&u);
    }
  };

  /// overloaded function resolve
  template<class ...Args>
  struct OFR {
    template<class R>
    constexpr auto operator ()(R (*f)(Args...)) const noexcept {
      return f;
    }
  };

  template<class T>
  static inline T remove(T &t) noexcept {
    return std::move(t);
  }
  // type cast end

  template<std::invocable T>
  static inline auto makeGuard(T &&t) noexcept {
    struct Guard {
      T t;
      Guard(T &&t) noexcept: t(std::move(t)) {}
      ~Guard() {
        t();
      }
    };
    return Guard(std::move(t));
  }
}
