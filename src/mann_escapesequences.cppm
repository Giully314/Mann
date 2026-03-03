// PURPOSE: Define common escape sequences.
//
// DESCRIPTION:
//  This file defines some common escape sequences used across the project 
//  to facilitate the mnemonic.
//  Terminal escape sequences starts with an escape char (27) followed by '[';
//

export module mann.escape_sequences;

import std;

export namespace mann {
    
// The H "function" takes 2 optional arguments that indicates the row and column.
// For example "\x1b[12;40H" position the cursor at line 12 and column 40.
// The default is 1;1.
// https://vt100.net/docs/vt100-ug/chapter3.html#CUP
inline constexpr std::string_view esc_seq_upper_left_cursor = "\x1b[H";

// Escape sequence to erase all the characters.
// 0 (default): erase from active position to the end of the screen inclusive.
// https://vt100.net/docs/vt100-ug/chapter3.html#ED 
inline constexpr std::string_view esc_seq_clear_screen = "\x1b[2J";

} // namespace mann
