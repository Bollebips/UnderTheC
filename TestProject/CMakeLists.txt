cmake_minimum_required(VERSION 4.0)
include(ExternalProject)
include(FetchContent)

# TODO: Maybe support cross-compiling? (compiling to an exe for windows from linux, using mingw-w64)
#if (MSVC)
#    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#endif()

set(UTCE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Engine)
set(UTCE_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/_deps/utce-build)

project(TestProject)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
#
# TODO: Add project version number

FetchContent_Declare(
    UTCe
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/../Engine
)

FetchContent_MakeAvailable(UTCe)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
add_Executable(TestProject ${SOURCE_FILES})

set_target_properties(TestProject PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)

add_custom_command(
  TARGET TestProject
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources
    ${CMAKE_BINARY_DIR}/$<CONFIG>/resources
)

target_link_libraries(TestProject UTCe)
