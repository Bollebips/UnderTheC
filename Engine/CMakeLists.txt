cmake_minimum_required(VERSION 3.16)

project(UnderTheC_Engine)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
add_library(UTCe SHARED ${SOURCE_FILES})

target_include_directories(UTCe PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# TODO: change the file structure like so, so we can export the library independant of the game project.
#UnderTheC_Engine/
#├── src/
#│   ├── internal.c
#│   └── internal.h       <-- internal-only
#├── include/
#│   └── UTCe/
#│       └── api.h        <-- public headers
#├── CMakeLists.txt
