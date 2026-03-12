// PURPOSE: Handle the logic of the editor.
//
// CLASSES:
//  Editor: Top level class which handles the logic of the editor.
// DESCRIPTION:
//  
// 
// TODO:
//  Right now there are some functionalities that I think should be moved 
//  to a lower layer in the architecture, for example the primitive of refresh
//  screen and drawing.

module;

#include <unistd.h>
#include <cerrno>

export module mann.editor;

import std;

import mann.config;
import mann.types;
import mann.exceptions;
import mann.utils;
import mann.cursor_primitives;
import mann.escape_sequences;

namespace mann {

enum class KeyType : u8 {
    // The keycode must be interpreted as a part of an escape sequence.
    EscapeSequence,
    // The keycode represent a key pressed by the user.
    Plain,
};


struct Key {
    const i8 key;
    const KeyType type;

    static constexpr auto plain(const i8 c) -> Key {
        return Key{c, KeyType::Plain};
    }

    static constexpr auto esc_seq(const i8 c) -> Key {
        return Key{c, KeyType::EscapeSequence};
    }
}; 

export 
class Editor final {
public:

    Editor() = default;

    Editor(const std::filesystem::path& path) : rows{read_file(path)} { }


    auto run() -> void {
        refresh_screen();
        process_keypress();
    }

private:
    auto read_key() const -> Key {
        i8 c{};
        auto nread = read(STDIN_FILENO, &c, 1);

        for (; nread != 1; nread = read(STDIN_FILENO, &c, 1)) {
            if (nread == -1 && errno != EAGAIN) {
                die("read");
            }
        }
        
        // Check if the current character is the start of an escape sequence
        // to be processed by the editor "\x1b[".
        if (c == '\x1b') {
            i8 seq[3];

            if (read(STDIN_FILENO, &seq[0], 1) != 1 ) {
                return Key::plain('\x1b');
            }
            if (read(STDIN_FILENO, &seq[1], 1) != 1) {
                return Key::plain('\x1b');
            }
            
            if (seq[0] == '[') {    
                // This piece of code lose information if done in this way. 
                // There are 2 options (that comes in my mind right now):
                // Return the seq[1:2] to be then interpreted by process_escape_sequence
                // or assume that we can do some 'initial parsing' without lose of information
                // like reading '~' and return just the number. But what is there are other commands 
                // with the same numbers? 
                // The following code has some useless checks that can be simplified when a decision is made.
                // Another thing that can be made, is to just remap the key to something else in the range of an ascii code;
                // because we have the additional information if it is an escape seq or not, there is no conflict during dispatch.

                // Check for page up/down.
                if (seq[1] >= '0' && seq[1] <= '9') {
                    if (read(STDIN_FILENO, &seq[2], 1) != 1) {
                        return Key::plain(c);
                    }
                    
                    // It is a page up/down.
                    if (seq[2] == '~') {
                        return Key::esc_seq(seq[1]);
                    }
                }
                return Key::esc_seq(seq[1]);
            }
        }

        return Key::plain(c);
    }

    auto process_keypress() -> void {
        auto c = read_key();

        if (c.type == KeyType::EscapeSequence) {
            process_escape_sequence(c.key);    
        } else if (c.type == KeyType::Plain) {
            process_plain(c.key);
        }
    }

    auto process_plain(const i8 c) -> void {
        switch (c) {
        case ctrl_key('q'):
            // We should throw here to cleanup all with RAII.
            clear_and_reset_cursor();
            throw QuitProgramError{"quit"};
        }
    }

    auto process_escape_sequence(const i8 c) -> void {
        switch (c) {

        case 'D': // Left arrow 
            if (cx > 0) {
                --cx;
            } else {
                if (col_offset > 0) {
                    --col_offset;
                }
            }
            break;
        case 'C': // Right arrow 
            if (cx < config.width() - 1) {
                ++cx;
            } else {
                // Here we should check on maximum len of the rows.
                ++col_offset;   
            }
            break;
        case 'A': // Up arrow 
            if (cy > 0) {
                --cy;
            } else {
                if (row_offset > 0) {
                    --row_offset;
                }
            }
            break;
        case 'B': // Down arrow
            if (cy < config.height() - 1) {
                ++cy;
            } else {
                // Technically this should be rows.size() - config.height() but this 
                // should be done by first checking if rows.size() > config.height().
                if (row_offset < rows.size()) {
                    ++row_offset;
                }
            }
            break;
        
        // Home key
        case '1':
        case '7':
        case 'H':
            cx = 0;
            break;

        // Delete key
        case '3':
            break;

        // End key
        case '4':
        case '8':
        case 'F':
            cx = config.width() - 1;
            break;
            

        // Temporary, right now the screen is fixed, so we just move the cursor at top or bottom of the current window.
        case '5': // Page up
            cy = 0;
            break;
        case '6': // Page down
            cy = config.height() - 1;
            break;
        }
    }

    auto draw_rows() -> void {
        for (u32 y = 0; y < config.height(); ++y) {
            const auto filerow = y + row_offset;
            if (filerow >= rows.size()) {
                buffer.append("~");
            } else {
                u32 len = static_cast<u32>(rows[filerow].size());
                if (len > config.width()) {
                    len = config.width();
                }
                const auto start = std::min(col_offset, len);
                buffer.append(rows[filerow], start, len);
            }
            
            buffer.append(esc::erase_line);
            if (y < config.height() - 1) {
                buffer.append("\r\n");
            }
        }
    }

    auto refresh_screen() -> void {
        buffer.append(esc::hide_cursor);
        buffer.append(esc::upper_left_cursor);

        draw_rows();
        
        buffer.append(std::format(esc::move_cursor, cy+1, cx+1));
        buffer.append(esc::show_cursor);

        write(STDOUT_FILENO, buffer.data(), buffer.size());
        buffer.clear();
        // TODO: Should we also resize the buffer to 0? 
    }


private:
    EditorConfig config;

    // Data content to be displayed at each frame.
    std::string buffer;

    // A rows of text used for tests.
    std::vector<std::string> rows{"Hello world."};

    // Cursor coordinates.
    u32 cx{0};
    u32 cy{0};

    // Used to keep track of the vertical scrolling.
    u32 row_offset{0};

    // Used to keep track of horizontal scrolling.
    u32 col_offset{0};
};

} // namespace mann
