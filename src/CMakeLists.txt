# Copyright (c) 2023-present, eaxecx.
# This file is distributed under the MIT license (https://opensource.org/license/mit/)

set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include/nchip8")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")

set(HEADERS
    "${INCLUDE_DIR}/application.hpp"
    "${INCLUDE_DIR}/breakpoint.hpp"
    "${INCLUDE_DIR}/config.hpp"
    "${INCLUDE_DIR}/display.hpp"
    "${INCLUDE_DIR}/imgui.hpp"
    "${INCLUDE_DIR}/instr_set.hpp"
    "${INCLUDE_DIR}/instruction.hpp"
    "${INCLUDE_DIR}/main.hpp"
    "${INCLUDE_DIR}/sdl.hpp"
    "${INCLUDE_DIR}/utils.hpp"
    "${INCLUDE_DIR}/vm.hpp"
    "${INCLUDE_DIR}/waveform_generator.hpp"
    "${INCLUDE_DIR}/ui/breakpoints.hpp"
    "${INCLUDE_DIR}/ui/disassembler.hpp"
    "${INCLUDE_DIR}/ui/instr_executor.hpp"
    "${INCLUDE_DIR}/ui/keypad.hpp"
    "${INCLUDE_DIR}/ui/registers.hpp"
    "${INCLUDE_DIR}/ui/settings.hpp"
    "${INCLUDE_DIR}/ui/stack.hpp"
    "${INCLUDE_DIR}/ui/ui.hpp"
    "${INCLUDE_DIR}/ui/ui_style.hpp"
    "${INCLUDE_DIR}/ui/window.hpp"
)

set(SOURCES
    "${SRC_DIR}/application.cpp"
    "${SRC_DIR}/breakpoint.cpp"
    "${SRC_DIR}/config.cpp"
    "${SRC_DIR}/display.cpp"
    "${SRC_DIR}/instr_set.cpp"
    "${SRC_DIR}/instruction.cpp"
    "${SRC_DIR}/main.cpp"
    "${SRC_DIR}/vm.cpp"
    "${SRC_DIR}/waveform_generator.cpp"
    "${SRC_DIR}/ui/breakpoints.cpp"
    "${SRC_DIR}/ui/disassembler.cpp"
    "${SRC_DIR}/ui/instr_executor.cpp"
    "${SRC_DIR}/ui/keypad.cpp"
    "${SRC_DIR}/ui/registers.cpp"
    "${SRC_DIR}/ui/settings.cpp"
    "${SRC_DIR}/ui/stack.cpp"
    "${SRC_DIR}/ui/ui.cpp"
    "${SRC_DIR}/ui/window.cpp"
)

if (COMPILER STREQUAL "gcc" OR COMPILER STREQUAL "clang")
    set(COMPILE_OPTIONS -Wall -Wextra)

    if (NOT COMPILER STREQUAL "gcc")
        # The -Weffc++ flag has been broken in GCC for over 10 years!!
        set(COMPILE_OPTIONS ${COMPILE_OPTIONS} -Weffc++)

        # These flags also report too many warnings
        set(COMPILE_OPTIONS ${COMPILE_OPTIONS} -Wshadow)
        set(COMPILE_OPTIONS ${COMPILE_OPTIONS} -Wconversion)
        set(COMPILE_OPTIONS ${COMPILE_OPTIONS} -Wsign-conversion)
    endif()
endif()

add_executable(nchip8 ${HEADERS} ${SOURCES})
target_compile_options(nchip8 PRIVATE ${COMPILE_OPTIONS})
target_include_directories(nchip8 PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(nchip8 PRIVATE m toml11::toml11 SDL2pp::SDL2pp imgui ${OPENGL_LIBRARIES} ImGuiFileDialog)

install(TARGETS nchip8 DESTINATION bin)
