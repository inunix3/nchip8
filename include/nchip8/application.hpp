// Copyright (c) 2023-present, eaxecx.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#pragma once

#include <cstdint>

namespace nchip8 {
    class Application {
    public:
        virtual ~Application();

        void run();
        virtual void update();
        virtual void render();
        virtual void deinit();

    protected:
        bool m_quit = false;
    };
}
