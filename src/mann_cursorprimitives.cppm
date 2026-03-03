// PURPOSE:
//
// DESCRIPTION:
// 

module;

#include <unistd.h>

export module mann.cursor_primitives;

import mann.escape_sequences;

export namespace mann {

// Clear the screen and reset the cursor to upper left position.
inline auto clear_and_reset_cursor() -> void {
    // Terminal escape sequences starts with an escape char (27) followed by ']';

    // Clear the screen.
    write(STDOUT_FILENO, esc_seq_clear_screen.data(), esc_seq_clear_screen.size());

    // Position the cursor to upper left part of the screen.
    // The 'H' command takes 2 arguments, row and column. The default is 1;1 so this is equivalen to
    // "1;1H"
    write(STDOUT_FILENO, esc_seq_upper_left_cursor.data(), esc_seq_upper_left_cursor.size());
}


} // namespace mann
