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

enum class PlatformConsoleError {
    // Error while trying to get window size.
    WindowSize=1,
};


// Return a pair where the first element is the width, the second element the height.
// Can throw an exception if ioctl returns errors.
auto get_window_size() -> std::expected<std::pair<i32, i32>, InfoError>;
    
} // namespace mann

CUSTOM_ERROR_CODE_DECLARATION(mann, PlatformConsoleError)

