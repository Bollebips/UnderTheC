cmake_minimum_required(VERSION 3.14)
include(FetchContent)

# TODO: Maybe support cross-compiling? (compiling to an exe for windows from linux, using mingw-w64)
# TODO: Maybe statically link runtime lib?
#if (MSVC)
#    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#endif()

project(VoxC-Demo)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
#
# TODO: Add project version number

FetchContent_Declare(
    VoxC
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/../Core
)

FetchContent_MakeAvailable(VoxC)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
add_Executable(VoxC-Demo ${SOURCE_FILES})

set_target_properties(VoxC-Demo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)

add_custom_command(
  TARGET VoxC-Demo
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources
    ${CMAKE_BINARY_DIR}/$<CONFIG>/resources
)

target_link_libraries(VoxC-Demo VoxC)
