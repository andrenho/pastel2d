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

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)

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
        src/pastel2d.hh
        src/error.c
        src/error.h
        src/res.c
        src/res.h
)
target_include_directories(${PROJECT_NAME}
        PUBLIC
            ${CMAKE_SOURCE_DIR}/src
            ${CMAKE_CURRENT_BINARY_DIR}/generated
)
target_link_libraries(${PROJECT_NAME}
        PUBLIC
            SDL3::SDL3
)

#
# sample executable
#

add_executable(pastel2d-example EXCLUDE_FROM_ALL
        example/example.c
)
target_link_libraries(pastel2d-example PRIVATE pastel2d)

embed(pastel2d-example example/example.png)