#pragma once
#include <string>
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/implot.h>

class EditorInterface
{

    public:
        inline EditorInterface(std::string window_title, uint32_t window_flags = 0) 
            : window_title(window_title), window_flags(window_flags) { };

        inline bool is_shown() const { return window_visible; }

        inline void hide_window() { window_visible = false; }
        inline void show_window() { window_visible = true; }
        inline void toggle_window() { window_visible = !window_visible; }

        inline virtual void update() { };

        inline void show()
        {

            if (!window_visible) return;

            display_prewindow();
            ImGui::Begin(window_title.c_str(), &window_visible, window_flags | ImGuiWindowFlags_MenuBar);

                if (ImGui::BeginMenuBar())
                {
                    display_menubar();
                    ImGui::EndMenuBar();
                }

                display_contents();
            ImGui::End();
            display_postwindow();
            
        }

        inline void main_menu_entry()
        {
            ImGui::MenuItem(window_title.c_str(), NULL, &window_visible);
        }

    private:
        inline virtual void display_contents() = 0;
        inline virtual void display_menubar() = 0;
        inline virtual void display_prewindow() { }
        inline virtual void display_postwindow() { }
        
    private:
        bool window_visible = true;
        uint32_t window_flags;
        std::string window_title;

};

