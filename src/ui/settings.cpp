// Copyright (c) 2023-present, eaxecx.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#include <nchip8/ui/settings.hpp>
#include <nchip8/ui/ui.hpp>

#include <cinttypes>
#include <cstddef>

using namespace nchip8::ui;

Settings::Settings(sdl::Window &window, VM &vm, UI &ui)
    : Window { "Settings", ImGuiWindowFlags_AlwaysAutoResize },
      m_window { window },
      m_vm     { vm },
      m_ui     { ui },
      m_newCfg     { vm.cfg },
      m_quirks     { vm.quirks },
      m_offColor   { imgui::rgbaToImVec4(vm.cfg.graphics.offColor) },
      m_onColor    { imgui::rgbaToImVec4(vm.cfg.graphics.onColor)  },
      m_enableGrid { vm.display.gridEnabled() },
}

void Settings::body() {
    auto &cfg     = m_vm.cfg;
    auto &display = m_vm.display;
    auto &beeper  = m_vm.beeper;

    if (ImGui::BeginTabBar("Settings Tab bar")) {
        if (ImGui::BeginTabItem("CPU"))      { sectionCPU();      ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Quirks"))   { sectionQuirks();   ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Graphics")) { sectionGraphics(); ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Sound"))    { sectionSound();    ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Input"))    { sectionInput();    ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("UI"))       { sectionUI();       ImGui::EndTabItem(); }
    }

    ImGui::EndTabBar();

    auto updateSettings = [&]() {
        m_newCfg.graphics.offColor = imgui::imVec4ToRGBA(m_offColor);
        m_newCfg.graphics.onColor = imgui::imVec4ToRGBA(m_onColor);

        if (cfg.graphics.windowSize != m_newCfg.graphics.windowSize) {
            m_window.SetSize(m_newCfg.graphics.windowSize);
        }

        if (cfg.sound.waveform != m_newCfg.sound.waveform) {
            beeper.changeWaveform(m_newCfg.sound.waveform);
        }

        if (cfg.ui.style != m_newCfg.ui.style) {
            m_ui.setStyle(m_newCfg.ui.style);
        }

        cfg = m_newCfg;
        cfg.writeFile();

        m_vm.quirks = m_quirks;

        display.setOffColor(cfg.graphics.offColor);
        display.setOnColor(cfg.graphics.onColor);
        m_enableGrid ? display.enableGrid() : display.disableGrid();
        display.setScaleFactor(cfg.graphics.scaleFactor);
        display.wrapPixelsX = m_quirks.wrapPixelsX;
        display.wrapPixelsY = m_quirks.wrapPixelsY;

        beeper.frequency = (int) cfg.sound.frequency;
        beeper.level = cfg.sound.level;
    };

    ImGui::Dummy({ 0, 5 });

    if (ImGui::Button("OK")) {
        updateSettings();

        show = false;
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel")) {
        show = false;
    }

    ImGui::SameLine();

    if (ImGui::Button("Apply")) {
        updateSettings();
    }
}

void Settings::sectionCPU() {
    ImGui::PushItemWidth(ImGui::GetFontSize() * 7);
    ImGui::InputScalar("Cycles/sec", ImGuiDataType_U32, &m_newCfg.cpu.cyclesPerSec, nullptr, nullptr, "%" PRId32);
    ImGui::InputScalar("PRNG seed",  ImGuiDataType_U32, &m_newCfg.cpu.rngSeed,      nullptr, nullptr, "%" PRId32);
    ImGui::PopItemWidth();

    ImGui::SameLine();

    if (ImGui::SmallButton("Auto seed")) {
        m_newCfg.cpu.rngSeed = (unsigned) time(nullptr);
    }

    markerNotSaved();

    ImGui::Checkbox("Debug mode", &m_newCfg.cpu.debugMode);
    markerNotSaved();
}

