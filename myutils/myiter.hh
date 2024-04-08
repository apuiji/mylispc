#pragma once

#include<algorithm>
#include<concepts>
#include<iterator>
#if __has_include(<ranges>)
#include<ranges>
#endif

namespace zlt::myiter {
  template<class It, class T>
  concept IteratorOf = std::is_same_v<std::iter_value_t<It>, T>;

  template<class T, class U>
  concept RangeOf = requires (T &t) {
    { t.begin() } -> IteratorOf<U>;
    { t.end() } -> IteratorOf<U>;
  };

  #ifdef __cpp_lib_ranges

  template<class R, class F>
  static inline auto forEach(R &&r, F &&f) {
    return std::ranges::for_each(std::forward<R>(r), std::forward<F>(f));
  }

  template<class I, class S>
  static inline auto range(I &&i, S &&s) {
    return std::ranges::subrange(std::forward<I>(i), std::forward<S>(s));
  }

  template<class R>
  static inline auto reverseView(R &&r) {
    return std::ranges::reverse_view(std::forward<R>(r));
  }

  template<class R, class F>
  static inline auto transformView(R &&r, F &&f) {
    return std::ranges::transform_view(std::forward<R>(r), std::forward<F>(f));
  }

  #else

  template<class R, class F>
  static inline auto forEach(R &&r, F &&f) {
    return std::for_each(r.begin(), r.end(), std::forward<F>(f));
  }

  template<std::input_or_output_iterator It>
  static inline auto range(const It &begin, const It &end) {
    struct Range {
      It begin_;
      It end_;
      Range(const It &begin, const It &end): begin_(begin), end_(end) {}
      It begin() const {
        return begin_;
      }
      It end() const {
        return end_;
      }
    };
    return Range(begin, end);
  }

  template<class R>
  static inline auto reverseView(R &&r) {
    return range(std::make_reverse_iterator(r.begin()), std::make_reverse_iterator(r.end()));
  }

  template<class It, class F>
  struct TransformIterator {
    using value_type = decltype(std::declval<F>()(*std::declval<It>()));
    using difference_type = typename It::difference_type;
    It value;
    F *f;
    TransformIterator(const It &value, F *f): value(value), f(f) {}
    bool operator ==(const TransformIterator &end) const {
      return value == end.value;
    }
    value_type operator *() {
      return (*f)(*value);
    }
    auto &operator ++() {
      ++value;
      return *this;
    }
    auto operator ++(int) {
      auto a = *this;
      ++value;
      return a;
    }
  };

  template<class It, class F>
  static inline auto makeTransformIterator(const It &it, F *f) {
    return TransformIterator<It, F>(it, f);
  }

  template<class R, class F>
  static inline auto transformView(R &&r, F &&f) {
    using It = decltype(r.begin());
    using G = std::remove_cvref_t<F>;
    struct Range {
      It begin_;
      It end_;
      G g;
      Range(const It &begin, const It &end, F &&f): begin_(begin), end_(end), g(std::forward<F>(f)) {}
      auto begin() const {
        return makeTransformIterator(begin_, &g);
      }
      auto end() const {
        return makeTransformIterator(end_, &g);
      }
    };
    return Range(r.begin(), r.end(), std::move(f));
  }

  #endif

  template<size_t I, class R>
  static inline auto elementsView(R &&r) {
    return transformView(std::forward<R>(r), [] (auto &&a) { return std::get<I>(a); });
  }
}
