// Copyright (c) 2024 inunix3.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#include <nchip8/display.hpp>

#include <cstdint>

using namespace nchip8;

Display::Display(sdl::Renderer &renderer)
    : m_lines     { HIRES_DISPLAY_SIZE.y },
      m_renderer  { renderer },
      m_texture   { renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, TEXTURE_SIZE.x, TEXTURE_SIZE.y } {
    setResolution(Resolution::LOW);
}

void Display::prepare() {
    auto drawPixel = [this](sdl::Point pos, sdl::Color color) {
        sdl::Rect pixel(pos * m_pixelSize, m_pixelSize);

        m_renderer.SetDrawColor(color);
        m_renderer.FillRect(pixel);

        if (m_enableGrid) {
            // Color of the grid is the inverted color of pixel (except for its alpha channel)
            color.r = ~color.r;
            color.g = ~color.g;
            color.b = ~color.b;

            m_renderer.SetDrawColor(color);
            m_renderer.DrawRect(pixel);
        }
    };

    auto fadePixels = [this, drawPixel]() {
        std::uint32_t currentTime = SDL_GetTicks();
        std::uint32_t deltaTime = currentTime - m_lastlyFaded;

        if (deltaTime >= 10) {
            m_lastlyFaded = currentTime;

            for (auto it = m_fadePixels.begin(); it != m_fadePixels.end();) {
                auto &px = it->second;

                px.fade(m_fadeSpeed);
                drawPixel(px.pos, px.color);

                if (px.faded()) {
                    it = m_fadePixels.erase(it);
                } else {
                    ++it;
                }
            }
        }
    };

    auto drawBuffer = [this, drawPixel]() {
        for (auto lineN : m_updatedLines) {
            const auto &line = m_lines[lineN];

            for (std::size_t i = line.updatedRegion.begin; i != line.updatedRegion.end; ++i) {
                auto color = line.data[i] ? m_onColor : m_offColor;

                drawPixel({ (int) i, (int) lineN }, color);
            }
        }
    };

    // We're writing our display buffer to the texture to speed up rendering
    m_renderer.SetTarget(m_texture);

    if (m_enableFade) {
        fadePixels();
    }

    if (m_changed) {
        drawBuffer();
    }

    // Reset target to the default
    m_renderer.SetTarget();

    m_updatedLines.clear();
    m_changed = false;
}

void Display::draw() {
    sdl::Rect part = { { 0, 0 }, m_size * m_pixelSize };

    float oldScaleX = m_renderer.GetXScale();
    float oldScaleY = m_renderer.GetYScale();

    m_renderer.SetScale((float) m_scaleFactor, (float) m_scaleFactor);
    m_renderer.Copy(m_texture, sdl::NullOpt, part);
    m_renderer.SetScale(oldScaleX, oldScaleY);
}

void Display::clear() {
    for (std::size_t y = 0; y < (std::size_t) m_size.y; ++y) {
        auto &line = m_lines[y];

        for (std::size_t x = 0; x < (std::size_t) m_size.x; ++x) {
            if (line.data[x]) {
                sdl::Point pos = { (int) x, (int) y };

                if (m_enableFade) {
                    m_fadePixels.insert({ pos, { pos, m_onColor, m_offColor } });
                }

                line.data[x] = 0;
            }
        }
    }

    updateAllLines();
}

void Display::setPixel(sdl::Point pos, PixelState state) {
    std::size_t posX = (std::size_t) pos.x;
    std::size_t posY = (std::size_t) pos.y;
    std::uint8_t bit = (std::uint8_t) state;

    auto &line = m_lines[posY];

    if (m_fadePixels.count(pos)) {
        m_fadePixels.erase(pos);
    }

    if (m_enableFade && !bit && line.data[posX]) {
        m_fadePixels.insert({ pos, { pos, m_onColor, m_offColor } });
    }

    line.data[posX] = bit;

    auto &region = line.updatedRegion;
    region.begin = region.begin == 0 ? posX : std::min(region.begin, posX);

    if (posX >= region.begin) {
        region.end = region.begin + posX;
    }

    m_updatedLines.insert(posY);
    m_changed = true;
}

PixelState Display::at(sdl::Point pos) const {
    return (PixelState) m_lines[(std::size_t) pos.y].data[(std::size_t) pos.x];
}

bool Display::drawSprite(const Sprite &sprite) {
    auto height = (int) sprite.pixels.size();
    bool collisionDetected = false;
    unsigned mask = sprite.width > 8 ? 0x8000 : 0x80;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < sprite.width; ++x) {
            std::uint16_t row = sprite.pixels[(std::size_t) y];

            if (!(row & (mask >> x))) {
                continue;
            }

            if (drawSpritePixel({ sprite.pos.x + x, sprite.pos.y + y })) {
                collisionDetected = true;
            }
        }
    }

    return collisionDetected;
}

