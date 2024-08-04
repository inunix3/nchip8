// Copyright (c) 2024 inunix3.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#include <nchip8/ui/breakpoints.hpp>
#include <nchip8/vm.hpp>

#include <cinttypes>
#include <cstddef>
#include <cstdint>

using namespace nchip8::ui;

Breakpoints::Breakpoints(BreakpointMap &bps)
    : Window { "Breakpoints", ImGuiWindowFlags_AlwaysAutoResize },
      m_bps  { bps } {
}

void Breakpoints::body() {
    if (ImGui::Button("Add...")) {
        ImGui::OpenPopup("Add breakpoint");
    }

    if (m_openEditBPPopup) {
        ImGui::OpenPopup("Edit breakpoint");

        m_openEditBPPopup = false;
    }

    popupAddBreakpoint();
    popupEditBreakpoint();

    ImGui::SameLine();
    ImGui::BeginDisabled(m_bps.has(PROG_OFFSET));

    if (ImGui::Button(("Break on start (" + utils::toHexPrefixed(PROG_OFFSET) + ")").c_str())) {
        m_bps.add({ "start", PROG_OFFSET });
    }

    ImGui::EndDisabled();

    ImGui::SameLine();
    ImGui::BeginDisabled(m_bps.empty());

    if (ImGui::Button("Remove all")) {
        m_bps.clear();
    }

    ImGui::EndDisabled();

    if (!ImGui::BeginTable("Breakpoints", 3, ImGuiTableFlags_Borders)) {
        ImGui::EndTable();
        ImGui::End();

        return;
    }

    ImGui::TableSetupColumn("Name");
    ImGui::TableSetupColumn("Action");
    ImGui::TableSetupColumn("Position");
    ImGui::TableHeadersRow();

    static const float nameCellWidth = ImGui::CalcTextSize("A LONG BREAKPOINT NAME").x;
    static const float posCellWidth  = ImGui::CalcTextSize("0x0000").x;

    // We can remove m_bps.directly in the loop, but it makes the code a bit messy...
    static std::stack<std::uint16_t> bpsPendingDelete;

    // This counter is needed for generating unique IDs for buttons.
    // See: https://github.com/ocornut/imgui/issues/74
    //  or: https://github.com/ocornut/imgui/blob/master/docs/FAQ.md#q-about-the-id-stack-system
    int actionBtnsID = 0;

    for (auto &it : m_bps) {
        auto &bp = it.second;

        ImGui::TableNextRow();

        if (ImGui::TableSetColumnIndex(0)) {
            ImGui::PushItemWidth(nameCellWidth);
            ImGui::TextUnformatted(bp.name.c_str());
            ImGui::PopItemWidth();
        }

        if (ImGui::TableSetColumnIndex(1)) {
            ImGui::PushID(actionBtnsID);

            if (ImGui::SmallButton("Remove")) bpsPendingDelete.push(bp.offset);

            ImGui::SameLine();

            if (ImGui::SmallButton("Edit")) {
                m_openEditBPPopup = true;

                m_editableBp = bp;
            }

            ImGui::PopID();

            ++actionBtnsID;
        }

        if (ImGui::TableSetColumnIndex(2)) {
            ImGui::PushItemWidth(posCellWidth);
            ImGui::Text("0x%04" PRIx16, bp.offset);
            ImGui::PopItemWidth();
        }
    }

    while (!bpsPendingDelete.empty()) {
        m_bps.remove(bpsPendingDelete.top());
        bpsPendingDelete.pop();
    }

    ImGui::EndTable();
}

void Breakpoints::popupAddBreakpoint() {
    if (!ImGui::BeginPopupModal("Add breakpoint", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        return;
    }

    static std::string name;
    static uint16_t offset = 0;

    ImGui::InputText("Name", &name);
    ImGui::InputScalar("Position", ImGuiDataType_U16, &offset, nullptr, nullptr, "%04" PRIx16);

    ImGui::Dummy(ImVec2(0, 10));

    if (m_bps.has(name)) {
        ImGui::TextUnformatted("A breakpoint with that name already exists!");
    } else if (m_bps.has(offset)) {
        ImGui::TextUnformatted("A breakpoint already exists on this offset!");
    }

    ImGui::BeginDisabled(containsOnlyWhitespaces(name));

    if (ImGui::Button("Add", ImVec2(60, 0)) && !containsOnlyWhitespaces(name)) {
        m_bps.add({ std::string(name), offset });

        name.clear();
        offset = 0;

        ImGui::CloseCurrentPopup();
    }

    ImGui::EndDisabled();
    ImGui::SameLine();

    if (ImGui::Button("Cancel", ImVec2(60, 0))) {
        name.clear();
        offset = 0;

        ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
}

void Breakpoints::popupEditBreakpoint() {
    if (!ImGui::BeginPopupModal("Edit breakpoint", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        return;
    }

    static std::string   name;
    static std::uint16_t offset = 0;

    if (name.empty()) {
        name = m_editableBp.name;
    }

    if (offset == 0) {
        offset = m_editableBp.offset;
    }

    ImGui::InputText("Name", &name);
    ImGui::InputScalar("Position", ImGuiDataType_U16, &offset, nullptr, nullptr, "%04" PRIx16);

    ImGui::Dummy(ImVec2(0, 10));

    if (name != m_editableBp.name && m_bps.has(name)) {
        ImGui::TextUnformatted("A breakpoint with that name already exists!");
    } else if (offset != m_editableBp.offset && m_bps.has(offset)) {
        ImGui::TextUnformatted("A breakpoint already exists on this offset!");
    }

    ImGui::BeginDisabled(containsOnlyWhitespaces(name));

    if (ImGui::Button("Save", ImVec2(60, 0))) {
        m_bps.remove(m_editableBp.offset);
        m_bps.add({ name, offset });

        name.clear();
        offset = 0;

        ImGui::CloseCurrentPopup();
    }

    ImGui::EndDisabled();
    ImGui::SameLine();

    if (ImGui::Button("Cancel", ImVec2(60, 0))) {
        name.clear();
        offset = 0;

        ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
}

bool Breakpoints::containsOnlyWhitespaces(const std::string_view &str) const {
    return std::all_of(str.begin(), str.end(), [](unsigned char c){ return std::isspace(c); });
}
