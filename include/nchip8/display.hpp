// Copyright (c) 2023-present, eaxecx.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#pragma once

#include "sdl.hpp"

#include <bitset>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <unordered_set>
#include <vector>

namespace nchip8 {
    inline constexpr sdl::Point  LORES_DISPLAY_SIZE  = { 64, 32 };
    inline constexpr sdl::Point  HIRES_DISPLAY_SIZE  = { 128, 64 };
    inline constexpr sdl::Point  LORES_PIXEL_SIZE    = { 10, 10 };
    inline constexpr sdl::Point  HIRES_PIXEL_SIZE    = { 5, 5 };
    inline constexpr sdl::Color  DEFAULT_OFF_COLOR   = { 0x00, 0x00, 0x00, 0xff };
    inline constexpr sdl::Color  DEFAULT_ON_COLOR    = { 0xff, 0xff, 0xff, 0xff };

    enum class PixelState {
        OFF,
        ON
    };

    constexpr inline PixelState operator!(PixelState px) {
        return (PixelState) !(int) px;
    }

    enum class Resolution {
        LOW,
        HIGH
    };

    enum class ScrollDirection {
        DOWN,
        RIGHT,
        LEFT
    };

    struct Sprite {
        sdl::Point pos;

        std::vector<std::uint16_t> pixels;
        int width;
    };

    class Display {
    public:
        Display(sdl::Renderer &renderer);

        void prepare();
        void draw();
        void clear();
        void setPixel(sdl::Point pos, PixelState state);
        PixelState at(sdl::Point pos) const;

        bool drawSprite(const Sprite &sprite);
        void scroll(ScrollDirection dir, int n);

        void setResolution(Resolution res);
        void setScaleFactor(int factor);
        void setOffColor(sdl::Color color);
        void setOnColor(sdl::Color color);
        void setFadeSpeed(double speed);
        void enableGrid(bool enable);
        void enableFade(bool enable);

        sdl::Point size() const;
        Resolution res() const;
        int scaleFactor() const;
        sdl::Color offColor() const;
        sdl::Color onColor() const;
        bool gridEnabled() const;
        bool fadeEnabled() const;

        bool wrapPixelsX = false;
        bool wrapPixelsY = false;

    private:
        struct FadePixel {
            FadePixel(sdl::Point pos, sdl::Color color, sdl::Color offColor);

            sdl::Point pos;
            sdl::Color color;
            sdl::Color offColor;
            double step = 7.0;

            void fade(double speed);
            bool faded() const;
        };

        struct Line {
            Line() = default;

            std::bitset<HIRES_DISPLAY_SIZE.x> data;

            struct {
                std::size_t begin;
                std::size_t end;
            } updatedRegion;
        };

        static constexpr sdl::Point TEXTURE_SIZE = HIRES_DISPLAY_SIZE * HIRES_PIXEL_SIZE;

        void updateAllLines();
        bool drawSpritePixel(sdl::Point pos);

        std::deque<Line> m_lines;
        std::unordered_set<std::size_t> m_updatedLines;
        std::unordered_map<sdl::Point, FadePixel> m_fadePixels;
        std::uint32_t m_lastlyFaded = 0;

        sdl::Renderer &m_renderer;
        sdl::Texture m_texture;
        bool m_changed = false;

        bool m_enableGrid  = false;
        bool m_enableFade = false;
        int  m_scaleFactor = 1;
        double m_fadeSpeed = 5.0;
        sdl::Color m_offColor = DEFAULT_OFF_COLOR;
        sdl::Color m_onColor  = DEFAULT_ON_COLOR;
        sdl::Point m_pixelSize;
        sdl::Point m_size;
        Resolution m_res;
    };
}
