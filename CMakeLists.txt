#******************************************************************************
#*
#* Copyright(c) 2022 Lian Ferrand
#* This file is part of GCTLib
#*
#* File description:
#*  GCTLib library makefile
#*
#******************************************************************************

cmake_minimum_required(VERSION 3.16.2)
project(gctlib C)

add_library(gctlib STATIC)

# Enable extra warnings and treat warnings as errors
if (CMAKE_BUILD_TYPE STREQUAL Debug)
  if (CMAKE_C_COMPILER_ID STREQUAL GNU)
    target_compile_options(gctlib PRIVATE -pedantic -Wall -Wextra -Werror)
    message(VERBOSE "Added '-pedantic -Wall -Wextra -Werror'")
  endif ()
endif ()

message(VERBOSE "GCTLib: Settings C standard to C99")
set_target_properties(gctlib PROPERTIES C_STANDARD 99)
set_target_properties(gctlib PROPERTIES C_STANDARD_REQUIRED ON)
set_target_properties(gctlib PROPERTIES C_EXTENSIONS OFF)

target_include_directories(gctlib PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(gctlib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_sources(gctlib PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/gctlib.c")
target_sources(gctlib PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/encode.c")
target_sources(gctlib PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/decode.c")

set(GCTLIB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include" PARENT_SCOPE)
