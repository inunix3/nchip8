// Copyright (c) 2024 inunix3.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#pragma once

#include "waveform_generator.hpp"
#include "display.hpp"
#include "sdl.hpp"
#include "ui/ui_style.hpp"

#include <array>
#include <ctime>
#include <ostream>
#include <string>

namespace nchip8 {
    inline constexpr const char *CONFIG_FILENAME = ".nchip8.toml";
    inline constexpr int KEY_COUNT = 16;

    using InputLayout = std::array<std::pair<SDL_Scancode, int>, KEY_COUNT>;

    inline constexpr InputLayout ORIGINAL_LAYOUT { {
        { SDL_SCANCODE_1, 0x1 },
        { SDL_SCANCODE_2, 0x2 },
        { SDL_SCANCODE_3, 0x3 },
        { SDL_SCANCODE_C, 0xC },

        { SDL_SCANCODE_4, 0x4 },
        { SDL_SCANCODE_5, 0x5 },
        { SDL_SCANCODE_6, 0x6 },
        { SDL_SCANCODE_D, 0xD },

        { SDL_SCANCODE_7, 0x7 },
        { SDL_SCANCODE_8, 0x8 },
        { SDL_SCANCODE_9, 0x9 },
        { SDL_SCANCODE_E, 0xE },

        { SDL_SCANCODE_A, 0xA },
        { SDL_SCANCODE_0, 0x0 },
        { SDL_SCANCODE_B, 0xB },
        { SDL_SCANCODE_F, 0xF }
    } };

    inline constexpr InputLayout MODERN_LAYOUT { {
        { SDL_SCANCODE_1, 0x1 },
        { SDL_SCANCODE_2, 0x2 },
        { SDL_SCANCODE_3, 0x3 },
        { SDL_SCANCODE_4, 0xC },

        { SDL_SCANCODE_Q, 0x4 },
        { SDL_SCANCODE_W, 0x5 },
        { SDL_SCANCODE_E, 0x6 },
        { SDL_SCANCODE_R, 0xD },

        { SDL_SCANCODE_A, 0x7 },
        { SDL_SCANCODE_S, 0x8 },
        { SDL_SCANCODE_D, 0x9 },
        { SDL_SCANCODE_F, 0xE },

        { SDL_SCANCODE_Z, 0xA },
        { SDL_SCANCODE_X, 0x0 },
        { SDL_SCANCODE_C, 0xB },
        { SDL_SCANCODE_V, 0xF }
    } };

    namespace default_values {
        namespace graphics {
            inline constexpr sdl::Color OFF_COLOR    = { 0x00, 0x00, 0x00, 0xff };
            inline constexpr sdl::Color ON_COLOR     = { 0x00, 0x00, 0x00, 0xff };
            inline constexpr sdl::Point WINDOW_SIZE  = LORES_DISPLAY_SIZE * 10;
            inline constexpr int        SCALE_FACTOR = 1;
        }

        namespace input {
            inline constexpr int         LAYOUT_IDX = 1; // MODERN_LAYOUT
            inline constexpr InputLayout LAYOUT     = MODERN_LAYOUT;
        }

        namespace cpu {
            inline constexpr unsigned CYCLES_PER_SEC = 250;
            inline constexpr bool     DEBUG_MODE     = false;
        }

        namespace sound {
            inline constexpr bool     ENABLE    = true;
            inline constexpr double   LEVEL     = 3.00; // dB
            inline constexpr int      FREQUENCY = 440;
            inline constexpr Waveform WAVEFORM  = Waveform::SQUARE;
        }

        namespace ui {
            inline constexpr ::nchip8::ui::UIStyle STYLE = ::nchip8::ui::UIStyle::DARK;
        }
    };

    struct GraphicsConfig {
        sdl::Color offColor   = { 0x00, 0x00, 0x00, 0xff };
        sdl::Color onColor    = { 0xff, 0xff, 0xff, 0xff };
        sdl::Point windowSize = LORES_DISPLAY_SIZE * 10;
        int scaleFactor = 1;
        bool enableFade = false;
    };

    struct InputConfig {
        int layoutIdx = 1; // Modern layout
        InputLayout layout = MODERN_LAYOUT;
    };

    struct CPUConfig {
        unsigned cyclesPerSec      = 250;
        bool     uncapCyclesPerSec = false;
        unsigned rngSeed           = (unsigned) time(NULL);
        bool     debugMode         = false;

        // By SCHIP design, these were supposed to be the RPL user flags on HP-48.
        //
        // SCHIP/XO-CHIP only
        std::uint64_t rplFlags = 0;
    };

    struct SoundConfig {
        bool     enable    = true;
        double   level     = 3.00; // dB
        int      frequency = 440;
        Waveform waveform  = Waveform::SQUARE;
    };

    struct UIConfig {
        ui::UIStyle style = ui::UIStyle::DARK;
    };

    struct Config {
        Config() = default;
        Config(const std::string &filename);

        void writeFile() const;
        void writeFile(const std::string &path) const;

        GraphicsConfig graphics;
        CPUConfig      cpu;
        InputConfig    input;
        SoundConfig    sound;
        UIConfig       ui;

        std::string savePath;
    };
}
