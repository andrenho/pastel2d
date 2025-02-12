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
# configuration
#

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 23)

# warnings / flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(warnings -Wall -Wextra -Wformat-nonliteral -Wundef -Wshadow -Wwrite-strings -Wfloat-equal -Wswitch-default -Wmissing-format-attribute -Wswitch-enum -Wmissing-noreturn -Wno-unused-parameter -Wno-unused)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(warnings ${warnings} -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn -Wsuggest-attribute=malloc -Wsuggest-attribute=format -Wsuggest-attribute=cold)
    endif()
endif()

# try to use ccache, if available
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

#
# libraries
#

find_package(PkgConfig)
find_package(SDL2 REQUIRED)
pkg_search_module(SDL2_TTF REQUIRED SDL2_ttf)
pkg_search_module(SDL2_IMAGE REQUIRED SDL2_image)
pkg_search_module(LUA REQUIRED luajit)

add_subdirectory(contrib/embed)

#
# library
#

set(PASTEL2D_SRC
        src/graphics.hh
        src/graphics.cc
        src/scene.hh
        src/resourcemanager.hh
        src/resourcemanager.cc
        src/util/noncopyable.hh
        src/pastel2d.hh
        src/util/visitor.hh
        src/util/time.hh
        src/imagemanipulation.cc
        src/imagemanipulation.hh
        src/textcache.cc
        src/textcache.hh
)

set(IMGUI_SRC
        contrib/imgui/imgui.cpp
        contrib/imgui/imgui_demo.cpp
        contrib/imgui/imgui_draw.cpp
        contrib/imgui/imgui_tables.cpp
        contrib/imgui/imgui_widgets.cpp
        contrib/imgui/backends/imgui_impl_sdl2.cpp
        contrib/imgui/backends/imgui_impl_sdlrenderer2.cpp
        contrib/ImGuiFileDialog/ImGuiFileDialog.h
        contrib/ImGuiFileDialog/ImGuiFileDialog.cpp
        contrib/ImGuiFileDialog/ImGuiFileDialogConfig.h
)

add_library(${PROJECT_NAME} STATIC
        ${PASTEL2D_SRC}
        ${IMGUI_SRC})

# ignore warnings in imported files
set_source_files_properties(${IMGUI_SRC} PROPERTIES COMPILE_FLAGS "-w")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-ggdb -O0)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(DEF B_PRODUCTION_MODE=ON)
    add_compile_options(-Ofast -flto)
endif()

target_compile_options(${PROJECT_NAME} PRIVATE ${warnings})
target_include_directories(${PROJECT_NAME}
        SYSTEM PUBLIC
        ..
        src
            ${CMAKE_CURRENT_SOURCE_DIR}/contrib/imgui
            ${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${LUA_INCLUDE_DIRS})
target_link_directories(${PROJECT_NAME} PUBLIC ${SDL2_TTF_LIBRARY_DIRS} ${SDL2_IMAGE_LIBRARY_DIRS} ${LUA_LIBRARY_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${LUA_LIBRARIES})

b_embed(${PROJECT_NAME} ../EMPTY)  # this is just for generating the headers

#
# sample executable
#

add_executable(pastel2d-example EXCLUDE_FROM_ALL example/main.cc)
target_link_libraries(pastel2d-example PRIVATE pastel2d)

b_embed(pastel2d-example example/example.png)
b_embed(pastel2d-example example/example.tileset.lua)
b_embed(pastel2d-example example/example-shadow.tileset.lua)
b_embed(pastel2d-example example/OpenSans-Medium.ttf)