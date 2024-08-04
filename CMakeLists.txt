# Copyright (c) 2024 inunix3.
# This file is distributed under the MIT license (https://opensource.org/license/mit/)

cmake_minimum_required(VERSION 3.13)

project(nchip8 VERSION 1.0.0 LANGUAGES CXX)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
    set(COMPILER "clang")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(COMPILER "gcc")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

add_subdirectory(src)
add_subdirectory(third-party)
