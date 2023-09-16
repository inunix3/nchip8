// Copyright (c) 2023-present, eaxecx.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#include <nchip8/display.hpp>

using namespace nchip8;

Display::Display(sdl::Renderer &renderer, int scaleFactor)
    : m_renderer  { renderer },
      m_pixelSize { PIXEL_SIZE * scaleFactor } {
}

void Display::draw() {
    auto drawPixel = [this](sdl::Point pos, sdl::Color color) {
        sdl::Rect pixel(pos, m_pixelSize);

        m_renderer.SetDrawColor(color);
        m_renderer.FillRect(pixel);

        if (enableGrid) {
            // Color of the grid is the inverted color of pixel (except for its alpha channel)
            color.r = ~color.r;
            color.g = ~color.g;
            color.b = ~color.b;

            m_renderer.SetDrawColor(color);
            m_renderer.DrawRect(pixel);
        }
    };

    for (std::size_t i = 0; i < m_pixels.size(); ++i) {
        int coordX = i % DISPLAY_SIZE.x;
        int coordY = ((int) i - coordX) / DISPLAY_SIZE.x;
        sdl::Point pos   = { coordX * m_pixelSize.x, coordY * m_pixelSize.y };
        sdl::Color color = m_pixels[i] ? this->onColor : this->offColor;

        drawPixel(pos, color);
    }
}

void Display::clear() {
    m_pixels.reset();

    m_renderer.SetDrawColor(0x0);
    m_renderer.Clear();
}

void Display::setPixel(sdl::Point pos, PixelState state) {
    uint8_t bit = (uint8_t) state;

    m_pixels.set(pointToIdx(pos), bit);
}

PixelState Display::at(sdl::Point pos) const {
    return (PixelState) m_pixels.test(pointToIdx(pos));
}

std::size_t Display::pointToIdx(sdl::Point pos) const {
    pos.x = std::clamp(0, pos.x, DISPLAY_SIZE.x - 1);
    pos.y = std::clamp(0, pos.y, DISPLAY_SIZE.y - 1);

    return (std::size_t) (pos.y * DISPLAY_SIZE.x + pos.x);
}

void Display::setScaleFactor(int factor) {
    m_pixelSize.x = PIXEL_SIZE.x * factor;
    m_pixelSize.y = PIXEL_SIZE.y * factor;
}
