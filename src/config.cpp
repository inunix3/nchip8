// Copyright (c) 2023-present, eaxecx.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#include <nchip8/config.hpp>

#include <libconfig.h++>

using namespace nchip8;
namespace lc = libconfig;

Config::Config(const std::string &path) : savePath { path } {
    auto u32ToColor = [](std::uint32_t color) -> sdl::Color {
        std::uint8_t r = (color & 0xff000000) >> 24;
        std::uint8_t g = (color & 0x00ff0000) >> 16;
        std::uint8_t b = (color & 0x0000ff00) >>  8;
        std::uint8_t a =  color & 0x000000ff;

        return { r, g, b, a };
    };

    lc::Config cfg;
    cfg.readFile(path);

    // Variables with 'Group' at the end were so named to avoid warnings about shadowing some fields.
    const auto &root = cfg.getRoot();
    const auto &graphicsGroup = root["graphics"];
    const auto &cpuGroup      = root["cpu"];
    const auto &inputGroup    = root["input"];
    const auto &soundGroup    = root["sound"];
    const auto &uiGroup       = root["ui"];

    graphics.onColor      = u32ToColor((std::uint32_t) (int) graphicsGroup.lookup("onColor"));
    graphics.offColor     = u32ToColor((std::uint32_t) (int) graphicsGroup.lookup("offColor"));
    graphics.windowSize.x = graphicsGroup.lookup("windowWidth");
    graphics.windowSize.y = graphicsGroup.lookup("windowHeight");
    graphics.scaleFactor  = graphicsGroup.lookup("scaleFactor");
    graphics.enableFade   = graphicsGroup.lookup("enableFade");

    cpu.cyclesPerSec      = cpuGroup.lookup("cyclesPerSec");
    cpu.uncapCyclesPerSec = cpuGroup.lookup("uncapCyclesPerSec");
    cpu.rplFlags          = (std::uint64_t) (long long) cpuGroup.lookup("rplFlags");

    input.layoutIdx = inputGroup.lookup("layoutIdx");
    input.layout    = input.layoutIdx == 0 ? ORIGINAL_LAYOUT : MODERN_LAYOUT;

    sound.enable    = soundGroup.lookup("enable");
    sound.level     = soundGroup.lookup("level");
    sound.frequency = soundGroup.lookup("frequency");
    sound.waveform  = (Waveform) (int) soundGroup.lookup("waveform");

    ui.style = (ui::UIStyle) (int) uiGroup.lookup("style");
}

void Config::writeFile() const {
    writeFile(savePath);
}

void Config::writeFile(const std::string &path) const {
    auto colorToInt = [](sdl::Color color) -> std::uint32_t {
        return std::uint32_t ((color.r << 24) | (color.g << 16) | (color.b << 8) | color.a);
    };

    lc::Config cfg;

    lc::Setting &root = cfg.getRoot();
    lc::Setting &graphicsGroup = root.add("graphics", lc::Setting::TypeGroup);
    lc::Setting &cpuGroup      = root.add("cpu",      lc::Setting::TypeGroup);
    lc::Setting &inputGroup    = root.add("input",    lc::Setting::TypeGroup);
    lc::Setting &soundGroup    = root.add("sound",    lc::Setting::TypeGroup);
    lc::Setting &uiGroup       = root.add("ui",       lc::Setting::TypeGroup);

    graphicsGroup.add("onColor",      lc::Setting::TypeInt)     = (int) colorToInt(graphics.onColor);
    graphicsGroup.add("offColor",     lc::Setting::TypeInt)     = (int) colorToInt(graphics.offColor);
    graphicsGroup.add("windowWidth",  lc::Setting::TypeInt)     = graphics.windowSize.x;
    graphicsGroup.add("windowHeight", lc::Setting::TypeInt)     = graphics.windowSize.y;
    graphicsGroup.add("scaleFactor",  lc::Setting::TypeInt)     = graphics.scaleFactor;
    graphicsGroup.add("enableFade",   lc::Setting::TypeBoolean) = graphics.enableFade;

    cpuGroup.add("cyclesPerSec",      lc::Setting::TypeInt)     = (int) cpu.cyclesPerSec;
    cpuGroup.add("uncapCyclesPerSec", lc::Setting::TypeBoolean) = cpu.uncapCyclesPerSec;
    cpuGroup.add("rplFlags",          lc::Setting::TypeInt64)   = (long long) cpu.rplFlags;

    inputGroup.add("layoutIdx", lc::Setting::TypeInt) = input.layoutIdx;

    soundGroup.add("enable",    lc::Setting::TypeBoolean) = sound.enable;
    soundGroup.add("level",     lc::Setting::TypeFloat) = sound.level;
    soundGroup.add("frequency", lc::Setting::TypeInt) = sound.frequency;
    soundGroup.add("waveform",  lc::Setting::TypeInt) = (int) sound.waveform;

    uiGroup.add("style", lc::Setting::TypeInt) = (int) ui.style;

    cfg.writeFile(path);
}
