import std;

#include <cerrno>
#include <termios.h> // terminal api
#include <unistd.h> // read, write
#include <sys/ioctl.h> // get terminal dimensions
#include <signal.h> // handle terminal events
#include <fcntl.h> // flags for non blocking I/O

import mann.config;
import mann.editor;
import mann.exceptions;


auto main() -> int {
    mann::Editor editor;

    try {
        while (true) {
            editor.run();
        }
    } catch (const mann::QuitProgramError& e) {
        return 0;
    }
    
    return 0;
}