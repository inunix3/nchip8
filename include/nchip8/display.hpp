// Copyright (c) 2023-present, eaxecx.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#pragma once

#include "sdl.hpp"

#include <bitset>
#include <cstddef>

namespace nchip8 {
    inline constexpr sdl::Point  DISPLAY_SIZE = { 64, 32 };
    inline constexpr sdl::Point  PIXEL_SIZE   = { 10, 10 };
    inline constexpr std::size_t DISPLAY_ARRAY_SIZE = DISPLAY_SIZE.x * DISPLAY_SIZE.y;
    inline constexpr sdl::Color  DEFAULT_OFF_COLOR  = { 0x00, 0x00, 0x00, 0xff };
    inline constexpr sdl::Color  DEFAULT_ON_COLOR   = { 0xff, 0xff, 0xff, 0xff };

    enum class PixelState {
        OFF,
        ON
    };

    constexpr inline PixelState operator!(PixelState px) {
        return (PixelState) !(int) px;
    }

    class Display {
    public:
        Display(sdl::Renderer &renderer, int scaleFactor);

        void draw();
        void clear();
        void setPixel(sdl::Point pos, PixelState state);
        PixelState at(sdl::Point pos) const;

        void setScaleFactor(int factor);

        sdl::Color offColor = DEFAULT_OFF_COLOR;
        sdl::Color onColor  = DEFAULT_ON_COLOR;
        bool enableGrid = false;

    private:
        std::size_t pointToIdx(sdl::Point pos) const;

        std::bitset<DISPLAY_ARRAY_SIZE> m_pixels;
        sdl::Renderer &m_renderer;
        sdl::Point m_pixelSize;
    };
}
