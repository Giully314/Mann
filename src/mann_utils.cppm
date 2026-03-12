// PURPOSE: Define some utility functions.
//
// DESCRIPTIONS:
//

export module mann.utils;

import std;


import mann.types;

namespace mann {

// Convert an ascii character to its representation with ctrl combo.
export
inline constexpr auto ctrl_key(const i8 c) -> i8 {
    // In the terminal, the ctrl key strips bit 5 and 6. 
    return c & static_cast<i8>(0b00011111);
}


// I wanted to try std::generator, but clang doesn't implement it :) (c++23 btw).
// export 
// auto read_file(const std::filesystem::path& path) -> std::generator<std::string> {
//     std::ifstream istream{path, std::ios_base::in};
    
//     if (!istream.is_open()) {
//         throw std::filesystem::filesystem_error{"Can't open file"};
//     }

//     std::string line;
    
//     while (std::getline(istream, line, '\n')) {
//         co_yield line;
//     }
// }

export 
auto read_file(const std::filesystem::path& path) -> std::vector<std::string> {
    std::ifstream istream{path, std::ios_base::in};
    
    if (!istream.is_open()) {
        throw std::filesystem::filesystem_error{"Can't open file", path, std::make_error_code(std::errc::no_such_file_or_directory)};
    }

    std::string line;
    std::vector<std::string> lines;
    while (std::getline(istream, line, '\n')) {
        lines.push_back(std::move(line));
    }

    return lines;
}
    
} // namespace mann
