// Copyright (c) 2024 inunix3.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#include <nchip8/ui/ui.hpp>
#include <nchip8/imgui.hpp>
#include <nchip8/main.hpp> // for the VERSION constant
#include <nchip8/utils.hpp>

#include <ImGuiFileDialog.h>

#include <string>

using namespace nchip8::ui;

UI::UI(sdl::Window &window, sdl::Renderer &renderer, VM &vm)
    : m_vm { vm },
      m_breakpoints   { vm.breakpoints },
      m_disassembler  { vm },
      m_instrExecutor { vm },
      m_keypad        { vm },
      m_registers     { vm },
      m_settings      { window, vm, *this },
      m_stack         { vm } {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    m_io = &ImGui::GetIO();
    m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Don't save positions and sizes of windows in INI file
    m_io->IniFilename = nullptr;

    setStyle(vm.cfg.ui.style);
    ImGuiStyle &style = ImGui::GetStyle();

    // Give an old-fashioned feel to the UI
    style.WindowBorderSize = 1;
    style.PopupBorderSize  = 1;
    style.FrameBorderSize  = 1;

    ImGui_ImplSDL2_InitForSDLRenderer(window.Get(), renderer.Get());
    ImGui_ImplSDLRenderer2_Init(renderer.Get());
}

UI::~UI() {
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void UI::update() {
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    input();
    menu();
    windows();

    if (m_showPauseScreen) {
        pauseScreen();
    }

    popups();
}

void UI::showError(const std::string &err) {
    m_currentError = err;
    m_openErrorPopup = true;
}

void UI::render() {
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
}

void UI::setStyle(UIStyle style) {
    switch (style) {
    case UIStyle::DARK:     ImGui::StyleColorsDark();    break;
    case UIStyle::LIGHT:    ImGui::StyleColorsLight();   break;
    case UIStyle::PURPLISH: ImGui::StyleColorsClassic(); break;
    }
}

bool UI::wantCaptureKeyboard() const {
    return m_io->WantCaptureKeyboard;
}

bool UI::quitRequested() const {
    return m_quitRequested;
}

void UI::input() {
    if (ImGui::IsPopupOpen("Error")) {
        return;
    }

    if (m_vm.mode() == VMMode::RUN && m_io->KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_P)) {
        m_vm.setMode(VMMode::PAUSED);

        m_showPauseScreen = true;
    }

    if (!m_showMainMenu) {
        if (m_io->KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_R)) {
            m_vm.reset();
        }

        if (m_io->KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Q)) {
            m_quitRequested = true;
        }

        if (m_io->KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S)) {
            try {
                m_vm.step();
            } catch (const VMError &err) {
                showError(err.what());
            }
        }

        if (m_io->KeyCtrl && m_io->KeyShift && ImGui::IsKeyPressed(ImGuiKey_C)) {
            m_vm.setMode(VMMode::RUN);
        }
    }

    if (m_showPauseScreen && ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        m_vm.setMode(m_vm.prevMode());

        m_showPauseScreen = false;
    }

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) &&
       !ImGui::IsPopupOpen("Error")) {
        ImGui::OpenPopup("Menu");
    }
}

void UI::pauseScreen() {
    constexpr float PADDING = 10.0f;

    const auto *viewport = ImGui::GetMainViewport();
    static auto workPos  = viewport->WorkPos;
    static auto workSize = viewport->WorkSize;
    static ImVec2 pos    = { workPos.x + PADDING, workPos.y + workSize.y - PADDING };
    static ImVec2 pivot  = { 0.0f, 1.0f };

    ImGui::SetNextWindowPos(pos, ImGuiCond_Always, pivot);
    ImGui::SetNextWindowBgAlpha(0.9f);

    auto flags = ImGuiWindowFlags_NoDecoration       | ImGuiWindowFlags_AlwaysAutoResize      | ImGuiWindowFlags_NoNav |
                 ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove;

    if (!ImGui::Begin("Pause screen", nullptr, flags)) {
        ImGui::End();

        return;
    }

    ImGui::TextDisabled("Game has been stopped. Press ESC to resume.");

    ImGui::End();
}

void UI::popups() {
    if (m_openErrorPopup) {
        ImGui::OpenPopup("Error");

        m_openErrorPopup = false;
    }

    if (!ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        return;
    }

    ImGui::TextUnformatted(m_currentError.c_str());
    ImGui::Dummy({ 0, 10 });

    if (!m_vm.cfg.cpu.debugMode) {
        if (ImGui::Button("Exit", { 60, 0 })) {
            ImGui::CloseCurrentPopup();

            m_quitRequested = true;
        }

        ImGui::SameLine();

        if (ImGui::Button("Enter debug mode", { 140, 0 })) {
            ImGui::CloseCurrentPopup();

            m_vm.setMode(VMMode::STEP);
            m_vm.cfg.cpu.debugMode = true;
        }
    } else {
        if (ImGui::Button("Continue", { 70, 0 })) {
            m_vm.setMode(VMMode::RUN);

            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Break", { 70, 0 })) {
            m_vm.setMode(VMMode::STEP);

            ImGui::CloseCurrentPopup();
        }
    }

    ImGui::EndPopup();
}

