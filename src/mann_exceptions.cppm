// PURPOSE: Define error_code and exceptions.
//
// CLASSES:
//  InfoError: Data to track an error.
// 
// DESCRIPTION:
//  This module defines exceptions and errors that could arises in the application.
//  Note that error_code are not mutually exclusives with exceptions; they are used as a
//  low level interfaces with C errors like returned from low level OS fuctions.
//
module;

#include <cerrno>
#include <cstdio>

export module mann.exceptions;

import std;

import mann.types;
import mann.cursor_primitives;

export namespace mann {

enum class NormalizedError {
// no 0.
InputInvalid = 1,
WindowSize,
Unknown,
IllegalAction,
};

std::error_condition make_error_condition(NormalizedError e);

// InfoError is a data container for error tracking in the application.
struct [[nodiscard]] InfoError final {
    explicit InfoError(const std::error_code& e) : code(e) { }

    explicit InfoError(const std::error_code& e, const std::string_view descr) : code(e), description(descr) { }

    std::error_code code;

    // For internal debug purposes.
    std::string description;

    std::source_location location{std::source_location::current()};

    // I found out that in 2026, latest version of clang STILL doesn't support stacktrace. 
    // I can't write what i'm thinking right now.
    // std::stacktrace trace{std::stacktrace::current()};
};

// Fatal error, signal the error and throw an exception to unwind the stack
// and clean the resources.
[[noreturn]]
auto die(const std::string_view error) -> void {
    clear_and_reset_cursor();

    std::perror(error.data());
    throw std::runtime_error{"application died"};
}

[[noreturn]]
auto die(const InfoError& e) -> void {
    clear_and_reset_cursor();

    // std::print(stderr, "{}", e);
    std::perror(e.description.c_str());
    throw std::runtime_error{"application died"};
}


class QuitProgramError : std::runtime_error {
public:
    template <typename StringLike>
        requires std::convertible_to<StringLike, std::string_view>
    QuitProgramError(StringLike&& msg) : std::runtime_error{std::forward<StringLike>(msg)} {}
};

} // namespace mann



namespace std {
template <>
struct is_error_condition_enum<::mann::NormalizedError> : true_type {};
}  // namespace std


// Custom formatter for InfoError used by <format> and <print>.
export
template <>
struct std::formatter<::mann::InfoError> final : std::formatter<std::string_view> {
    auto format(const ::mann::InfoError& e, std::format_context& ctx) const {
        return std::formatter<std::string_view>::format(std::format("{} {} {}", e.code.category().name(), e.code.message(), e.description), ctx);
    }
};
