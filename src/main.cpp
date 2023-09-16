// Copyright (c) 2023-present, eaxecx.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#include <nchip8/main.hpp>
#include <nchip8/config.hpp>
#include <nchip8/sdl.hpp>
#include <nchip8/vm.hpp>

#include <libconfig.h++>

// Needed for getting an absolute path to the HOME
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <optional>

using namespace nchip8;
namespace fs = std::filesystem;

MainApplication::MainApplication()
    : m_cfg { readConfig() },
      m_window { "nCHIP-8 v" + VERSION, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_cfg.graphics.windowSize.x,
          m_cfg.graphics.windowSize.y, SDL_WINDOW_ALLOW_HIGHDPI },
      m_renderer { m_window, -1, SDL_RENDERER_ACCELERATED },
      m_display { m_renderer, m_cfg.graphics.scaleFactor },
      m_vm { m_display, m_cfg },
      m_ui { m_window, m_renderer, m_vm } {
    std::srand(m_cfg.cpu.rngSeed);

    m_display.offColor = m_cfg.graphics.offColor;
    m_display.onColor  = m_cfg.graphics.onColor;
}

void MainApplication::update() {
    auto handleEvents = [this]() {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);

            if (event.type == SDL_QUIT || (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)) {
                m_quit = true;
            } else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                if (!m_ui.wantCaptureKeyboard()) {
                    m_vm.updateInputTable(event);
                }
            }
        }
    };

    handleEvents();

    try {
        m_vm.update();
    } catch (const VMError &err) {
        m_ui.showError(err.what());
    }

    m_ui.update();

    if (m_ui.quitRequested()) {
        m_quit = true;
    }
}

void MainApplication::render() {
    m_renderer.SetDrawColor(0x0);
    m_renderer.Clear();

    m_display.draw();
    m_ui.render();

    m_renderer.Present();
}

Config MainApplication::readConfig() {
    auto getHomeDirPath = []() -> std::optional<std::string> {
        const char *home = std::getenv("HOME");

        if (!home) {
            struct passwd *pw = getpwuid(getuid());

            if (pw) {
                home = pw->pw_dir;
            }
        }

        return home ? std::make_optional(home) : std::nullopt;
    };

    std::string path;
    auto homeDir = getHomeDirPath();

    if (!homeDir) {
        path += "./";
        path += CONFIG_FILENAME;

        std::cerr << "warning: cannot get path to the HOME directory. "
                  << "Config file will be saved in the current directory.\n";
    } else {
        path = homeDir.value() + '/' + CONFIG_FILENAME;
    }

    if (!fs::exists(path)) {
        Config cfg;

        // Write config with default settings
        cfg.writeFile(path);
        cfg.savePath = path;

        return cfg;
    }

    return Config(path);
}

int main() {
    try {
        sdl::SDL sdl(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

        MainApplication app;
        app.run();
    } catch (const libconfig::SettingNotFoundException &err) {
        std::cerr << "error: setting " << err.getPath() << " not found\n";
    } catch (const libconfig::SettingNameException &err) {
        std::cerr << "error: invalid setting name " << err.getPath() << " (already exists or has invalid characters)\n";
    } catch (const libconfig::SettingTypeException &err) {
        std::cerr << "error: setting " << err.getPath() << " has invalid type\n";
    } catch (const libconfig::ParseException &err) {
        std::cerr << "error: cannot parse config file: " << err.getLine() << ": " << err.getError() << '\n';
    } catch (const libconfig::FileIOException &err) {
        std::cerr << "error: cannot read/write config file due to I/O error\n";
    } catch (const std::runtime_error &err) {
        std::cerr << "error: " << err.what() << '\n';
    }

    return EXIT_SUCCESS;
}
