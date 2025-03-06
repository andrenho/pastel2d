#
# project definitions
#

cmake_minimum_required(VERSION 3.28)

project(pastel2d
        VERSION 0.1.0
        DESCRIPTION "A 2D thin static game library built on top of SDL2, Lua and ImGui."
        LANGUAGES C CXX
)

#
# configuration
#

include(ExternalProject)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fms-extensions")

# warnings / flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(warnings -Wall -Wextra -Wformat-nonliteral -Wshadow -Wwrite-strings -Wmissing-format-attribute -Wswitch -Wmissing-noreturn -Wno-unused-parameter -Wno-unused)
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

add_compile_definitions(
        PROJECT_VERSION=\"${PROJECT_VERSION}\"
        PROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
        PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR}
        PROJECT_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

#
# external libraries
#

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)

# LuaJIT

if (APPLE)
    execute_process(
            COMMAND sw_vers -productVersion
            OUTPUT_VARIABLE MACOS_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    string(REGEX REPLACE "^([0-9]+).*$" "\\1" MACOS_MAJOR_MINOR_VERSION "${MACOS_VERSION}.0")
endif()

ExternalProject_Add(luajit-ep
        SOURCE_DIR ${PROJECT_SOURCE_DIR}/contrib/LuaJIT/src
        INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}
        CONFIGURE_COMMAND ""
        BUILD_COMMAND MACOSX_DEPLOYMENT_TARGET=${MACOS_MAJOR_MINOR_VERSION} make -j8 libluajit.a
        BUILD_IN_SOURCE 1
        INSTALL_COMMAND cp <SOURCE_DIR>/libluajit.a <INSTALL_DIR>
        INSTALL_BYPRODUCTS <INSTALL_DIR>/libluajit.a
)
add_library(luajit SHARED IMPORTED)
add_dependencies(luajit luajit-ep)
set_target_properties(luajit
        PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/libluajit.a
)

#
# function to generate embedded file
#

set(GENHEADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "genheader source dir")

function(embed target source)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated)

    string(REPLACE "/" "_" TARGET_NAME "${source}")
    string(REPLACE "." "_" TARGET_NAME "${TARGET_NAME}")

    get_filename_component(FILENAME "${source}" NAME)

    add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated/${FILENAME}.h
            COMMAND ${GENHEADER_SOURCE_DIR}/bin/genheader ${source} ${ARGV2} > ${CMAKE_CURRENT_BINARY_DIR}/generated/${FILENAME}.h
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            DEPENDS ${source}
    )

    add_custom_target(${TARGET_NAME}_${target}
            DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/generated/${FILENAME}.h
            COMMENT "Generating header file for ${source}..."
    )

    add_dependencies(${target} ${TARGET_NAME}_${target})
endfunction()


#
# C library
#

add_library(pastel2d-temp STATIC EXCLUDE_FROM_ALL
        src/graphics.h
        src/graphics.c
        src/pastel2d.h
        src/pastel2d.c
        src/res.c
        src/res.h
        src/scene.c
        src/scene.h
        src/context.h
        src/context.c
        src/manip.c
        src/manip.h
        src/private/textcache.c
        src/private/textcache.h
        src/audio.c
        src/audio.h
        src/private/graphics_priv.h
        src/private/res_priv.h
        src/private/audio_priv.h
        src/private/scene_priv.h
        src/pastel2d.hh
        src/pastel2d-cc.cc
)

target_include_directories(pastel2d-temp
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/src
            ${CMAKE_CURRENT_SOURCE_DIR}/contrib/LuaJIT/src
            ${CMAKE_CURRENT_BINARY_DIR}/generated
            ${CMAKE_CURRENT_SOURCE_DIR}/contrib/pastel-log
        PRIVATE SYSTEM
            ${CMAKE_CURRENT_SOURCE_DIR}/contrib/stb
            ${CMAKE_CURRENT_SOURCE_DIR}/contrib/pocketmod
)
target_link_libraries(pastel2d-temp
        PRIVATE
            SDL3::SDL3
        PUBLIC
            m
)
target_compile_options(pastel2d-temp
        PRIVATE ${warnings})

#
# join libraries
#

if (APPLE)
    set(JOIN_AR libtool -static -o libpastel2d.a libpastel2d-temp.a libluajit.a)
else()
    set(JOIN_AR ar -M <${CMAKE_CURRENT_SOURCE_DIR}/libpastel2d.mri)
endif()

add_custom_target(combined
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${JOIN_AR}
        BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/libpastel2d.a
        DEPENDS pastel2d-temp luajit)

add_library(pastel2d STATIC IMPORTED GLOBAL)
add_dependencies(pastel2d combined)

set_target_properties(pastel2d
        PROPERTIES
        IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/libpastel2d.a
)

target_include_directories(pastel2d
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/generated
)

target_include_directories(pastel2d SYSTEM
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/contrib/LuaJIT/src
        ${CMAKE_CURRENT_SOURCE_DIR}/contrib/stb
        ${CMAKE_CURRENT_SOURCE_DIR}/contrib/pastel-log
)

target_link_libraries(pastel2d INTERFACE m)

#
# sample executable - C
#

add_executable(pastel2d-example EXCLUDE_FROM_ALL
        example/example.c
)
target_compile_options(pastel2d-example
        PRIVATE ${warnings})
target_link_libraries(pastel2d-example PRIVATE pastel2d SDL3::SDL3)

embed(pastel2d-example example/example.png)
embed(pastel2d-example example/example.tileset.lua lua-strip)
embed(pastel2d-example example/example-shadow.tileset.lua lua-strip)
embed(pastel2d-example example/OpenSans-Medium.ttf)
embed(pastel2d-example example/Born2bSportyFS.otf)
embed(pastel2d-example example/nemesis.mod)
embed(pastel2d-example example/shotgun.wav)

#
# sample executable - C++
#

add_executable(pastel2d-cc-example EXCLUDE_FROM_ALL
        example/example-cc.cc
)
target_compile_options(pastel2d-cc-example
        PRIVATE ${warnings})
target_link_libraries(pastel2d-cc-example PRIVATE pastel2d SDL3::SDL3)

embed(pastel2d-cc-example example/example.png)
embed(pastel2d-cc-example example/example.tileset.lua lua-strip)
embed(pastel2d-cc-example example/example-shadow.tileset.lua lua-strip)
embed(pastel2d-cc-example example/OpenSans-Medium.ttf)
embed(pastel2d-cc-example example/Born2bSportyFS.otf)
embed(pastel2d-cc-example example/nemesis.mod)
embed(pastel2d-cc-example example/shotgun.wav)

# check for leaks

add_custom_target(pastel2d-leaks)
add_custom_command(TARGET pastel2d-leaks
        POST_BUILD
        COMMENT "Check for leaks using valgrind."
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS pastel2d-example
        COMMAND valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --suppressions=${CMAKE_SOURCE_DIR}/valgrind.supp ./pastel2d-example
)
