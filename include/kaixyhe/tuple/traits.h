#pragma once

#include <cstddef>
#include <type_traits>

#include "kaixyhe/tuple/tuple.h"

namespace kaixyhe {

// ---------------------------------------------------------------------------
// primary template: 匹配失败，交由各级特化逐层剥离
// ---------------------------------------------------------------------------
template <typename T>
struct member_function_traits;

// ---------------------------------------------------------------------------
// 成员函数指针的类型形如 Return (Class::*)(Args...) cv ref noexcept。
// cv(4) × ref-qualifier(3) × noexcept(2) = 24 种形态，无法用偏序覆盖，
// 必须用宏逐组展开出全部特化。
// ---------------------------------------------------------------------------
#define KAIXYHE_MEMBER_FUNCTION_TRAITS_CV_REF_NOEXCEPT(                          \
    CV_REF_QUALIFIER, NOEXCEPT_QUALIFIER, IS_NOEXCEPT_VALUE, IS_CONST_VALUE,     \
    IS_VOLATILE_VALUE, IS_LVALUE_REF_VALUE, IS_RVALUE_REF_VALUE)                 \
  template <typename Return, typename Class, typename... Args>                   \
  struct member_function_traits<Return (Class::*)(Args...)                       \
                                    CV_REF_QUALIFIER NOEXCEPT_QUALIFIER> {       \
    using return_type = Return;                                                  \
    using class_type = Class;                                                    \
    using args_tuple = tuple<Args...>;                                           \
    static constexpr std::size_t arity = sizeof...(Args);                        \
    static constexpr bool is_noexcept = IS_NOEXCEPT_VALUE;                       \
    static constexpr bool is_const = IS_CONST_VALUE;                             \
    static constexpr bool is_volatile = IS_VOLATILE_VALUE;                       \
    static constexpr bool is_lvalue_ref_qualified = IS_LVALUE_REF_VALUE;         \
    static constexpr bool is_rvalue_ref_qualified = IS_RVALUE_REF_VALUE;         \
    static constexpr bool is_variadic = false;                                   \
    template <std::size_t I>                                                     \
    using arg = tuple_element_t<I, tuple<Args...>>;                              \
  };

// CV_QUALIFIER 与 IS_CONST/IS_VOLATILE 一一对应，由最外层宏显式传入，
// 避免在模板内部用 is_convertible 反推 cv 限定（volatile 组合下不可靠）。
#define KAIXYHE_MEMBER_FUNCTION_TRAITS_CV_REF(CV_QUALIFIER, IS_CONST_VALUE,      \
                                              IS_VOLATILE_VALUE,                 \
                                              IS_LVALUE_REF_VALUE,               \
                                              IS_RVALUE_REF_VALUE)               \
  KAIXYHE_MEMBER_FUNCTION_TRAITS_CV_REF_NOEXCEPT(                                \
      CV_QUALIFIER, , false, IS_CONST_VALUE, IS_VOLATILE_VALUE,                  \
      IS_LVALUE_REF_VALUE, IS_RVALUE_REF_VALUE)                                  \
  KAIXYHE_MEMBER_FUNCTION_TRAITS_CV_REF_NOEXCEPT(                                \
      CV_QUALIFIER, noexcept, true, IS_CONST_VALUE, IS_VOLATILE_VALUE,           \
      IS_LVALUE_REF_VALUE, IS_RVALUE_REF_VALUE)

#define KAIXYHE_MEMBER_FUNCTION_TRAITS_CV(CV_QUALIFIER, IS_CONST_VALUE,          \
                                          IS_VOLATILE_VALUE)                     \
  KAIXYHE_MEMBER_FUNCTION_TRAITS_CV_REF(CV_QUALIFIER, IS_CONST_VALUE,            \
                                        IS_VOLATILE_VALUE, false, false)         \
  KAIXYHE_MEMBER_FUNCTION_TRAITS_CV_REF(CV_QUALIFIER &, IS_CONST_VALUE,          \
                                        IS_VOLATILE_VALUE, true, false)          \
  KAIXYHE_MEMBER_FUNCTION_TRAITS_CV_REF(CV_QUALIFIER &&, IS_CONST_VALUE,         \
                                        IS_VOLATILE_VALUE, false, true)

