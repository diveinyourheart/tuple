#include <cassert>
#include <iostream>
#include <string>
#include <utility>

#include "kaixyhe/tuple/tuple.h"

using kaixyhe::forward_as_tuple;
using kaixyhe::get;
using kaixyhe::make_tuple;
using kaixyhe::tie;
using kaixyhe::tuple;
using kaixyhe::tuple_element_t;
using kaixyhe::tuple_size_v;

namespace {

int add3(int a, int b, int c) { return a + b + c; }

}  // namespace

int main() {
    // 构造与 get
    tuple<int, std::string, double> t(1, std::string("hello"), 2.5);
    static_assert(tuple_size_v<decltype(t)> == 3);
    static_assert(std::is_same_v<tuple_element_t<1, decltype(t)>, std::string>);

    assert(get<0>(t) == 1);
    assert(get<1>(t) == "hello");
    assert(get<2>(t) == 2.5);

    get<0>(t) = 42;
    assert(get<0>(t) == 42);

    // make_tuple / tie / forward_as_tuple
    auto t2 = kaixyhe::make_tuple(1, 2, 3);
    assert(get<2>(t2) == 3);

    int a = 0;
    std::string b;
    kaixyhe::tie(a, b) = kaixyhe::make_tuple(7, std::string("bound"));
    assert(a == 7);
    assert(b == "bound");

    auto forwarded = forward_as_tuple(1, a);
    get<1>(forwarded) = 99;
    assert(a == 99);

    // apply
    assert(kaixyhe::apply(add3, t2) == 6);

    // swap
    auto lhs = kaixyhe::make_tuple(1, std::string("a"));
    auto rhs = kaixyhe::make_tuple(2, std::string("b"));
    lhs.swap(rhs);
    assert(get<0>(lhs) == 2 && get<1>(lhs) == "b");
    assert(get<0>(rhs) == 1 && get<1>(rhs) == "a");

    std::cout << "tuple example ok\n";
    return 0;
}
