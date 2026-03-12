import std;

import mann.config;
import mann.editor;
import mann.exceptions;


auto main(int argc, const char** argv) -> int {
    std::unique_ptr<mann::Editor> editor;
    
    try {
        if (argc == 2) {
            editor = std::make_unique<mann::Editor>(argv[1]);
        } else {
            editor = std::make_unique<mann::Editor>();
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::println("{}", e.what());
        return 0;
    }
    
    try {
        while (true) {
            editor->run();
        }
    } catch (const mann::QuitProgramError& e) {
        return 0;
    }
    
    return 0;
}