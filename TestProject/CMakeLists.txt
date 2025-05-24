cmake_minimum_required(VERSION 4.0)
include(ExternalProject)
include(FetchContent)

# TODO: Maybe support cross-compiling? (compiling to an exe for windows from linux, using mingw-w64)
#if (MSVC)
#    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#endif()

set(UTCE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Engine)
set(UTCE_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/_deps/utce-build)
#if(WIN32)
#    set(UTCE_LIB_PATH ${UTCE_INSTALL_DIR}/build/Debug/UTCe.dll)
#    set(UTCE_STATIC_LIB_PATH ${UTCE_INSTALL_DIR}/build/Debug/UTCe.lib)
#else()
#    set(UTCE_LIB_PATH ${UTCE_INSTALL_DIR}/build/libUTCe.so)
#endif()

project(TestProject)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

find_package(OpenGL REQUIRED)

set(GLFW_LIBRARY_TYPE SHARED CACHE BOOL "Build shared libs" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
    # DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/glfw
    # DOWNLOAD_EXTRACT_TIMESTAMP OFF
    # SOURCE_DIR ${GLFW_INSTALL_DIR}
)

FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v2.0.8
    SOURCE_SUBDIR cmake
)

FetchContent_MakeAvailable(glfw glad)

find_package(Python3 REQUIRED COMPONENTS Interpreter)
execute_process(COMMAND ${Python3_EXECUTABLE} -m pip install --user --break-system-packages -r ${glad_SOURCE_DIR}/requirements.txt)
glad_add_library(glad SHARED REPRODUCIBLE LOADER API gl:core=4.6)

# TODO: Add project version number

add_subdirectory(${UTCE_SOURCE_DIR} ${UTCE_INSTALL_DIR})

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
add_Executable(TestProject ${SOURCE_FILES})

target_compile_definitions(UTCe PUBLIC DEBUG)
add_dependencies(TestProject UTCe)

set_target_properties(glfw PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set_target_properties(glfw PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set_target_properties(glad PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set_target_properties(glad PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set_target_properties(UTCe PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set_target_properties(UTCe PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


# TODO: Change this so we use an include folder of UTC engine as well.
# target_include_directories(TestProject PUBLIC ${glfw_SOURCE_DIR}/include)
# target_include_directories(TestProject PUBLIC ${glad_SOURCE_DIR}/include)

if(NOT WIN32)
    target_link_libraries(TestProject m)
endif()

set_target_properties(TestProject PROPERTIES LINK_FLAGS "-Wl,-rpath,$ORIGIN/lib")

target_link_libraries(TestProject UTCe glfw glad OpenGL::GL)
