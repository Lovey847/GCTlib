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

# Encoder
add_executable(encoder)

# Enable extra warnings and treat warnings as errors
if (CMAKE_BUILD_TYPE STREQUAL Debug)
  if (CMAKE_C_COMPILER_ID STREQUAL GNU)
    target_compile_options(encoder PRIVATE -pedantic -Wall -Wextra -Werror)
    message(VERBOSE "encoder: Added '-pedantic -Wall -Wextra -Werror'")
  endif ()
endif ()

message(VERBOSE "encoder: Setting C standard to C90")
set_target_properties(encoder PROPERTIES C_STANDARD 90)
set_target_properties(encoder PROPERTIES C_STANDARD_REQUIRED ON)
set_target_properties(encoder PROPERTIES C_EXTENSIONS OFF)

target_include_directories(encoder PRIVATE "${GCTLIB_INCLUDE_DIR}")
target_sources(encoder PRIVATE "${CMAKE_SOURCE_DIR}/src/encode.c")
target_link_libraries(encoder gctlib)

# Decoder
add_executable(decoder)

# Enable extra warnings and treat warnings as errors
if (CMAKE_BUILD_TYPE STREQUAL Debug)
  if (CMAKE_C_COMPILER_ID STREQUAL GNU)
    target_compile_options(decoder PRIVATE -pedantic -Wall -Wextra -Werror)
    message(VERBOSE "decoder: Added '-pedantic -Wall -Wextra -Werror'")
  endif ()
endif ()

message(VERBOSE "decoder: Setting C standard to C90")
set_target_properties(decoder PROPERTIES C_STANDARD 90)
set_target_properties(decoder PROPERTIES C_STANDARD_REQUIRED ON)
set_target_properties(decoder PROPERTIES C_EXTENSIONS OFF)

target_include_directories(decoder PRIVATE "${GCTLIB_INCLUDE_DIR}")
target_sources(decoder PRIVATE "${CMAKE_SOURCE_DIR}/src/decode.c")
target_link_libraries(decoder gctlib)
