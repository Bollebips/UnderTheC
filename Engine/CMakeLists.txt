cmake_minimum_required(VERSION 3.16)

project(UnderTheC_Engine)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

add_library(UTCe SHARED ${SOURCE_FILES})

target_compile_definitions(UTCe PRIVATE UTCeEXPORT )
target_include_directories(UTCe PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# TODO: Add -pedantic-errors

# TODO: change the file structure like so, so we can export the library independant of the game project.
#UnderTheC_Engine/
#├── src/
#│   ├── internal.c
#│   └── internal.h       <-- internal-only
#├── include/
#│   └── UTCe/
#│       └── api.h        <-- public headers
#├── CMakeLists.txt
