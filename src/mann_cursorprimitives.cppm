// PURPOSE: Define cursor action primitives.
//
// DESCRIPTION:
//  This module defines some basic actions used as primitives in the library.
//  An action can be defined as one or more escape sequences; for just one escape 
//  sequence, there is no need of abstraction in a function, we can use directly
//  the mnemonics defined in mann.escape_sequences. 


module;

#include <unistd.h>

export module mann.cursor_primitives;

import std;

import mann.escape_sequences;

export namespace mann {

// Clear the screen and reset the cursor to upper left position.
inline auto clear_and_reset_cursor() -> void {
    write(STDOUT_FILENO, esc::clear_screen.data(), esc::clear_screen.size());
    write(STDOUT_FILENO, esc::upper_left_cursor.data(), esc::upper_left_cursor.size());
}


// Append to buffer the escape sequences for clear and reset cursor.
inline auto clear_and_reset_cursor(std::string& buffer) -> void {
    buffer.append(esc::clear_screen);
    buffer.append(esc::upper_left_cursor);
}

} // namespace mann
