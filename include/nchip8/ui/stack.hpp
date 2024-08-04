// Copyright (c) 2024 inunix3.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#pragma once

#include "window.hpp"
#include "../vm.hpp"

namespace nchip8::ui {
    class Stack : public Window {
    public:
        Stack(VM &vm);

    private:
        void body() override;

        VM &m_vm;
    };
}
