// Copyright (c) 2024 inunix3.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#include "application.hpp"
#include "config.hpp"
#include "display.hpp"
#include "sdl.hpp"
#include "ui/ui.hpp"
#include "vm.hpp"

#include <string>

namespace nchip8 {
    inline const std::string VERSION = "1.0";

    class MainApplication : public Application {
    public:
        MainApplication();

        void update() override;
        void render() override;
        void deinit() override;

    private:
        Config readConfig();

        Config m_cfg;
        sdl::Window m_window;
        sdl::Renderer m_renderer;
        Display m_display;
        VM m_vm;
        ui::UI m_ui;
    };
}
