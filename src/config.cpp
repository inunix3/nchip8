// Copyright (c) 2024 inunix3.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#include <nchip8/config.hpp>

#include <toml.hpp>

#include <fstream>

using namespace nchip8;

Config::Config(const std::string &path) : savePath { path } {
    toml::value root = toml::parse(path);

    const auto &graphicsTable = toml::find_or(root, "graphics", {});
    const auto &cpuTable      = toml::find_or(root, "cpu", {});
    const auto &inputTable    = toml::find_or(root, "input", {});
    const auto &soundTable    = toml::find_or(root, "sound", {});
    const auto &uiTable       = toml::find_or(root, "ui", {});
    
    auto u32ToColor = [](std::uint32_t color) -> sdl::Color {
        std::uint8_t r = (color & 0xff000000) >> 24;
        std::uint8_t g = (color & 0x00ff0000) >> 16;
        std::uint8_t b = (color & 0x0000ff00) >>  8;
        std::uint8_t a =  color & 0x000000ff;

        return { r, g, b, a };
    };

    std::uint32_t onColorHex  = toml::find_or(graphicsTable, "onColor",  0xffffffffu);
    std::uint32_t offColorHex = toml::find_or(graphicsTable, "offColor", 0x00000000u);

    graphics.onColor  = u32ToColor(onColorHex);
    graphics.offColor = u32ToColor(offColorHex);
    graphics.windowSize.x = toml::find_or(graphicsTable, "windowWidth", LORES_DISPLAY_SIZE.x * 10);
    graphics.windowSize.y = toml::find_or(graphicsTable, "windowHeight", LORES_DISPLAY_SIZE.y * 10);
    graphics.scaleFactor  = toml::find_or(graphicsTable, "scaleFactor", 1);
    graphics.enableFade   = toml::find_or(graphicsTable, "enableFade", false);

    cpu.cyclesPerSec      = toml::find_or(cpuTable, "cyclesPerSec", 250u);
    cpu.uncapCyclesPerSec = toml::find_or(cpuTable, "uncapCyclesPerSec", false);
    cpu.rplFlags          = toml::find_or(cpuTable, "rplFlags", (std::uint64_t) 0);

    input.layoutIdx = toml::find_or(inputTable, "layoutIdx", 1); // Modern layout
    input.layout    = input.layoutIdx == 0 ? ORIGINAL_LAYOUT : MODERN_LAYOUT;

    sound.enable    = toml::find_or(soundTable, "enable", true);
    sound.level     = toml::find_or(soundTable, "level", 3.00); // dB
    sound.frequency = toml::find_or(soundTable, "frequency", 440);
    sound.waveform  = (Waveform) toml::find_or(soundTable, "waveform", (int) Waveform::SQUARE);

    ui.style = (ui::UIStyle) toml::find_or(uiTable, "style", (int) ui::UIStyle::DARK);
}

void Config::writeFile() const {
    writeFile(savePath);
}

void Config::writeFile(const std::string &path) const {
    auto colorToU32 = [](sdl::Color color) -> std::uint32_t {
        return std::uint32_t ((color.r << 24) | (color.g << 16) | (color.b << 8) | color.a);
    };

    toml::value root = toml::table();
    toml::value graphicsTable = toml::table();
    toml::value cpuTable      = toml::table();
    toml::value inputTable    = toml::table();
    toml::value soundTable    = toml::table();
    toml::value uiTable       = toml::table();

    graphicsTable["onColor"]      = colorToU32(graphics.onColor);
    graphicsTable["offColor"]     = colorToU32(graphics.offColor);
    graphicsTable["windowWidth"]  = graphics.windowSize.x;
    graphicsTable["windowHeight"] = graphics.windowSize.y;
    graphicsTable["scaleFactor"]  = graphics.scaleFactor;
    graphicsTable["enableFade"]   = graphics.enableFade;

    cpuTable["cyclesPerSec"]      = cpu.cyclesPerSec;
    cpuTable["uncapCyclesPerSec"] = cpu.uncapCyclesPerSec;
    cpuTable["rplFlags"]          = cpu.rplFlags;

    inputTable["layoutIdx"] = input.layoutIdx;

    soundTable["enable"]    = sound.enable;
    soundTable["level"]     = sound.level;
    soundTable["frequency"] = sound.frequency;
    soundTable["waveform"]  = (int) sound.waveform;

    uiTable["style"] = (int) ui.style;

    root["graphics"] = graphicsTable;
    root["cpu"]      = cpuTable;
    root["input"]    = inputTable;
    root["sound"]    = soundTable;
    root["ui"]       = uiTable;

    std::ofstream file(path, std::ios::binary | std::ios::trunc);
    file << root;
}
