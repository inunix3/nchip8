// Copyright (c) 2023-present, eaxecx.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#include <nchip8/waveform_generator.hpp>

// All formulas and details can be found on Wikipedia:
//   Frequency:   https://en.wikipedia.org/wiki/Frequency
//   Decibel:     https://en.wikipedia.org/wiki/Decibel
//   Saw wave:    https://en.wikipedia.org/wiki/Sawtooth_wave
//   Sine wave:   https://en.wikipedia.org/wiki/Sine_wave
//   Square wave: https://en.wikipedia.org/wiki/Square_wave

#include <algorithm>
// On some older platforms this define is needed for the PI constant
#define _USE_MATH_DEFINES 1
#include <cmath>

using namespace nchip8;

namespace {
    inline sdl::AudioSpec createSpec(int sampleRate, int sampleCount) {
        return { sampleRate, AUDIO_S16LSB, 1, (std::uint16_t) sampleCount };
    }
}

WaveformGenerator::WaveformGenerator(Waveform waveform, double level, int frequency)
    : level     { level },
      frequency { frequency },
      m_audioDevice { sdl::NullOpt, 0, createSpec(SAMPLE_RATE, BUFFER_SIZE), 0 } {
    changeWaveform(waveform);
    m_audioDevice.Pause(false);
}

void WaveformGenerator::play() {
    auto clip = [](double x, double max, double min) -> double {
        return std::max(min, std::min(x, max));
    };

    auto dBToAmplitude = [](double dB) -> double {
        return std::pow(10, dB / 20);
    };

    double amplitude = dBToAmplitude(level);

    while (m_audioDevice.GetQueuedAudioSize() < BUFFER_SIZE * 2) {
        for (std::size_t i = 0; i < BUFFER_SIZE; ++i) {
            // Since our samples are generated in the range [-1; 1], we need increate it to make them audible.
            constexpr double GAIN = 1000.0;

            double sample = clip(amplitude * GAIN * nextSample(), INT16_MAX, INT16_MIN);
            m_buf[i] = (std::int16_t) sample;

            ++m_sampleCount;
        }

        m_audioDevice.QueueAudio(m_buf.data(), BUFFER_SIZE * 2);
    }
}

void WaveformGenerator::changeWaveform(Waveform waveform) {
    m_waveform = waveform;
    m_sampleCount = 0;
}

double WaveformGenerator::nextSample() const {
    auto freqToAngular = [](double f) -> double {
        return 2 * M_PI * f;
    };

    double t = (double) m_sampleCount / SAMPLE_RATE;

    switch (m_waveform) {
    case Waveform::SINE:
        return std::sin(freqToAngular(frequency) * t);
    case Waveform::SQUARE:
        return std::pow(-1, std::floor(2 * frequency * t));
    case Waveform::SAW:
        double period = t / (1.0 / frequency);

        return 2 * ((period - std::floor(0.5 + period)));
    }
}
