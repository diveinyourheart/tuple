# tuple

一个以 C++20 实现的轻量 tuple 工具库，包含 tuple 基础操作和成员函数指针萃取。

## 内容

- `kaixyhe::tuple`：构造、`get<I>`、比较赋值、`swap`、`make_tuple`、`tie`、
  `forward_as_tuple` 和 `apply`。
- `kaixyhe::member_function_traits`：提取成员函数的所属类、返回类型、参数类型、
  cv/ref/noexcept 限定以及变参属性。

## 构建与测试

```bash
cmake --preset debug
cmake --build --preset debug
ctest --test-dir build/debug --output-on-failure
```

也可以直接配置：

```bash
cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug --parallel
ctest --test-dir build/debug --output-on-failure
```

## 使用

```cpp
#include "kaixyhe/tuple/traits.h"
#include "kaixyhe/tuple/tuple.h"

auto values = kaixyhe::make_tuple(1, 2.5);
static_assert(kaixyhe::tuple_size_v<decltype(values)> == 2);
```

使用 CMake 时链接 `kaixyhe::tuple_traits` 即可同时获得 tuple 和萃取接口。
