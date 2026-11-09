#include <cassert>
#include <iostream>
#include <string>
#include <type_traits>

#include "kaixyhe/tuple/traits.h"

using kaixyhe::member_function_traits;
using kaixyhe::member_of_t;

namespace {

struct Base {
    virtual int base_method(double) { return 0; }
    int base_only_method(int) { return 0; }
    virtual ~Base() = default;
};

struct Widget : Base {
    int plain(int, const std::string&) { return 0; }
    double const_method() const { return 0.0; }
    void volatile_method() volatile {}
    int ref_method() & { return 0; }
    int rvalue_method() && { return 0; }
    int noexcept_method() noexcept { return 0; }
    int const_noexcept_method() const noexcept { return 0; }
    long variadic_method(int, ...) { return 0; }
    static void static_method() {}
    int data_member = 0;
    int base_method(double) override { return 1; }
};

// 静态成员函数指针是普通函数指针，数据成员指针也不是成员函数指针，
// 主模板未定义，因此下面这个检测应当为 false（SFINAE 友好）。
template <auto Ptr>
constexpr bool has_member_function_traits_impl(...) {
    return false;
}

template <auto Ptr, typename = typename kaixyhe::member_function_traits<
                        std::decay_t<decltype(Ptr)>>::class_type>
constexpr bool has_member_function_traits_impl(int) {
    return true;
}

template <auto Ptr>
constexpr bool has_member_function_traits() {
    return has_member_function_traits_impl<Ptr>(0);
}

template <auto Ptr>
void template_has_no_traits() {
    static_assert(!has_member_function_traits<Ptr>());
}

}  // namespace

int main() {
    // ---- 取出所属类型：核心需求 ----
    using plain_traits = member_function_traits<decltype(&Widget::plain)>;
    static_assert(std::is_same_v<plain_traits::class_type, Widget>);
    static_assert(std::is_same_v<member_of_t<&Widget::plain>, Widget>);

    // 未被覆盖的继承成员函数，所属类型是声明它的 Base
    static_assert(std::is_same_v<member_of_t<&Widget::base_only_method>, Base>);
    // 被 Widget 覆盖后，&Widget::base_method 的类型是 int (Widget::*)(double)
    static_assert(std::is_same_v<member_of_t<&Widget::base_method>, Widget>);

    // ---- 返回类型 ----
    static_assert(std::is_same_v<plain_traits::return_type, int>);
    static_assert(std::is_same_v<
                  member_function_traits<decltype(&Widget::const_method)>::return_type,
                  double>);

    // ---- 参数个数与逐个参数类型 ----
    static_assert(plain_traits::arity == 2);
    static_assert(std::is_same_v<plain_traits::arg<0>, int>);
    static_assert(std::is_same_v<plain_traits::arg<1>, const std::string&>);

    // ---- cv / 引用限定 / noexcept 变体都能匹配 ----
    static_assert(std::is_same_v<member_of_t<&Widget::const_method>, Widget>);
    static_assert(std::is_same_v<member_of_t<&Widget::volatile_method>, Widget>);
    static_assert(std::is_same_v<member_of_t<&Widget::ref_method>, Widget>);
    static_assert(std::is_same_v<member_of_t<&Widget::rvalue_method>, Widget>);
    static_assert(std::is_same_v<member_of_t<&Widget::noexcept_method>, Widget>);
    static_assert(std::is_same_v<member_of_t<&Widget::const_noexcept_method>, Widget>);

    static_assert(member_function_traits<decltype(&Widget::noexcept_method)>::is_noexcept);
    static_assert(!member_function_traits<decltype(&Widget::plain)>::is_noexcept);
    static_assert(member_function_traits<decltype(&Widget::const_method)>::is_const);
    static_assert(!member_function_traits<decltype(&Widget::plain)>::is_const);
    static_assert(
        member_function_traits<decltype(&Widget::volatile_method)>::is_volatile);
    static_assert(
        member_function_traits<decltype(&Widget::ref_method)>::is_lvalue_ref_qualified);
    static_assert(
        member_function_traits<decltype(&Widget::rvalue_method)>::is_rvalue_ref_qualified);
    static_assert(!member_function_traits<decltype(&Widget::plain)>::is_variadic);

    // ---- 变参 ----
    using variadic_traits = member_function_traits<decltype(&Widget::variadic_method)>;
    static_assert(std::is_same_v<variadic_traits::class_type, Widget>);
    static_assert(variadic_traits::is_variadic);
    static_assert(variadic_traits::arity == 1);
    static_assert(std::is_same_v<variadic_traits::arg<0>, int>);

    // ---- 静态成员函数不是成员函数指针，萃取应失败（SFINAE 友好） ----
    template_has_no_traits<&Widget::static_method>();
    template_has_no_traits<&Widget::data_member>();

    std::cout << "traits example ok\n";
    std::cout << "  &Widget::plain           -> class_type = Widget\n";
    std::cout << "  &Widget::base_only_method-> class_type = Base (declared in base)\n";
    std::cout << "  &Widget::base_method     -> class_type = Widget (overridden)\n";
    return 0;
}
