#pragma once
#include "imgui/imgui.h"
#include <application/editor/editor_interface.hpp>
#include <platform/logging.hpp>

class ConsoleEditor : public EditorInterface
{

    public:
        inline ConsoleEditor() : EditorInterface("Console") {} 


    private:
        inline virtual void display_contents() override;
        inline virtual void display_menubar() override;
        inline virtual void display_prewindow() override;
        inline virtual void display_postwindow() override;

};

void ConsoleEditor::
display_prewindow()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
}

void ConsoleEditor::
display_postwindow()
{
    ImGui::PopStyleVar();
}

void ConsoleEditor::
display_contents()
{

    static int last_log_count = 0;
    auto logs = pde_logging_get_logs();

    if (ImGui::BeginTable("ConsoleLogs", 3,
                          ImGuiTableFlags_RowBg |
                          ImGuiTableFlags_SizingStretchProp |
                          ImGuiTableFlags_BordersInnerV |
                          ImGuiTableFlags_Resizable |
                          ImGuiTableFlags_ScrollY))
    {
        ImGui::TableSetupColumn("Origin");
        ImGui::TableSetupColumn("Severity");
        ImGui::TableSetupColumn("Message");
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin(static_cast<int>(logs.size()));
        while (clipper.Step())
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
            {
                const auto& log = logs[i];
                ImGui::TableNextRow();

                // Origin
                ImGui::TableSetColumnIndex(0);
                ImGui::Spacing();
                ImGui::TextUnformatted(to_string(log.origin).c_str());

                // Severity (colored)
                ImGui::TableSetColumnIndex(1);
                {
                    ImVec4 color;
                    switch (log.severity)
                    {
                        case MessageSeverityType_Debug:    color = ImVec4(0.5f, 0.5f, 0.5f, 1.0f); break; // Gray
                        case MessageSeverityType_Info:     color = ImVec4(0.2f, 0.7f, 1.0f, 1.0f); break; // Blue
                        case MessageSeverityType_Warning:  color = ImVec4(1.0f, 0.8f, 0.2f, 1.0f); break; // Yellow
                        case MessageSeverityType_Critical: color = ImVec4(1.0f, 0.2f, 0.2f, 1.0f); break; // Bright red
                        case MessageSeverityType_Error:    color = ImVec4(0.9f, 0.1f, 0.1f, 1.0f); break; // Darker red
                        default:                           color = ImVec4(1,1,1,1); break;
                    }
                    ImGui::TextColored(color, "%s", to_string(log.severity).c_str());
                }

                // Message
                ImGui::TableSetColumnIndex(2);
                ImGui::TextUnformatted(log.message.c_str());
            }
        }

        // Auto-scroll if new logs arrived and we’re already at the bottom
        if (logs.size() > last_log_count)
        {
            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 5.0f)
                ImGui::SetScrollHereY(1.0f);
            last_log_count = static_cast<int>(logs.size());
        }

        ImGui::EndTable();
    }

}

void ConsoleEditor::
display_menubar()
{

}
