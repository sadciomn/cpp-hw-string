cmake_minimum_required(VERSION 3.16)

project(string CXX)

# Prefer C++23, fall back to C++20 (the minimum this project requires)
if("cxx_std_23" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
  set(CMAKE_CXX_STANDARD 23)
else()
  set(CMAKE_CXX_STANDARD 20)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(WITH_ASAN "Build with AddressSanitizer" OFF)

if(WITH_ASAN)
  set(SANITIZE_COMPILE_OPT -fsanitize=address -fno-omit-frame-pointer)
endif()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # GTest 1.15 trips -Wcharacter-conversion on newer clang (char8_t printing);
  # its own build promotes warnings to errors
  foreach(_t IN ITEMS gtest gtest_main gmock gmock_main)
    target_compile_options(${_t} PRIVATE -Wno-character-conversion)
  endforeach()
endif()

if(WITH_ASAN)
  # GTest objects must be instrumented consistently with the test objects
  foreach(_t IN ITEMS gtest gtest_main gmock gmock_main)
    target_compile_options(${_t} PRIVATE ${SANITIZE_COMPILE_OPT})
  endforeach()
endif()

# Implementation lives in src/*.cpp; string.hpp holds the declarations
file(GLOB STRING_IMPL_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
if(STRING_IMPL_SOURCES)
  add_library(string_impl STATIC ${STRING_IMPL_SOURCES})
  target_include_directories(string_impl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
  if(WITH_ASAN)
    target_compile_options(string_impl PRIVATE ${SANITIZE_COMPILE_OPT})
  endif()
else()
  add_library(string_impl INTERFACE)
  target_include_directories(string_impl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
endif()

include(GoogleTest)

enable_testing()

function(apply_asan_flags target)
  if(NOT WITH_ASAN)
    return()
  endif()
  target_compile_options(${target} PRIVATE ${SANITIZE_COMPILE_OPT})
  target_link_options(${target} PRIVATE -fsanitize=address)
endfunction()

function(add_string_test name source)
  add_executable(${name} ${source})
  target_link_libraries(${name} PRIVATE string_impl GTest::gtest_main GTest::gtest)
  apply_asan_flags(${name})
  if(APPLE AND WITH_ASAN)
    set(_gtest_props PROPERTIES TIMEOUT 7 ENVIRONMENT "ASAN_OPTIONS=detect_leaks=1")
  else()
    set(_gtest_props PROPERTIES TIMEOUT 7)
  endif()
  gtest_discover_tests(${name} ${_gtest_props})
endfunction()

add_string_test(test_string tests/test.cpp)

add_executable(play_string play/main.cpp)
target_link_libraries(play_string PRIVATE string_impl)
apply_asan_flags(play_string)
