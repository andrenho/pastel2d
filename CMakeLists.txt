#
# project definitions
#

cmake_minimum_required(VERSION 3.28)

project(pastel2d
        VERSION 1.0.0
        DESCRIPTION "A 2D thin static game library built on top of SDL2, Lua and ImGui."
        LANGUAGES C CXX
)

#
# external libraries
#

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
add_subdirectory(contrib/glfw)

#
# C library
#

add_library(${PROJECT_NAME} STATIC
        src/graphics.h
        src/graphics.c
        src/pastel2d.hh
)
target_include_directories(${PROJECT_NAME} PUBLIC src)

#
# sample executable
#

add_executable(pastel2d-example EXCLUDE_FROM_ALL example/example.c)
target_link_libraries(pastel2d-example PRIVATE pastel2d glfw OpenGL::GL)