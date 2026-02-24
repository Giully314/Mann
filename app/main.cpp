import std;

#include <cerrno>
#include <termios.h> // terminal api
#include <unistd.h> // read, write
#include <sys/ioctl.h> // get terminal dimensions
#include <signal.h> // handle terminal events
#include <fcntl.h> // flags for non blocking I/O

[[noreturn]]
auto die(const std::string_view error) -> void {
    std::perror(error.data());
    throw std::runtime_error{"application died"};
}

// Config terminal settings.
struct TerminalSettings {
    
    // The default mode of the terminal is called cooked mode (canonical mode), where the input is sent to the 
    // terminal after the user presses the enter. We need to work in raw mode, where each byte is sent 
    // immediately.
    // Setup the console in raw mode.
    TerminalSettings() {
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
    
    // Restore the console to the original settings.
    ~TerminalSettings() {
        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_settings) == -1) {
            die("tcsetattr while destructor");
        }
    }
private:
    termios original_settings;
};


auto main() -> int {
    TerminalSettings settings;

    while (true) {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) {
            die("read");
        }

        // Check if the character is a control char (non printable).
        if (std::iscntrl(c)) {
            std::print("{:d}\r\n", c);
        } else {
            std::print("{:d} {}\r\n", c, c);
        }
        if (c == 'q') {
            break;
        }
    }
    return 0;
}