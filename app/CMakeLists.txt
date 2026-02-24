add_executable(mann_main)
target_sources(mann_main PRIVATE main.cpp)
target_link_libraries(mann_main PRIVATE mann mann_compiler_flags)