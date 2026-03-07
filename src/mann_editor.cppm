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
            cx = std::max(0, cx-1);
            break;
        case 'C': // Right arrow 
            cx = std::min(cx+1, config.width() - 1);
            break;
        case 'A': // Up arrow 
            cy = std::max(0, cy-1);
            break;
        case 'B': // Down arrow
            cy = std::min(cy+1, config.height() - 1);
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
        for (auto y = 0; y < config.height() - 1; ++y) {
            buffer.append(esc::erase_line);
            buffer.append("~\r\n");
        }
        buffer.push_back('~');
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
    std::string buffer;

    // Cursor coordinates.
    i32 cx{0};
    i32 cy{0};
};

} // namespace mann
