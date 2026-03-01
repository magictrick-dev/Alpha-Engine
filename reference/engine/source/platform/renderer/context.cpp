#include <platform/renderer/state.hpp>
#include <platform/renderer/context.hpp>
#include <platform/filesystem.hpp>
#include <platform/window.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

bool
pde_renderer_create_context()
{

    GLFWwindow *window = (GLFWwindow*)pde_window_get_window_handle();

    // OpenGL.
    glfwMakeContextCurrent(window);
    if (!gladLoadGL()) return false;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);

    // Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();

    ImFontConfig font_config = {};
    font_config.OversampleH = 4;
    font_config.OversampleV = 4;

    std::string font_path = pde_filesystem_resolve("./resources/fonts/FiraCode-SemiBold.ttf");
    if (font_path == "") return false;
    io.Fonts->AddFontFromFileTTF(font_path.c_str(), 14.0f, &font_config);

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.098f, 0.098f, 0.098f, 0.8f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border]                 = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.00f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.58f, 0.47f, 0.34f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.58f, 0.47f, 0.34f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.58f, 0.47f, 0.34f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.27f, 0.25f, 0.21f, 1.00f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.58f, 0.47f, 0.34f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.58f, 0.47f, 0.34f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.141f, 0.141f, 0.141f, 1.000f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.42f, 0.36f, 0.29f, 1.00f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.42f, 0.36f, 0.29f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.58f, 0.47f, 0.34f, 1.00f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.58f, 0.47f, 0.34f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.58f, 0.47f, 0.34f, 1.00f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.75f, 0.52f, 0.23f, 1.00f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.75f, 0.52f, 0.23f, 1.00f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.58f, 0.47f, 0.34f, 1.00f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.27f, 0.25f, 0.21f, 1.00f);
    colors[ImGuiCol_TabSelected]            = ImVec4(0.580f, 0.471f, 0.341f, 1.000f);
    colors[ImGuiCol_TabSelectedOverline]    = ImVec4(0.42f, 0.36f, 0.29f, 1.00f);
    colors[ImGuiCol_TabDimmed]              = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_TabDimmedSelected]      = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_TabDimmedSelectedOverline]  = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_DockingPreview]         = ImVec4(0.74f, 0.52f, 0.23f, 1.00f);
    colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextLink]               = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    colors[ImGuiCol_TreeLines]              = ImVec4(0.430f, 0.430f, 0.500f, 1.000f);

    style.WindowRounding        = 4;
    style.TabRounding           = 4;
    style.FrameRounding         = 8;
    style.GrabRounding          = 4;
    style.ChildRounding         = 6;

    style.TabBarOverlineSize    = 0;

    style.TreeLinesSize         = 3;
    style.TreeLinesRounding     = 5;

    style.FrameBorderSize       = 0.0f;
    style.ChildBorderSize       = 2.0f;
    style.TabBarBorderSize      = 4.0f;

    style.DockingSeparatorSize  = 4.0f;

    style.CellPadding           = { 5.0f, 5.0f };
    style.FramePadding          = { 4.0f, 5.0f };
    style.ItemSpacing           = { 3.0f, 3.0f };

    style.WindowMenuButtonPosition = ImGuiDir_Right;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430 core");

    return true;

}

bool 
pde_renderer_destroy_context()
{

    return true;

}

void 
pde_renderer_swap_buffers()
{

    GLFWwindow *window = (GLFWwindow*)pde_window_get_window_handle();
    glfwSwapBuffers(window);

}

void 
pde_renderer_begin_frame(glm::vec2 framebuffer, glm::vec4 color)
{

    glViewport(0, 0, framebuffer.x, framebuffer.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(color.r, color.g, color.b, color.a);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(0, NULL, ImGuiDockNodeFlags_PassthruCentralNode, NULL);

}

void 
pde_renderer_finalize_frame()
{

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

