cmake_minimum_required(VERSION 4.0)
include(ExternalProject)
include(FetchContent)

project(UnderTheC_Engine)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

find_package(OpenGL REQUIRED)

set(GLFW_LIBRARY_TYPE STATIC CACHE BOOL "Build shared libs" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)

FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v2.0.8
    SOURCE_SUBDIR cmake
)

FetchContent_Declare(
    cglm
    GIT_REPOSITORY https://github.com/recp/cglm.git
    GIT_TAG        v0.9.6
)

FetchContent_MakeAvailable(glfw glad cglm)

find_package(Python3 REQUIRED COMPONENTS Interpreter)
execute_process(COMMAND ${Python3_EXECUTABLE} -m pip install --user --break-system-packages -r ${glad_SOURCE_DIR}/requirements.txt)
glad_add_library(glad SHARED REPRODUCIBLE LOADER API gl:core=4.6)

if(MSVC)
target_compile_options(cglm PRIVATE
  "$<$<CONFIG:Release>:/Ox>"
  "$<$<CONFIG:Debug>:/Od>"
)
endif()

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

add_library(UTCe SHARED ${SOURCE_FILES})

target_compile_definitions(UTCe PRIVATE UTCeEXPORT )
#TODO: This should probably be included privately, but publically
target_include_directories(UTCe PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# set_target_properties(glfw PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
# set_target_properties(glfw PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set_target_properties(glad PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set_target_properties(glad PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set_target_properties(cglm PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set_target_properties(cglm PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set_target_properties(UTCe PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set_target_properties(UTCe PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)

target_link_libraries(UTCe glfw glad cglm OpenGL::GL)

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
