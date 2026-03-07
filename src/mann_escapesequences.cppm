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

// Define escape sequences.
namespace esc {

// The H "function" takes 2 optional arguments that indicates the row and column.
// For example "\x1b[12;40H" position the cursor at line 12 and column 40.
// The default is 1;1.
// https://vt100.net/docs/vt100-ug/chapter3.html#CUP
inline constexpr std::string_view upper_left_cursor = "\x1b[H";

// Formatted string for moving the cursor. Must be used with std::format to insert the right values.
inline constexpr std::string_view move_cursor = "\x1b[{};{}H";


// Escape sequence to erase all the characters.
// 0 (default): erase from active position to the end of the screen inclusive.
// https://vt100.net/docs/vt100-ug/chapter3.html#ED 
inline constexpr std::string_view clear_screen = "\x1b[2J";


// 'l' is the reset mode command.
inline constexpr std::string_view hide_cursor = "\x1b[?25l";

// 'h' is the set mode command.
inline constexpr std::string_view show_cursor = "\x1b[?25h";

// Default argument is 0 which is erase the line at the right of the cursor.
// https://vt100.net/docs/vt100-ug/chapter3.html#EL
inline constexpr std::string_view erase_line = "\x1b[K";



} // namespace esc
} // namespace mann