// C++17 起 noexcept 是函数类型的一部分，必须与非 noexcept 分开特化。
// 4 种 cv × 3 种 ref-qualifier × 2 种 noexcept = 24 个特化。
#define KAIXYHE_MEMBER_FUNCTION_TRAITS_ALL()                                     \
  KAIXYHE_MEMBER_FUNCTION_TRAITS_CV(, false, false)                              \
  KAIXYHE_MEMBER_FUNCTION_TRAITS_CV(const, true, false)                          \
  KAIXYHE_MEMBER_FUNCTION_TRAITS_CV(volatile, false, true)                       \
  KAIXYHE_MEMBER_FUNCTION_TRAITS_CV(const volatile, true, true)

KAIXYHE_MEMBER_FUNCTION_TRAITS_ALL()

#undef KAIXYHE_MEMBER_FUNCTION_TRAITS_ALL
#undef KAIXYHE_MEMBER_FUNCTION_TRAITS_CV
#undef KAIXYHE_MEMBER_FUNCTION_TRAITS_CV_REF
#undef KAIXYHE_MEMBER_FUNCTION_TRAITS_CV_REF_NOEXCEPT

// ---------------------------------------------------------------------------
// C 风格变参：Return (Class::*)(Args..., ...)
// ---------------------------------------------------------------------------
#define KAIXYHE_MEMBER_FUNCTION_TRAITS_VARIADIC(CV_QUALIFIER, IS_CONST_VALUE,    \
                                                IS_VOLATILE_VALUE)               \
  template <typename Return, typename Class, typename... Args>                   \
  struct member_function_traits<Return (Class::*)(Args..., ...)                  \
                                    CV_QUALIFIER> {                              \
    using return_type = Return;                                                  \
    using class_type = Class;                                                    \
    using args_tuple = tuple<Args...>;                                           \
    static constexpr std::size_t arity = sizeof...(Args);                        \
    static constexpr bool is_noexcept = false;                                   \
    static constexpr bool is_const = IS_CONST_VALUE;                             \
    static constexpr bool is_volatile = IS_VOLATILE_VALUE;                       \
    static constexpr bool is_lvalue_ref_qualified = false;                       \
    static constexpr bool is_rvalue_ref_qualified = false;                       \
    static constexpr bool is_variadic = true;                                    \
    template <std::size_t I>                                                     \
    using arg = tuple_element_t<I, tuple<Args...>>;                              \
  };

KAIXYHE_MEMBER_FUNCTION_TRAITS_VARIADIC(, false, false)
KAIXYHE_MEMBER_FUNCTION_TRAITS_VARIADIC(const, true, false)
KAIXYHE_MEMBER_FUNCTION_TRAITS_VARIADIC(volatile, false, true)
KAIXYHE_MEMBER_FUNCTION_TRAITS_VARIADIC(const volatile, true, true)

#undef KAIXYHE_MEMBER_FUNCTION_TRAITS_VARIADIC

// ---------------------------------------------------------------------------
// 便捷别名
// ---------------------------------------------------------------------------
template <typename T>
using member_function_class_t = typename member_function_traits<T>::class_type;

template <typename T>
using member_function_return_t = typename member_function_traits<T>::return_type;

template <typename T, std::size_t I>
using member_function_arg_t = typename member_function_traits<T>::template arg<I>;

template <typename T>
inline constexpr std::size_t member_function_arity_v =
    member_function_traits<T>::arity;

// ---------------------------------------------------------------------------
// 萃取实例：给定一个成员函数指针的值（编译期常量），取出所属类型
// ---------------------------------------------------------------------------
template <auto MemberPtr>
struct member_of {
    using traits = member_function_traits<std::decay_t<decltype(MemberPtr)>>;
    using class_type = typename traits::class_type;
    using return_type = typename traits::return_type;
};

template <auto MemberPtr>
using member_of_t = typename member_of<MemberPtr>::class_type;

}  // namespace kaixyhe
