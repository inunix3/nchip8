// Copyright (c) 2023-present, eaxecx.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#include <nchip8/ui/disassembler.hpp>
#include <nchip8/imgui.hpp>

#include <cinttypes>
#include <cstddef>
#include <cstdint>

using namespace nchip8::ui;

Disassembler::Disassembler(VM &vm)
    : Window { "Disassembler", ImGuiWindowFlags_AlwaysAutoResize },
      m_vm   { vm } {
}

void Disassembler::body() {
    VMState &vmState = m_vm.state;

    ImGui::TextUnformatted("* The yellow row means that the PC counter is pointing to the same address.");
    ImGui::Text("PC: 0x%04" PRIx16, vmState.pc);
    ImGui::Text("ROM size: %zu bytes", m_vm.state.romSize);

    if (m_vm.state.romSize > 0) {
        ImGui::Text("   lowest address: 0x%04" PRIx16, PROG_OFFSET);
        ImGui::Text("   highest: 0x%04" PRIx16, (std::uint16_t) (PROG_OFFSET + m_vm.state.romSize - 2));
    }

    if (!ImGui::BeginTable("Memory Content", 3, ImGuiTableFlags_BordersOuter)) {
        ImGui::EndTable();
        ImGui::End();

        return;
    }

    ImGui::TableSetupColumn("Offset");
    ImGui::TableSetupColumn("Data");
    ImGui::TableSetupColumn("Disassembled");
    ImGui::TableHeadersRow();

    // ImGuiListClipper is useful for improving performance: if we drew the table without it, even the rows that are
    // not visible (i.e. outside the window viewport or screen) would still be drawn. And since in our case we can
    // draw a table of several thousand rows, this may not be a good thing for us.
    ImGuiListClipper clipper;
    clipper.Begin((int) vmState.romSize / 2);
    while (clipper.Step()) {
        for (int rowN = clipper.DisplayStart; rowN < clipper.DisplayEnd; ++rowN) {
            std::size_t memIdx = PROG_OFFSET + 2 * (std::size_t) rowN;

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("0x%.4" PRIx16, (std::uint16_t) memIdx);

            ImGui::TableSetColumnIndex(1);

            std::uint16_t opcode = *(const uint16_t *const) &vmState.memory[memIdx];
            opcode = utils::swapEndianess(opcode);

            ImGui::Text("0x%.4" PRIx16, opcode);

            if (memIdx == vmState.pc) {
                ImU32 color = ImGui::GetColorU32(ImVec4(1.0, 1.0, 0.0, 0.4f));
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, color);
            }

            ImGui::TableSetColumnIndex(2);

            try {
                ImGui::TextUnformatted(m_vm.disassemble(opcode).c_str());
            } catch (const InvalidOpcode &ex) {
                ImGui::TextUnformatted("<unknown>");
            }
        }
    }

    ImGui::EndTable();
}
