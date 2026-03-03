// PURPOSE:
//
// DESCRIPTION:
//

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

export 
class Editor final {
public:

    auto run() -> void {
        refresh_screen();
        process_keypress();
    }

private:
    auto read_key() const -> i8 {
        i8 c{};
        auto nread = read(STDIN_FILENO, &c, 1);

        for (; nread != 1; nread = read(STDIN_FILENO, &c, 1)) {
            if (nread == -1 && errno != EAGAIN) {
                die("read");
            }
        }
        return c;
    }

    auto process_keypress() const -> void {
        switch (auto c = read_key(); c) {
        case ctrl_key('q'):
            clear_and_reset_cursor();
            // We should throw here to cleanup all the RAII.
            throw QuitProgramError{"quit"};
        }
    }

    auto draw_rows() const -> void {
        for (auto y = 0; y < config.height(); ++y) {
            write(STDOUT_FILENO, "~\r\n", 3);
        }
    }

    auto refresh_screen() const -> void {
        clear_and_reset_cursor();
        draw_rows();

        write(STDOUT_FILENO, esc_seq_upper_left_cursor.data(), esc_seq_upper_left_cursor.size());
    }


private:
    EditorConfig config;
};

} // namespace mann
