# Copyright (c) 2023-present, eaxecx.
# This file is distributed under the MIT license (https://opensource.org/license/mit/)

find_package(OpenGL REQUIRED)

# toml11
add_subdirectory(toml11)

# SDL2
set(SDL_TEST       OFF CACHE BOOL "" FORCE)
set(SDL_SHARED     OFF CACHE BOOL "" FORCE)
set(SDL_STATIC     ON  CACHE BOOL "" FORCE)
set(SDL_STATIC_PIC ON  CACHE BOOL "" FORCE)
add_subdirectory(sdl2)

# libSDL2pp
set(SDL2PP_WITH_IMAGE OFF)
set(SDL2PP_WITH_MIXER OFF)
set(SDL2PP_WITH_TTF   OFF)
set(SDL2PP_STATIC     ON)
add_subdirectory(libSDL2pp)
target_compile_options(SDL2pp PRIVATE -w)

# ImGui
set(IMGUI_DIR "${PROJECT_SOURCE_DIR}/third-party/imgui")
set(IMGUI_BACKENDS_DIR "${IMGUI_DIR}/backends")

set(IMGUI_HEADERS
    "${IMGUI_DIR}/imconfig.h"
    "${IMGUI_DIR}/imgui.h"
    "${IMGUI_DIR}/imgui_internal.h"
    "${IMGUI_DIR}/imstb_rectpack.h"
    "${IMGUI_DIR}/imstb_textedit.h"
    "${IMGUI_DIR}/imstb_truetype.h"
    "${IMGUI_DIR}/misc/cpp/imgui_stdlib.h"
)

set(IMGUI_SOURCES
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_demo.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp"
)

set(IMGUI_BACKENDS_HEADERS
    "${IMGUI_BACKENDS_DIR}/imgui_impl_sdl2.h"
    "${IMGUI_BACKENDS_DIR}/imgui_impl_sdlrenderer2.h"
)

set(IMGUI_BACKENDS_SOURCES
    "${IMGUI_BACKENDS_DIR}/imgui_impl_sdl2.cpp"
    "${IMGUI_BACKENDS_DIR}/imgui_impl_sdlrenderer2.cpp"
)

add_library(imgui STATIC ${IMGUI_HEADERS} ${IMGUI_BACKENDS_HEADERS} ${IMGUI_SOURCES} ${IMGUI_BACKENDS_SOURCES})
target_include_directories(imgui PUBLIC ${IMGUI_DIR} ${IMGUI_BACKENDS_DIR})
target_link_libraries(imgui PRIVATE SDL2::SDL2-static ${OPENGL_LIBRARIES})

# ImGuiFileDialog
add_subdirectory(ImGuiFileDialog)
target_link_libraries(ImGuiFileDialog PUBLIC imgui)