void Display::scroll(ScrollDirection dir, int n) {
    if (m_res == Resolution::LOW) {
        n /= 2;
    }

    switch (dir) {
    case ScrollDirection::DOWN:
        for (int i = 0; i < n; ++i) {
            // Push an empty line at the top of display
            m_lines.push_front({ });
            // Remove the last line
            m_lines.pop_back();
        }

        break;
    case ScrollDirection::RIGHT:
        for (auto &line : m_lines) {
            line.data <<= (std::size_t) n;
        }

        break;
    case ScrollDirection::LEFT:
        for (auto &line : m_lines) {
            line.data >>= (std::size_t) n;
        }

        break;
    }

    updateAllLines();
}

void Display::setResolution(Resolution res) {
    m_res = res;

    switch (res) {
    case Resolution::LOW:
        m_lines.resize((std::size_t) LORES_DISPLAY_SIZE.y);
        m_pixelSize = LORES_PIXEL_SIZE;
        m_size = LORES_DISPLAY_SIZE;

        break;
    case Resolution::HIGH:
        m_lines.resize((std::size_t) HIRES_DISPLAY_SIZE.y);
        m_pixelSize = HIRES_PIXEL_SIZE;
        m_size = HIRES_DISPLAY_SIZE;

        break;
    }

    updateAllLines();
}

void Display::setScaleFactor(int factor) {
    m_scaleFactor = factor;

    updateAllLines();
}

void Display::setOffColor(sdl::Color color) {
    m_offColor = color;
    m_fadePixels.clear();

    updateAllLines();
}

void Display::setOnColor(sdl::Color color) {
    m_onColor = color;
    m_fadePixels.clear();

    updateAllLines();
}

void Display::setFadeSpeed(double speed) {
    m_fadeSpeed = speed * 0.005;
}

void Display::enableGrid(bool enable) {
    m_enableGrid = enable;

    updateAllLines();
}

void Display::enableFade(bool enable) {
    m_enableFade = enable;

    if (!enable) {
        m_fadePixels.clear();
    }
}

sdl::Point Display::size() const {
    return m_size;
}

Resolution Display::res() const {
    return m_res;
}

int Display::scaleFactor() const {
    return m_scaleFactor;
}

sdl::Color Display::offColor() const {
    return m_offColor;
}

sdl::Color Display::onColor() const {
    return m_onColor;
}

bool Display::gridEnabled() const {
    return m_enableGrid;
}

bool Display::fadeEnabled() const {
    return m_enableFade;
}

void Display::updateAllLines() {
    for (std::size_t i = 0; i < m_lines.size(); ++i) {
        auto &line = m_lines[i];

        line.updatedRegion.begin = 0;
        line.updatedRegion.end   = (std::size_t) m_size.x;

        m_updatedLines.insert(i);
    }

    m_changed = true;
}

bool Display::drawSpritePixel(sdl::Point pos) {
    bool collisionDetected = false;

    if (pos.x >= m_size.x) {
        if (wrapPixelsX) {
            pos.x %= m_size.x;
        } else {
            return false;
        }
    }

    if (pos.y >= m_size.y) {
        if (wrapPixelsY) {
            pos.y %= m_size.y;
        } else {
            return false;
        }
    }

    PixelState pxState = at(pos);

    if (pxState == PixelState::ON) {
        collisionDetected = true;
    }

    setPixel(pos, !pxState);

    return collisionDetected;
}

Display::FadePixel::FadePixel(sdl::Point pos, sdl::Color color, sdl::Color offColor)
    : pos { pos }, color { color }, offColor { offColor } {
        auto isGreater = [](sdl::Color a, sdl::Color b) -> bool {
            return a.r > b.r || a.g > b.g || a.b > b.b;
        };

        step = isGreater(color, offColor) ? -step : step;
    }

void Display::FadePixel::fade(double speed) {
    auto fadeColor = [this, speed](std::uint8_t pxColor, std::uint8_t refColor) -> std::uint8_t {
        if (pxColor == refColor) {
            return pxColor;
        }

        double result = pxColor + step * speed;

        if ((std::signbit(step) && result < refColor) ||
           (!std::signbit(step) && result > refColor)) {
            result = refColor;
        }

        return (std::uint8_t) result;
    };

    color.r = fadeColor(color.r, offColor.r);
    color.g = fadeColor(color.g, offColor.g);
    color.b = fadeColor(color.b, offColor.b);
}

bool Display::FadePixel::faded() const {
    return color == offColor;
}
