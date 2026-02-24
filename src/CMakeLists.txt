add_library(mann)
target_sources(mann 
    PUBLIC FILE_SET CXX_MODULES FILES 
    mann.cppm)
target_link_libraries(mann PRIVATE mann_compiler_flags)