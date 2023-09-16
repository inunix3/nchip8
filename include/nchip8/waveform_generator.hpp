// Copyright (c) 2023-present, eaxecx.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#pragma once

#include "sdl.hpp"

#include <array>
#include <cstdint>

namespace nchip8 {
    enum class Waveform {
        SINE,
        SQUARE,
        SAW
    };

    class WaveformGenerator {
    public:
        WaveformGenerator(Waveform waveform, double level, int frequency);

        void play();
        void changeWaveform(Waveform waveform);

        double level;
        int frequency;

    private:
        // size is in samples, not in bytes (one sample is two bytes)
        static constexpr int BUFFER_SIZE = 256;
        static constexpr int SAMPLE_RATE = 44100;

        inline double nextSample() const;

        unsigned m_sampleCount = 0;
        Waveform m_waveform;

        sdl::AudioDevice m_audioDevice;
        std::array<std::int16_t, BUFFER_SIZE> m_buf;
    };
}
