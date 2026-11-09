#pragma once

#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>

namespace kaixyhe {

template <typename... Ts>
class tuple;

// ---------------------------------------------------------------------------
// tuple_element / tuple_size
// ---------------------------------------------------------------------------

namespace tuple_detail {

template <std::size_t I, typename T>
struct leaf {
    T value;
};

template <typename Seq, typename... Ts>
struct tuple_impl;

template <std::size_t... Is, typename... Ts>
struct tuple_impl<std::index_sequence<Is...>, Ts...> : leaf<Is, Ts>... {
    tuple_impl() = default;

    template <typename... Us>
    explicit tuple_impl(Us&&... us) : leaf<Is, Ts>{std::forward<Us>(us)}... {}
};

template <std::size_t I, typename T>
leaf<I, T>& get_leaf(leaf<I, T>& node) noexcept {
    return node;
}

template <std::size_t I, typename T>
const leaf<I, T>& get_leaf(const leaf<I, T>& node) noexcept {
    return node;
}

}  // namespace tuple_detail

template <typename T>
struct tuple_size;

template <typename... Ts>
struct tuple_size<tuple<Ts...>> : std::integral_constant<std::size_t, sizeof...(Ts)> {};

template <typename T>
inline constexpr std::size_t tuple_size_v = tuple_size<T>::value;

template <std::size_t I, typename T>
struct tuple_element;

template <std::size_t I, typename Head, typename... Tail>
struct tuple_element<I, tuple<Head, Tail...>>
    : tuple_element<I - 1, tuple<Tail...>> {};

template <typename Head, typename... Tail>
struct tuple_element<0, tuple<Head, Tail...>> {
    using type = Head;
};

template <std::size_t I, typename T>
using tuple_element_t = typename tuple_element<I, T>::type;

// ---------------------------------------------------------------------------
// get<I>
// ---------------------------------------------------------------------------

template <std::size_t I, typename... Ts>
constexpr tuple_element_t<I, tuple<Ts...>>& get(tuple<Ts...>& t) noexcept {
    using impl_type = tuple_detail::tuple_impl<std::index_sequence_for<Ts...>, Ts...>;
    auto& leaf = tuple_detail::get_leaf<I>(static_cast<impl_type&>(t));
    return leaf.value;
}

template <std::size_t I, typename... Ts>
constexpr const tuple_element_t<I, tuple<Ts...>>& get(const tuple<Ts...>& t) noexcept {
    using impl_type = tuple_detail::tuple_impl<std::index_sequence_for<Ts...>, Ts...>;
    const auto& leaf = tuple_detail::get_leaf<I>(static_cast<const impl_type&>(t));
    return leaf.value;
}

template <std::size_t I, typename... Ts>
constexpr tuple_element_t<I, tuple<Ts...>>&& get(tuple<Ts...>&& t) noexcept {
    return std::move(kaixyhe::get<I>(static_cast<tuple<Ts...>&>(t)));
}

// ---------------------------------------------------------------------------
// tuple
// ---------------------------------------------------------------------------

template <typename... Ts>
class tuple : public tuple_detail::tuple_impl<std::index_sequence_for<Ts...>, Ts...> {
    using base = tuple_detail::tuple_impl<std::index_sequence_for<Ts...>, Ts...>;

 public:
    tuple() = default;

    explicit tuple(const Ts&... values) : base(values...) {}

    template <typename... Us>
    explicit tuple(Us&&... values) : base(std::forward<Us>(values)...) {}

    tuple& operator=(const tuple& other) {
        assign_impl(other, std::index_sequence_for<Ts...>{});
        return *this;
    }

    template <typename... Us>
    tuple& operator=(const tuple<Us...>& other) {
        assign_impl(other, std::index_sequence_for<Ts...>{});
        return *this;
    }

    void swap(tuple& other) { swap_impl(other, std::index_sequence_for<Ts...>{}); }

 private:
    template <typename Other, std::size_t... Is>
    void assign_impl(const Other& other, std::index_sequence<Is...>) {
        ((kaixyhe::get<Is>(*this) = kaixyhe::get<Is>(other)), ...);
    }

    template <typename Other, std::size_t... Is>
    void swap_impl(Other& other, std::index_sequence<Is...>) {
        (std::swap(kaixyhe::get<Is>(*this), kaixyhe::get<Is>(other)), ...);
    }
};

template <typename... Ts>
tuple(Ts...) -> tuple<Ts...>;

// ---------------------------------------------------------------------------
// make_tuple / tie / forward_as_tuple
// ---------------------------------------------------------------------------

template <typename... Ts>
constexpr tuple<std::decay_t<Ts>...> make_tuple(Ts&&... args) {
    return tuple<std::decay_t<Ts>...>(std::forward<Ts>(args)...);
}

template <typename... Ts>
constexpr tuple<Ts&...> tie(Ts&... args) noexcept {
    return tuple<Ts&...>(args...);
}

template <typename... Ts>
constexpr tuple<Ts&&...> forward_as_tuple(Ts&&... args) noexcept {
    return tuple<Ts&&...>(std::forward<Ts>(args)...);
}

// ---------------------------------------------------------------------------
// apply
// ---------------------------------------------------------------------------

namespace tuple_detail {

template <typename Fn, typename Tuple, std::size_t... Is>
constexpr decltype(auto) apply_impl(Fn&& fn, Tuple&& t, std::index_sequence<Is...>) {
    return std::invoke(std::forward<Fn>(fn),
                       kaixyhe::get<Is>(std::forward<Tuple>(t))...);
}

}  // namespace tuple_detail

template <typename Fn, typename Tuple>
constexpr decltype(auto) apply(Fn&& fn, Tuple&& t) {
    return tuple_detail::apply_impl(
        std::forward<Fn>(fn), std::forward<Tuple>(t),
        std::make_index_sequence<tuple_size_v<std::decay_t<Tuple>>>{});
}

}  // namespace kaixyhe
