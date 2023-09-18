// Copyright (c) 2023-present, eaxecx.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#include <nchip8/display.hpp>

#include <cstdint>

using namespace nchip8;

Display::Display(sdl::Renderer &renderer, Resolution res, int scaleFactor)
    : scaleFactor { scaleFactor },
      m_renderer  { renderer } {
    setResolution(res);
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

    float oldScaleX = m_renderer.GetXScale();
    float oldScaleY = m_renderer.GetYScale();

    m_renderer.SetScale((float) scaleFactor, (float) scaleFactor);

    for (std::size_t i = 0; i < m_pixels.size(); ++i) {
        int coordX =  (int) i % m_size.x;
        int coordY = ((int) i - coordX) / m_size.x;
        sdl::Point pos   = { coordX * m_pixelSize.x, coordY * m_pixelSize.y };
        sdl::Color color = m_pixels[i] ? this->onColor : this->offColor;

        drawPixel(pos, color);
    }

    m_renderer.SetScale(oldScaleX, oldScaleY);
}

void Display::clear() {
    m_pixels.reset();

    m_renderer.SetDrawColor(0x0);
    m_renderer.Clear();
}

void Display::setPixel(sdl::Point pos, PixelState state) {
    std::uint8_t bit = (std::uint8_t) state;

    m_pixels.set(pointToIdx(pos), bit);
}

PixelState Display::at(sdl::Point pos) const {
    return (PixelState) m_pixels.test(pointToIdx(pos));
}

std::size_t Display::pointToIdx(sdl::Point pos) const {
    pos.x = std::clamp(0, pos.x, m_size.x - 1);
    pos.y = std::clamp(0, pos.y, m_size.y - 1);

    return (std::size_t) (pos.y * m_size.x + pos.x);
}

void Display::setResolution(Resolution res) {
    m_res = res;

    switch (res) {
    case Resolution::LOW:
        m_pixelSize = LORES_PIXEL_SIZE;
        m_size = LORES_DISPLAY_SIZE;

        break;
    case Resolution::HIGH:
        m_pixelSize = HIRES_PIXEL_SIZE;
        m_size = HIRES_DISPLAY_SIZE;

        break;
    }
}

sdl::Point Display::size() const {
    return m_size;
}

Resolution Display::res() const {
    return m_res;
}
