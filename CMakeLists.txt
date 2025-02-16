#
# project definitions
#

cmake_minimum_required(VERSION 3.28)

project(pastel2d
        VERSION 1.0.0
        DESCRIPTION "A 2D thin static game library built on top of SDL2, Lua and ImGui."
        LANGUAGES C
)

#
# configuration
#

set(CMAKE_C_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# warnings / flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(warnings -Wall -Wextra -Wformat-nonliteral -Wshadow -Wwrite-strings -Wmissing-format-attribute -Wswitch-enum -Wmissing-noreturn -Wno-unused-parameter -Wno-unused)
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set(warnings ${warnings} -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn -Wsuggest-attribute=malloc -Wsuggest-attribute=format -Wsuggest-attribute=cold)
    endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-ggdb -O0)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(DEF B_PRODUCTION_MODE=ON)
    add_compile_options(-Ofast -flto)
endif()


#
# external libraries
#

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
set(LUAJIT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/contrib/LuaJIT)
add_subdirectory(contrib/luajit-cmake)

#
# function to generate embedded file
#

function(embed target source)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated)

    string(REPLACE "/" "_" TARGET_NAME "${source}")
    string(REPLACE "." "_" TARGET_NAME "${TARGET_NAME}")

    get_filename_component(FILENAME "${source}" NAME)

    add_custom_target(${TARGET_NAME}
            COMMAND ${PROJECT_SOURCE_DIR}/bin/genheader ${source} ${ARGV2} > ${CMAKE_CURRENT_BINARY_DIR}/generated/${FILENAME}.h
            DEPENDS ${PROJECT_SOURCE_DIR}/bin/genheader ${PROJECT_SOURCE_DIR}/${source}
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            COMMENT "Generating header file for ${source}..."
    )

    add_dependencies(${target} ${TARGET_NAME})
endfunction()


#
# C library
#

add_library(${PROJECT_NAME} STATIC
        src/graphics.h
        src/graphics.c
        src/pastel2d.h
        src/pastel2d.c
        src/error.c
        src/error.h
        src/res.c
        src/res.h
        src/scene.c
        src/scene.h
        src/context.h
        src/context.c
        src/manip.c
        src/manip.h
        src/textcache.c
        src/textcache.h
        src/audio.c
        src/audio.h
        src/passert.h
        src/passert.c
)
target_include_directories(${PROJECT_NAME}
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/src
            ${CMAKE_CURRENT_BINARY_DIR}/generated
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/contrib/stb
            ${CMAKE_CURRENT_SOURCE_DIR}/contrib/pocketmod
)
target_link_libraries(${PROJECT_NAME}
        PRIVATE
            luajit::lib luajit::header
        PUBLIC
            SDL3::SDL3
            m
)
target_compile_options(${PROJECT_NAME}
        PRIVATE ${warnings})

#
# sample executable
#

add_executable(pastel2d-example EXCLUDE_FROM_ALL
        example/example.c
)
target_compile_options(pastel2d-example
        PRIVATE ${warnings})
target_link_libraries(pastel2d-example PRIVATE pastel2d)

embed(pastel2d-example example/example.png)
embed(pastel2d-example example/example.tileset.lua lua-strip)
embed(pastel2d-example example/example-shadow.tileset.lua lua-strip)
embed(pastel2d-example example/OpenSans-Medium.ttf)
embed(pastel2d-example example/Born2bSportyFS.otf)
embed(pastel2d-example example/nemesis.mod)
embed(pastel2d-example example/shotgun.wav)

# check for leaks

add_custom_target(leaks)
add_custom_command(TARGET leaks
        POST_BUILD
        COMMENT "Check for leaks using valgrind."
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS pastel2d-example
        COMMAND valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --suppressions=${CMAKE_SOURCE_DIR}/valgrind.supp ./pastel2d-example
)
