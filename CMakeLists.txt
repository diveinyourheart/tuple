cmake_minimum_required(VERSION 3.20)

project(
  tuple
  VERSION 0.1.0
  DESCRIPTION "C++20 tuple and member-function traits"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

function(tuple_warnings target)
  if(MSVC)
    target_compile_options(${target} PRIVATE /W4 /permissive- /utf-8)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endfunction()

function(tuple_enable_assertions target)
  if(MSVC)
    target_compile_options(${target} PRIVATE /UNDEBUG)
  else()
    target_compile_options(${target} PRIVATE -UNDEBUG)
  endif()
endfunction()

add_library(tuple INTERFACE)
add_library(kaixyhe::tuple ALIAS tuple)
target_compile_features(tuple INTERFACE cxx_std_20)
target_include_directories(
  tuple
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

add_library(tuple_traits INTERFACE)
add_library(kaixyhe::tuple_traits ALIAS tuple_traits)
target_compile_features(tuple_traits INTERFACE cxx_std_20)
target_include_directories(
  tuple_traits
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
target_link_libraries(tuple_traits INTERFACE kaixyhe::tuple)

add_executable(tuple_example examples/tuple_example.cc)
target_link_libraries(tuple_example PRIVATE kaixyhe::tuple)
tuple_warnings(tuple_example)
tuple_enable_assertions(tuple_example)

add_executable(tuple_traits_example examples/traits_example.cc)
target_link_libraries(tuple_traits_example PRIVATE kaixyhe::tuple_traits)
tuple_warnings(tuple_traits_example)
tuple_enable_assertions(tuple_traits_example)

include(CTest)

if(BUILD_TESTING)
  add_test(NAME tuple_example COMMAND tuple_example)
  add_test(NAME tuple_traits_example COMMAND tuple_traits_example)
endif()
