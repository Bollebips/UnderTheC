cmake_minimum_required(VERSION 3.16)

# TODO: Maybe support cross-compiling? (compiling to an exe for windows from linux, using mingw-w64)
#if (MSVC)
#    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#endif()

set(UTCE_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Engine)
set(UTCE_LIB_PATH ${UTCE_INSTALL_DIR}/build/libUTCe.so)

set(GLEW_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/glew)
set(GLEW_LIB_PATH ${GLEW_INSTALL_DIR}/lib/libGLEW.so)

set(GLFW_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/glfw)
set(GLFW_LIB_PATH ${GLFW_INSTALL_DIR}/lib/libglfw.so)

# exclude unnecessary folders from glfw (samples, tests, docs, ...)
#set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
#add_subdirectory("3rdParty/glfw")

# Configure GLEW before building
#set(glew-cmake_BUILD_SHARED ON CACHE BOOL "" FORCE)
#set(glew-cmake_BUILD_STATIC OFF CACHE BOOL "" FORCE)
#set(glew-cmake_BUILD_REGAL OFF CACHE BOOL "" FORCE)
#add_subdirectory(3rdParty/glew)

#ExternalProject_Add(glew_external SOURCE_DIR ${CMAKE_SOURCE_DIR}/3rdParty/glew/build/cmake)
#    CMAKE_ARGS 
#        -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/glew-install
#	-Dglew-cmake_BUILD_STATIC=OFFlib
#	-Dglew-cmake_BUILD_SHARED=ON
#        -Dglew-cmake_BUILD_REGAL=OFF
#        -Dglew-cmake_BUILD_GLX=ON
#        -Dglew-cmake_BUILD_EGL=OFF
#        -Dglew-cmake_BUILD_OSMESA=OFF
#)

#add_subdirectory(../Engine/src)

project(TestProject)

# TODO: Set C standard

# we define MY_SOURCES to be a list of all the source files for my project
#file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")


include(ExternalProject)
#find_package(GLEW REQUIRED)
#include_directories(${GLEW_INCLUDE_DIRS})
#link_libraries(${GLEW_LIBRARIES})

#TODO: Only do this on linux
ExternalProject_Add(
    glew
    PREFIX glew
    GIT_REPOSITORY https://github.com/nigels-com/glew.git
    GIT_TAG        master 
    UPDATE_COMMAND ""  # Avoid updating every time
    CONFIGURE_COMMAND 
    ${CMAKE_COMMAND} -E chdir <SOURCE_DIR>/auto make && # run make in auto/ to generate include files
    ${CMAKE_COMMAND} -S <SOURCE_DIR>/build/cmake -B <BINARY_DIR> -DCMAKE_INSTALL_PREFIX=${GLEW_INSTALL_DIR}
    #BUILD_ALWAYS 1 (Might want to enable this again if 3rdparty folder is removed.)
)

ExternalProject_Add(
    glfw
    PREFIX glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4  # You can pin to a specific tag like 'glew-2.2.0'
    UPDATE_COMMAND ""  # Avoid updating every time
    CONFIGURE_COMMAND 
    ${CMAKE_COMMAND} -S <SOURCE_DIR> -B <BINARY_DIR> -DCMAKE_INSTALL_PREFIX=${GLFW_INSTALL_DIR} -DBUILD_SHARED_LIBS=ON
    #BUILD_ALWAYS 1 (Might want to enable this again if 3rdparty folder is removed.)
)
# Define the directories where the GLEW headers and libraries will be located
#ExternalProject_Get_Property(glew source_dir binary_dir)

# Specify where to copy the GLEW headers and libraries
#set(GLEW_INCLUDE_DIR ${binary_dir}/include)
#set(GLEW_LIBRARY_DIR ${binary_dir}/lib)

# Optionally, link the GLEW library in your project
#add_library(glew_external STATIC IMPORTED)
#set_target_properties(glew_external PROPERTIES
#    IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/lib/libGLEW.a  # Adjust as needed (e.g., libGLEW.so)
#    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_BINARY_cmake DIR}/include/glew
#)

# TODO: Add project version number

# Define an imported shared library target for GLEW
add_library(glew_shared SHARED IMPORTED GLOBAL)
file(MAKE_DIRECTORY ${GLEW_INSTALL_DIR}/include)
set_target_properties(glew_shared PROPERTIES
    IMPORTED_LOCATION ${GLEW_LIB_PATH}
    INTERFACE_INCLUDE_DIRECTORIES ${GLEW_INSTALL_DIR}/include
)

add_library(glfw_shared SHARED IMPORTED GLOBAL)
set_target_properties(glfw_shared PROPERTIES
	IMPORTED_LOCATION ${GLFW_LIB_PATH}
	INTERFACE_INCLUDE_DIRECTORIES ${GLFW_INSTALL_DIR}/include
)

add_dependencies(glew_shared glew)
add_dependencies(glfw_shared glfw)

find_package(OpenGL REQUIRED)


add_Executable(TestProject src/main.c)

add_subdirectory(${UTCE_INSTALL_DIR} UTC_Engine)
#set(CMAKE_BUILD_TYPE DEBUG)
target_compile_definitions(UTCe PUBLIC DEBUG)

# TODO: Change this so we use an include folder instead of src
target_include_directories(TestProject PUBLIC ${UTCE_INSTALL_DIR}/src)
target_include_directories(TestProject PUBLIC ${GLEW_INSTALL_DIR}/include)
target_include_directories(TestProject PUBLIC ${GLFW_INSTALL_DIR}/include)
target_link_libraries(TestProject PUBLIC UTCe glew_shared glfw_shared OpenGL::GL)
