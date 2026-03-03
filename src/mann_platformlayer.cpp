module;

#include "mann_errormacros.hpp"

#include <sys/ioctl.h>
#include <unistd.h>

module mann.platform_layer;   

namespace {

struct PlaftormErrorCategory final : std::error_category  {
    // Name of the category
    auto name() const noexcept -> const char* override {
        return "PlaftormError";
    }

    // Message associated to an enum value.
    auto message(int condition) const -> std::string override {
        switch (static_cast<::mann::PlatformConsoleError>(condition)) {
        using enum ::mann::PlatformConsoleError;
            case WindowSize:
                return "can't get window size";
            default:
                return "unknown error";
        }
    }

    // Map the custom error to the normalized error.
    auto default_error_condition(int cond) const noexcept -> std::error_condition override {
        switch (static_cast<::mann::PlatformConsoleError>(cond)) {
            using enum ::mann::PlatformConsoleError;
            case WindowSize:
                return mann::NormalizedError::WindowSize;
            default:
                return mann::NormalizedError::Unknown;
        }
    }
};

const PlaftormErrorCategory platform_error_category;
}

CUSTOM_ERROR_CODE_DEFINITION(mann, PlatformConsoleError, platform_error_category)



namespace mann
{
    
auto get_window_size() -> std::expected<std::pair<i32, i32>, InfoError> {
    winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        return std::unexpected<InfoError>{std::in_place, PlatformConsoleError::WindowSize}; 
    } else {
        return std::pair{ws.ws_col, ws.ws_row};
    }
}

} // namespace mann 