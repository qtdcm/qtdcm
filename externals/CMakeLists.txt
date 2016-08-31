# Including CMake required module
include(ExternalProject)

string(ASCII 27 Esc)

# Including external libraries directives now
include(dcmtk.cmake)
include(itk.cmake)

message("${Esc}[1;32mExternal libraries configured.${Esc}[m")
message("")
set(CONFIGURE_EXTERNALS 0 CACHE BOOL INTERNAL FORCE)
