// Copyright (c) 2023-present, eaxecx.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#include <nchip8/ui/instr_executor.hpp>
#include <nchip8/imgui.hpp>

#include <cinttypes>
#include <cstddef>
#include <cstdint>

using namespace nchip8::ui;

InstrExecutor::InstrExecutor(VM &vm)
    : Window { "Instruction Executor", ImGuiWindowFlags_AlwaysAutoResize },
      m_vm   { vm } {
}

void InstrExecutor::body() {
    static std::uint16_t opcode = 0;

    ImGui::InputScalar("Opcode", ImGuiDataType_U16, &opcode, nullptr, nullptr, "%04" PRIx16);

    auto instrKind = m_vm.tryDecodeOpcode(opcode);
    ImGui::Text("Instruction: %s", instrKind.has_value() ? instrKindToString(instrKind.value()).c_str() : "UNKNOWN");

    OperandMap ops(opcode);
    ImGui::SeparatorText("Operands");

    if (ImGui::BeginTable("Operands", 2)) {
        ImGui::TableSetupColumn("First");
        ImGui::TableSetupColumn("Second");

        ImGui::TableNextRow();

        if (ImGui::TableSetColumnIndex(0)) ImGui::Text("X:       0x%02" PRIx8,  ops.x);
        if (ImGui::TableSetColumnIndex(0)) ImGui::Text("Y:       0x%02" PRIx8,  ops.y);
        if (ImGui::TableSetColumnIndex(0)) ImGui::Text("Address: 0x%03" PRIx16, ops.addr);
        if (ImGui::TableSetColumnIndex(1)) ImGui::Text("4-bit:   0x%01" PRIx8,  ops.imm1);
        if (ImGui::TableSetColumnIndex(1)) ImGui::Text("8-bit:   0x%02" PRIx16, ops.imm2);
        if (ImGui::TableSetColumnIndex(1)) ImGui::Text("12-bit:  0x%03" PRIx16, ops.imm3);
    }

    ImGui::EndTable();

    ImGui::BeginDisabled(!instrKind.has_value());

    if (ImGui::Button("Execute")) {
        m_vm.execInstr(opcode);
    }

    ImGui::EndDisabled();
    ImGui::SameLine();

    if (ImGui::Button("Close")) {
        show = false;

        opcode = 0;
        instrKind.reset();
    }
}
