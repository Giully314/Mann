// PURPOSE: Handle editor configuration.
//
// CLASSES:
//  EditorConfig: Configuration class for the editor.
//
// DESCRIPTION:
//  This module manages the interactions with the low level routines offered by
//  the system to set and retrieve terminal informations. For example setting 
//  raw mode, retrieve number of columns and rows.

module;

#include <cerrno>
#include <termios.h> // terminal api
#include <unistd.h> // read, write
#include <sys/ioctl.h> // get terminal dimensions
#include <signal.h> // handle terminal events
#include <fcntl.h> // flags for non blocking I/O

export module mann.config;

import std;

import mann.types;
import mann.platform_layer;
import mann.exceptions;

export namespace mann {

// Configuration class for the editor in the terminal and query basic informations like
// terminal dimensions.
struct EditorConfig {
    
    // The default mode of the terminal is called cooked mode (canonical mode), where the input is sent to the 
    // terminal after the user presses the enter. We need to work in raw mode, where each byte is sent 
    // immediately.
    // Setup the console in raw mode.
    EditorConfig() {
        enable_raw_mode();

        auto exp = get_window_size();
        if (!exp) {
            // Info error
            die(exp.error());
        }

        std::tie(screen_cols, screen_rows) = exp.value();
    }

    // Restore the console to the original settings.
    ~EditorConfig() {
        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_settings) == -1) {
            die("tcsetattr while destructor");
        }
    }    
    
    // Used only for debug.
    static
    auto instance() -> EditorConfig& {
        static EditorConfig config{};
        return config;
    }

    // Return the width in number of character.
    auto width() const noexcept -> u32 {
        return screen_cols;
    }

    // Return the height in number of character.
    auto height() const noexcept -> u32 {
        return screen_rows;
    }

private:
    auto enable_raw_mode() -> void {
        if (tcgetattr(STDIN_FILENO, &original_settings) == -1) {
            die("tcgetattr while reading original settings");
        }

        termios raw = original_settings;
        
        // Flag setting.
        // Disable some default key combinations. This implies that the bytes are not interpreted 
        // and mapped to a specific action but are just read.

        // ICRNL: \r -> \n
        // IXON: ctrl+s/q
        raw.c_iflag &= ~static_cast<tcflag_t>(ICRNL | IXON | BRKINT | INPCK | ISTRIP);

        // ECHO: Echoing the characters.
        // ICANON: canonical mode activated means read the input line by line instead of char by char.
        // ISIG: ctrl+c signal to the current process to terminate and ctrl+z signal to the current process to go in background.
        // IEXTEN: ctrl+v
        raw.c_lflag &= ~static_cast<tcflag_t>(ECHO | ICANON | ISIG | IEXTEN);

        // set the character size to 8 bits per byte.
        raw.c_cflag |= static_cast<tcflag_t>(CS8);

        // Disable \n -> \r\n conversion in output.
        raw.c_oflag &= ~static_cast<tcflag_t>(OPOST);
        
        // Set the minimum number of input bytes needed before read() can return and 
        // set the max amount of time to wait before read() returns (1/10 of seconds).
        // So after 100ms of no input bytes read() returns with code 0.
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 1;

        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
            die("tcsetattr while saving raw mode settings");
        }
    }

private:
    u32 screen_rows;
    u32 screen_cols;

    // Save the original settings to be restored at exit of the 
    // application.
    termios original_settings;
};
    
} // namespace mann
