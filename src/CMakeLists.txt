add_library(mann)

# these should not be all public.
# Implementation files of low level stuff should be private.
target_sources(mann 
    PUBLIC FILE_SET CXX_MODULES FILES 
    mann_config.cppm 
    mann_cursorprimitives.cppm
    mann_escapesequences.cppm
    mann_exceptions.cppm 
    mann_platformlayer.cppm
    mann_types.cppm
    mann_utils.cppm
    mann_editor.cppm
    mann.cppm 
)

target_sources(mann
    PRIVATE mann_platformlayer.cpp mann_exceptions.cpp)
 
target_link_libraries(mann PRIVATE mann_compiler_flags)