void UI::menu() {
    auto menuLabel = [](const char *text) {
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(text).x) * 0.5f);

        ImGui::MenuItem(text, nullptr, false, false);
    };

    ImGui::SetNextWindowSize({ 235, 0 });
    if (!ImGui::BeginPopupContextWindow("Menu")) {
        return;
    }

    // See https://github.com/ocornut/imgui/issues/331
    bool openPauseScreen = false;

    menuLabel("CONTROL");

    ImGui::BeginDisabled(m_vm.mode() == VMMode::EMPTY);

    if (ImGui::MenuItem("Pause",   "Ctrl-P")) openPauseScreen = true;
    if (ImGui::MenuItem("Restart", "Ctrl-R")) m_vm.reset();

    if (ImGui::MenuItem("Unload ROM")) {
        m_vm.unload();
        m_showPauseScreen = false;
    }

    ImGui::EndDisabled();

    if (ImGui::MenuItem("Settings"))       m_settings.show = true;
    if (ImGui::MenuItem("About nCHIP-8"))  m_showAbout     = true;
    if (ImGui::MenuItem("Quit", "Ctrl-Q")) m_quitRequested = true;

    if (m_vm.cfg.cpu.debugMode) {
        ImGui::Separator();

        menuLabel("DEBUG");

        ImGui::BeginDisabled(m_vm.mode() != VMMode::STEP);

        if (ImGui::MenuItem("Step",     "Ctrl-S"))       m_vm.step();
        if (ImGui::MenuItem("Continue", "Ctrl-Shift-C")) m_vm.setMode(VMMode::RUN);

        ImGui::EndDisabled();

        if (ImGui::MenuItem("Keypad"))          m_keypad.show        = true;
        if (ImGui::MenuItem("Disassembler"))    m_disassembler.show  = true;
        if (ImGui::MenuItem("Stack"))           m_stack.show         = true;
        if (ImGui::MenuItem("Registers"))       m_registers.show     = true;
        if (ImGui::MenuItem("Breakpoints"))     m_breakpoints.show   = true;
        if (ImGui::MenuItem("Execute Instr."))  m_instrExecutor.show = true;
    }

    ImGui::EndPopup();

    if (openPauseScreen) {
        m_vm.setMode(VMMode::PAUSED);

        ImGui::OpenPopup("Pause screen");
    }
}

void UI::windows() {
    if (m_vm.mode() == VMMode::EMPTY && !m_showMainMenu) {
        m_showMainMenu = true;
    }

    if (m_showMainMenu) mainMenu();
    if (m_showAbout)    about();

    m_settings.render();

    if (m_vm.cfg.cpu.debugMode) {
        m_breakpoints.render();
        m_disassembler.render();

        try {
            m_instrExecutor.render();
        } catch (const VMError &err) {
            showError(err.what());
        }

        m_keypad.render();
        m_registers.render();
        m_stack.render();
    }
}

void UI::mainMenu() {
    ImGui::SetNextWindowPos({ m_io->DisplaySize.x * 0.5f, m_io->DisplaySize.y * 0.5f }, ImGuiCond_Always, { 0.5f,0.5f });

    ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration;
    if (!ImGui::Begin("Main menu", nullptr, flags)) {
        ImGui::End();

        return;
    }

    static ImVec2 btnSize(120, 0);
    static ImVec2 spacing(0, 10);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, spacing);

    if (ImGui::Button("Load ROM", btnSize)) {
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose ROM", ".ch8", ".", 1, nullptr,
                ImGuiFileDialogFlags_Modal);
    }

    static Extension ext = Extension::NONE;

    ImGui::PushItemWidth(btnSize.x);
    ImGui::Combo("##Extension", (int *) &ext, "CHIP-8\0SCHIP 1.1\0");
    ImGui::PopItemWidth();

    if (ImGui::Button("Settings",      btnSize)) m_settings.show = true;
    if (ImGui::Button("About nCHIP-8", btnSize)) m_showAbout     = true;
    if (ImGui::Button("Quit",          btnSize)) m_quitRequested = true;

    ImGui::PopStyleVar();

    auto *fileDialog = ImGuiFileDialog::Instance();

    try {
        if (fileDialog->Display("ChooseFileDlgKey", ImGuiWindowFlags_NoCollapse, { 600, 300 })) {
            if (fileDialog->IsOk()) {
                std::string rom = fileDialog->GetFilePathName();

                if (m_vm.ext() != ext) {
                    m_vm.setExtension(ext);
                }

                m_vm.loadFile(rom);
                m_vm.setMode(VMMode::RUN);

                m_showMainMenu = false;
            }

            fileDialog->Close();
        }
    } catch (const std::runtime_error &err) {
        // This exception can most likely occur due to an I/O error while reading
        // the file (does not exist, bad permissions or something else)

        fileDialog->Close();

        showError(err.what());
    }

    ImGui::End();
}

void UI::about() {
    if (!ImGui::Begin("About nCHIP-8", &m_showAbout, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::End();

        return;
    }

    ImGui::TextUnformatted("nCHIP-8 is a customizable CHIP-8/SUPER-CHIP interpreter with debug capabilities.");

    ImGui::Dummy({ 0, 1 });

    ImGui::TextUnformatted("Written in C++17 with the use of these libraries:");
    ImGui::BulletText("toml11");
    ImGui::BulletText("SDL2");
    ImGui::BulletText("SDL2pp (C++ wrapper over SDL2)");
    ImGui::BulletText("Dear ImGui");
    ImGui::BulletText("ImGuiFileDialog");

    ImGui::Dummy({ 0, 1 });

    ImGui::Text("Version: v%s", VERSION.c_str());
    ImGui::Text("Build date: %s %s", __DATE__, __TIME__);

    ImGui::End();
}
