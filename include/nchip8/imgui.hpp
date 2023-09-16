// Copyright (c) 2023-present, eaxecx.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#pragma once

#include "sdl.hpp"
#include "utils.hpp"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <misc/cpp/imgui_stdlib.h>

#include <cstdint>
#include <string>

namespace nchip8::imgui {
    inline sdl::Color imVec4ToRGBA(ImVec4 color) {
        // ImGui for some reason uses BGR for colors, so to convert our RGBA colors, we must reverse their byte order
        // (the same applies to rgbaToImVec4()).
        std::uint32_t abgr = ImGui::ColorConvertFloat4ToU32(color);

        std::uint8_t r =  abgr & 0x000000ff;
        std::uint8_t g = (abgr & 0x0000ff00) >> 8;
        std::uint8_t b = (abgr & 0x00ff0000) >> 16;
        std::uint8_t a = (abgr & 0xff000000) >> 24;

        return sdl::Color(r, g, b, a);
    }

    inline ImVec4 rgbaToImVec4(sdl::Color color) {
        std::uint32_t abgr = 0;

        abgr |= (std::uint32_t) color.r;
        abgr |= (std::uint32_t) color.g << 8;
        abgr |= (std::uint32_t) color.b << 16;
        abgr |= (std::uint32_t) color.a << 24;

        return ImGui::ColorConvertU32ToFloat4(abgr);
    }
}
