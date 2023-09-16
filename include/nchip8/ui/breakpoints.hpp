// Copyright (c) 2023-present, eaxecx.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#pragma once

#include "window.hpp"
#include "../breakpoint.hpp"
#include "../imgui.hpp"

namespace nchip8::ui {
    class Breakpoints : public Window {
    public:
        Breakpoints(BreakpointMap &bps);

    private:
        void body() override;

        void popupAddBreakpoint();
        void popupEditBreakpoint();
        inline bool containsOnlyWhitespaces(const std::string_view &str) const;

        BreakpointMap &m_bps;
        Breakpoint m_editableBp;

        // See https://github.com/ocornut/imgui/issues/331
        bool m_openEditBPPopup = false;
    };
}