void Settings::sectionQuirks() {
    ImGui::Checkbox("Bnnn: use only V0 as the offset", &m_quirks.jumpOffsetUseV0);
    markerNotSaved();
    ImGui::Checkbox("Dxyn: horizontal wrapping", &m_quirks.wrapPixelsX);
    markerNotSaved();
    ImGui::Checkbox("Dxyn: vertical wrapping", &m_quirks.wrapPixelsY);
    markerNotSaved();
    ImGui::Checkbox("8xy6 and 8xyE: set VX to VY", &m_quirks.shiftSetVxToVy);
    markerNotSaved();
    ImGui::Checkbox("8xy1, 8xy2 and 8xy3: reset VF", &m_quirks.bitwiseResetVF);
    markerNotSaved();
    ImGui::Checkbox("Fx55 and Fx65: increment I", &m_quirks.loadSaveIncrementI);
    markerNotSaved();

    ImGui::SeparatorText("SCHIP");
    ImGui::Checkbox("Dxy0: draw 8x16 sprite in lo-res mode", &m_quirks.draw8x16SpriteInLores);
    markerNotSaved();
}

void Settings::sectionGraphics() {
    static const sdl::Point windowSizes[] = {
        { 640,  320 }, { 1280, 640 }, { 1920, 960 }
    };

    static const char *windowSizeTitles[] = {
        "640x320", "1280x640", "1920x960"
    };

    static int windowSizeIdx = m_newCfg.graphics.scaleFactor - 1;

    if (ImGui::Combo("Window size", &windowSizeIdx, windowSizeTitles, IM_ARRAYSIZE(windowSizeTitles))) {
        m_newCfg.graphics.scaleFactor = windowSizeIdx + 1;
        m_newCfg.graphics.windowSize = windowSizes[windowSizeIdx];
    }

    ImGui::PushItemWidth(ImGui::GetFontSize() * 10);

    ImGui::ColorEdit3("OFF pixel color", (float *) &m_offColor);
    ImGui::ColorEdit3("ON pixel color", (float *) &m_onColor);

    ImGui::PopItemWidth();

    ImGui::Checkbox("Draw grid", &m_enableGrid);
    markerNotSaved();
}

void Settings::sectionInput() {
    auto drawKeypad = [](const InputLayout &layout) {
        for (int i = 0; i < KEY_COUNT; ++i) {
            const auto &key = layout[(std::size_t) i];

            ImGui::PushStyleColor(ImGuiCol_Button, 0x00);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, 0x00);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, 0x00);

            ImGui::Button(SDL_GetScancodeName(key.first));

            ImGui::PopStyleColor(3);

            if (!utils::divisibleBy(i + 1, 4)) {
                ImGui::SameLine();
            }
        }
    };

    static const char *layouts[] = {
        "Original",
        "Modern"
    };

    ImGui::PushItemWidth(ImGui::GetFontSize() * 8);

    if (ImGui::Combo("Layout", &m_newCfg.input.layoutIdx, layouts, IM_ARRAYSIZE(layouts))) {
        m_newCfg.input.layout = m_newCfg.input.layoutIdx == 0 ? ORIGINAL_LAYOUT : MODERN_LAYOUT;
    }

    ImGui::PopItemWidth();

    drawKeypad(m_newCfg.input.layout);
}

void Settings::sectionSound() {
    ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
    ImGui::InputScalar("Sound frequency (Hz)", ImGuiDataType_U32, &m_newCfg.sound.frequency);
    ImGui::InputDouble("Volume (dB)", &m_newCfg.sound.level, 0.5, 1.0, "%.2f");
    ImGui::PopItemWidth();

    static const char *waveformTitles[] = {
        "Sine",
        "Square",
        "Saw"
    };

    ImGui::Combo("Waveform", (int *) &m_newCfg.sound.waveform, waveformTitles, IM_ARRAYSIZE(waveformTitles));
    ImGui::Checkbox("Enable sound", &m_newCfg.sound.enable);
}

void Settings::sectionUI() {
    ImGui::PushItemWidth(ImGui::GetFontSize() * 7);
    ImGui::Combo("Color Scheme##Style Selector", (int *) &m_newCfg.ui.style, "Dark\0Light\0Purplish\0");
    ImGui::PopItemWidth();
}

void Settings::markerNotSaved() {
    ImGui::SameLine();
    ImGui::TextDisabled("(*)");

    if (ImGui::BeginItemTooltip()) {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted("This setting is not saved to the config file");
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
};
