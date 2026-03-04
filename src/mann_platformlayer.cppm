// PURPOSE: Implement platform utilities.
//
// DESCRIPTION:
//

module;

#include "mann_errormacros.hpp"

export module mann.platform_layer;

import std;

import mann.types;
import mann.exceptions;

export namespace mann {

enum class PlatformTerminalError {
    // Error while trying to get window size.
    WindowSize=1,
    CursorPosition,
};


// Return a pair where the first element is the width, the second element the height.
// In case of an error, return InfoError with the error condition.
auto get_window_size() -> std::expected<std::pair<i32, i32>, InfoError>;

// Return a pair where the first element is the row, the second element the col.
// In case of an error, return InfoError with the error condition.
auto get_cursor_position() -> std::expected<std::pair<i32, i32>, InfoError>;
    
} // namespace mann

CUSTOM_ERROR_CODE_DECLARATION(mann, PlatformTerminalError)

