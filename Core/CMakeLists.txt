cmake_minimum_required(VERSION 3.14)
include(FetchContent)

project(VoxC_Engine)

set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build/_deps CACHE PATH "" FORCE)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)

set(GLFW_LIBRARY_TYPE STATIC CACHE BOOL "Build static libs" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)

#TODO: Compile to static lib
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v2.0.8
    SOURCE_SUBDIR cmake
)

#TODO: Compile to static lib
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

add_library(VoxC SHARED ${SOURCE_FILES})

#TODO: get rid of this and find a standard define based on configuration
# or at the very least make this private?
target_compile_definitions(VoxC PUBLIC DEBUG)

target_compile_definitions(VoxC PRIVATE VOXC_EXPORT)
#TODO: REMOVE? This is only used for autocomplete to work
target_include_directories(VoxC PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/glad/gladsources/include)
#TODO: This should probably be included privately, but publically
target_include_directories(VoxC PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

set_target_properties(glad PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set_target_properties(glad PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set_target_properties(cglm PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set_target_properties(cglm PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set_target_properties(VoxC PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set_target_properties(VoxC PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)

target_link_libraries(VoxC glfw glad cglm OpenGL::GL)

# TODO: Add -pedantic-errors
