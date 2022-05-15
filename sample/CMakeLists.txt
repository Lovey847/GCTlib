#******************************************************************************
#*
#* Copyright(c) 2022 Lian Ferrand
#* This file is part of GCTLib
#*
#* File description:
#*  GCTLib sample makefile
#*
#******************************************************************************

add_subdirectory("${CMAKE_SOURCE_DIR}/.." "${CMAKE_BINARY_DIR}/gctlib")

cmake_minimum_required(VERSION 3.16.2)
project(example C)

add_executable(example)

# Enable extra warnings and treat warnings as errors
if (CMAKE_BUILD_TYPE STREQUAL Debug)
  if (CMAKE_C_COMPILER_ID STREQUAL GNU)
    target_compile_options(example PRIVATE -pedantic -Wall -Wextra -Werror)
    message(VERBOSE "Added '-pedantic -Wall -Wextra -Werror'")
  endif ()
endif ()

message(VERBOSE "Setting C standard to C90")
set_target_properties(example PROPERTIES C_STANDARD 90)
set_target_properties(example PROPERTIES C_STANDARD_REQUIRED ON)
set_target_properties(example PROPERTIES C_EXTENSIONS OFF)

target_include_directories(example PRIVATE "${GCTLIB_INCLUDE_DIR}")
target_sources(example PRIVATE "${CMAKE_SOURCE_DIR}/src/main.c")
target_link_libraries(example gctlib)
