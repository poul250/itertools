#pragma once

#include <ranges>

namespace itertools {

namespace details {

struct Empty {};

template <bool Present, typename T>
using maybe_present_t = std::conditional<Present, T, Empty>;

template <bool Const, typename T>
using maybe_const_t = std::conditional_t<Const, const T, T>;

template <typename Callable>
struct RangeAdaptor {
 private:
  // We do not store the functor in the adapter
  // if it can be constructed by default
  [[no_unique_address]] maybe_present_t<
      !std::is_default_constructible_v<Callable>, Callable>
      callable_;

 public:
  constexpr RangeAdaptor(
      const Callable& = {}) requires std::is_default_constructible_v<Callable> {
  }

  constexpr RangeAdaptor(Callable callable_) requires(
      !std::is_default_constructible_v<Callable>)
      : callable_{std::move(callable_)} {}

  template <typename... Args>
  requires(sizeof...(Args) >= 1) constexpr auto operator()(Args&&... args) {
    // TODO do something
  }
};

}  // namespace details

template <std::ranges::view Range>
class enumerate_view
    : public std::ranges::view_interface<enumerate_view<Range>> {
 private:
  template <bool Const>
  struct sentinel;

  template <bool Const>
  struct iterator {
   private:
    static constexpr auto iter_concept_() {
      if constexpr (std::ranges::random_access_range<Range>) {
        return std::random_access_iterator_tag{};
      } else if constexpr (std::ranges::bidirectional_range<Range>) {
        return std::bidirectional_iterator_tag{};
      } else if constexpr (std::ranges::forward_range<Range>) {
        return std::forward_iterator_tag{};
      } else {
        return std::input_iterator_tag{};
      }
    }

    using base_t = details::maybe_const_t<Const, Range>;
    using base_iter_t = std::ranges::iterator_t<base_t>;

    base_iter_t current_it_;
    std::size_t current_ = 0;

   public:
    using value_type = std::ranges::range_value_t<base_t>;
    using difference_type = std::ranges::range_difference_t<base_t>;

    iterator() = default;

    constexpr iterator(base_iter_t current) : current_it_(std::move(current)) {}

    constexpr iterator(iterator<!Const> it) requires Const&& std::
        convertible_to<std::ranges::iterator_t<Range>,
                       std::ranges::iterator_t<base_t>>
        : current_it_(std::move(it.current_it_)) {}

    constexpr base_iter_t base() const& requires std::copyable<base_iter_t> {
      return current_it_;
    }

    constexpr base_iter_t base() && { return std::move(current_it_); }

    constexpr std::pair<std::size_t, std::ranges::range_reference_t<base_t>>
    operator*() const {
      return {current_, *current_it_};
    }

    constexpr iterator& operator++() {
      ++current_;
      ++current_it_;
      return *this;
    }

    constexpr iterator operator++(
        int) requires std::ranges::forward_range<base_t> {
      auto result = *this;
      ++*this;
      return result;
    }

    constexpr iterator&
    operator--() requires std::ranges::bidirectional_range<base_t> {
      --current_;
      --current_it_;
      return *this;
    }

    constexpr iterator operator--(
        int) requires std::ranges::bidirectional_range<base_t> {
      auto result = *this;
      --*this;
      return result;
    }

    friend constexpr bool operator==(const iterator& lhs, const iterator& rhs) {
      return lhs.current_it_ == rhs.current_it_;
    }

    friend constexpr bool operator!=(const iterator& lhs, const iterator& rhs) {
      return lhs.current_it_ != rhs.current_it_;
    }

    friend struct iterator<!Const>;
    template <bool Const2>
    struct sentinel;
  };

  template <bool Const>
  struct sentinel {
   private:
    using base_t = details::maybe_const_t<Const, Range>;
    using base_sentinel_t = std::ranges::sentinel_t<base_t>;

    std::ranges::sentinel_t<base_t> end_ = std::ranges::sentinel_t<base_t>();

   public:
    sentinel() = default;

    constexpr explicit sentinel(base_sentinel_t end) : end_(std::move(end)) {}

    constexpr sentinel(sentinel<!Const> s) requires Const&& std::convertible_to<
        std::ranges::sentinel_t<Range>, base_sentinel_t>
        : end_(std::move(s.end_)) {}

    constexpr base_sentinel_t base() const { return end_; }

    template <bool Const2>
    requires std::sentinel_for<std::ranges::sentinel_t<base_t>,
                               std::ranges::iterator_t<details::maybe_const_t<
                                   Const2, Range>>> friend constexpr bool
    operator==(const iterator<Const2>& lhs, const sentinel& rhs) {
      return lhs.current_it_ == rhs.end_;
    }

    friend struct sentinel<!Const>;
  };

  Range base_ = Range();

 public:
  enumerate_view() = default;

  constexpr enumerate_view(Range base) : base_(std::move(base)) {}

  constexpr Range base() const& requires std::copy_constructible<Range> {
    return base_;
  }

  constexpr Range base() && { return std::move(base_); }

  constexpr iterator<false> begin() {
    return iterator<false>{std::ranges::begin(base_)};
  }

  constexpr iterator<true> begin() const
      requires std::ranges::range<const Range> {
    return iterator<true>{std::ranges::begin(base_)};
  }

  constexpr sentinel<false> end() {
    return sentinel<false>{std::ranges::end(base_)};
  }

  constexpr iterator<false> end() requires std::ranges::common_range<Range> {
    return iterator<false>{std::ranges::end(base_)};
  }

  constexpr sentinel<true> end() const
      requires std::ranges::range<const Range> {
    return sentinel<true>{std::ranges::end(base_)};
  }

  constexpr sentinel<true> end() const requires std::ranges::common_range<
      Range>&& std::ranges::range<const Range> {
    return sentinel<true>{std::ranges::end(base_)};
  }

  constexpr auto size() const requires std::ranges::sized_range<Range> {
    return std::ranges::size(base_);
  }

  constexpr auto size() const requires std::ranges::sized_range<const Range> {
    return std::ranges::size(base_);
  }
};

template <typename Range>
enumerate_view(Range &&) -> enumerate_view<std::views::all_t<Range>>;

inline constexpr auto enumerate =
    []<std::ranges::viewable_range Range>(Range&& r) {
      return enumerate_view{std::forward<Range>(r)};
    };

}  // namespace itertools
