#ifndef INCLUDED_UTILITY_MACROS_HPP
#define INCLUDED_UTILITY_MACROS_HPP

// PURPOSE: define utilities for dealing with std::expected and std::optional.
//
// MACROS:
//  EXPECTED_ASSIGN:            macro for assign a result of an expected to a variable or return the error.
//  EXPECTED_AUTO:              macro for assign a result of an expected to an auto variable or return the error.
//  EXPECTED_CHECK:             macro for checking an expected<void>.
//  EXPECTED_CHECK_OR_ELSE:     macro for checking an expected<void>, in case of error applies the specified function with the error.
//
// DESCRIPTION:
//  The macros implemented in this component are used to reduce the boilerplate of checking types like
//  std::expected.
//
// TODO:
//  -Add the possibility to return a result from check_or_else macro with the custom function.
//
// USAGE:
//
//  std::expected<int, string> test() {
//      return std::expected{10};
//  }
//
//  std::expected<int, string> test2() {
//      return std::unexpected{"ciao"};
//  }
//
//  std::expected<int, string> f() {
//      EXPECTED_AUTO(x1, test());
//      EXPECTED_ASSIGN(auto x2, test2()); // this return.
//      return x1 + x2;
//  }
//
//  int main() {
//      // here we have the unexpected string "ciao" because test2() returned error.
//      auto r = f();
//  }

#include <concepts>
#include <functional>
#include <type_traits>

// #include <libassert/assert.hpp>
#include <twinfabrica/utility/utility_concepts.hpp>

// We need double indirection to evaluate __LINE__.
#define TEMP_VAR_TOKEN(x, y) x##y
#define TEMP_VAR_TOKEN2(x, y) TEMP_VAR_TOKEN(x, y)
#define TEMP_VAR TEMP_VAR_TOKEN2(temp_var_, __LINE__)

#define EXPECTED_ASSIGN(var_name, result)                                                                           \
    auto&& TEMP_VAR = result;                                                                                       \
    DEBUG_ASSERT(::newtwen::CExpectedLike<decltype(TEMP_VAR)>, "EXPECTED_ASSIGN requires an expected like value."); \
    if (!TEMP_VAR)                                                                                                  \
        return std::unexpected{std::forward<decltype(TEMP_VAR)>(TEMP_VAR).error()};                         \
    var_name = *std::forward<decltype(TEMP_VAR)>(TEMP_VAR);

#define EXPECTED_AUTO(var_name, result) EXPECTED_ASSIGN(auto var_name, result)

#define EXPECTED_CHECK(result)                                                                                         \
    {                                                                                                                          \
        auto&& TEMP_VAR = result;                                                                                      \
        DEBUG_ASSERT(::newtwen::CExpectedLike<decltype(TEMP_VAR)>, "EXPECTED_CHECK requires an expected like value."); \
        if (!TEMP_VAR)                                                                                                 \
            return std::unexpected{std::forward<decltype(TEMP_VAR)>(TEMP_VAR).error()};                        \
    }

#define EXPECTED_CHECK_OR_ELSE(result, ferr)                                                                                   \
    {                                                                                                                                  \
        auto&& TEMP_VAR = result;                                                                                              \
        DEBUG_ASSERT(::newtwen::CExpectedLike<decltype(TEMP_VAR)>, "EXPECTED_CHECK_OR_ELSE requires an expected like value."); \
        if (!TEMP_VAR)                                                                                                         \
            std::invoke(ferr, std::forward<decltype(TEMP_VAR)>(TEMP_VAR).error());                                     \
    }

#endif
