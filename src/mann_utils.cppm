// PURPOSE: Define some utility functions.
//
// DESCRIPTIONS:
//

export module mann.utils;

import std;

import mann.types;

namespace mann {

// Convert an ascii character to its representation with ctrl combo.
export
inline constexpr auto ctrl_key(const i8 c) -> i8 {
    // In the terminal, the ctrl key strips bit 5 and 6. 
    return c & static_cast<i8>(0b00011111);
}
    
} // namespace mann
