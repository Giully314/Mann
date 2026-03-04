module;

#include "mann_errormacros.hpp"

#include <sys/ioctl.h>
#include <unistd.h>
#include <cstdio>

module mann.platform_layer;   

namespace {

struct PlaftormErrorCategory final : std::error_category  {
    // Name of the category
    auto name() const noexcept -> const char* override {
        return "PlaftormError";
    }

    // Message associated to an enum value.
    auto message(int condition) const -> std::string override {
        switch (static_cast<::mann::PlatformTerminalError>(condition)) {
        using enum ::mann::PlatformTerminalError;
            case WindowSize:
                return "can't get window size";
            default:
                return "unknown error";
        }
    }

    // Map the custom error to the normalized error.
    auto default_error_condition(int cond) const noexcept -> std::error_condition override {
        switch (static_cast<::mann::PlatformTerminalError>(cond)) {
            using enum ::mann::PlatformTerminalError;
            case WindowSize:
                return mann::NormalizedError::WindowSize;
            case CursorPosition:
                return mann::NormalizedError::Cursor;
            default:
                return mann::NormalizedError::Unknown;
        }
    }
};

const PlaftormErrorCategory platform_error_category;
}

CUSTOM_ERROR_CODE_DEFINITION(mann, PlatformTerminalError, platform_error_category)



namespace mann {
    
auto get_window_size() -> std::expected<std::pair<i32, i32>, InfoError> {
    winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        // Move the cursor forward by 999 and down by 999. 
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) {
            return std::unexpected<InfoError>{std::in_place, PlatformTerminalError::WindowSize}; 
        }

        auto e = get_cursor_position();
        if (!e) {
            return e;
        }

        const auto& [r, c] = e.value();
        return std::expected<std::pair<i32, i32>, InfoError>{std::in_place, c, r};
    } else {
        return std::expected<std::pair<i32, i32>, InfoError>{std::in_place, ws.ws_col, ws.ws_row};
    }
}


auto get_cursor_position() -> std::expected<std::pair<i32, i32>, InfoError> {
    // n command is device status report, used to query for terminal informations.
    // The argument 6 is used to ask for cursor position.
    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) {
        return std::unexpected<InfoError>{std::in_place, PlatformTerminalError::CursorPosition};
    }

    // The output of the n command is a string of the form
    // \x1b[row;colR
    i8 esc;
    i8 bracket;
    i8 trash;
    u32 row, col;
    std::cin >> esc >> bracket >> row >> trash >> col >> trash;

    return std::expected<std::pair<i32, i32>, InfoError>{std::in_place, row, col};
}

} // namespace mann 