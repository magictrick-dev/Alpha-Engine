CMAKE_MINIMUM_REQUIRED(VERSION 3.28)

# Vulkan port... soon?
#FIND_PACKAGE(Vulkan REQUIRED)

PROJECT(paperdreams)

ADD_EXECUTABLE(paperdreams WIN32
    "source/platform/entry.cpp"
    "source/platform/launcher.cpp"
    "source/platform/window.cpp"
    "source/platform/events.cpp"
    "source/platform/filesystem.cpp"
    "source/platform/input.cpp"
    "source/platform/logging.cpp"
    "source/platform/utilities.cpp"
    "source/platform/controller.cpp"
    "source/platform/renderer/context.cpp"
    "source/platform/renderer/texture.cpp"
    "source/platform/renderer/mesh.cpp"
    "source/platform/renderer/shader.cpp"
    "source/platform/renderer/framebuffer.cpp"

    "../vendor/glad/glad.c"
    "../vendor/glad/glad_wgl.c"
    "../vendor/imgui/imgui.cpp"
    "../vendor/imgui/imgui_demo.cpp"
    "../vendor/imgui/imgui_draw.cpp"
    "../vendor/imgui/imgui_tables.cpp"
    "../vendor/imgui/imgui_stdlib.cpp"
    "../vendor/imgui/imgui_widgets.cpp"
    "../vendor/imgui/implot.cpp"
    "../vendor/imgui/implot_demo.cpp"
    "../vendor/imgui/implot_items.cpp"
    "../vendor/imgui/backends/imgui_impl_opengl3.cpp"
    "../vendor/imgui/backends/imgui_impl_win32.cpp"
    "../vendor/imgui/backends/imgui_impl_glfw.cpp"
    "../app_icon.rc"

)

SET(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
SET(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

TARGET_COMPILE_DEFINITIONS(paperdreams PRIVATE 
    PAPERDREAMS_LIBRARY_EXPORTS
    PAPERDREAMS_PEDANTIC_CHECKS
)

TARGET_INCLUDE_DIRECTORIES(paperdreams PUBLIC 
    "include/"
    "source/"
    "../vendor/"
    "../vendor/glfw/include"
    ${Vulkan_INCLUDE_DIRS}
)

TARGET_LINK_LIBRARIES(paperdreams PUBLIC
    Shlwapi.lib
    opengl32.lib
    gdi32.lib
    glfw
    xinput.lib
    ../assimp-vc143-mt
    ${Vulkan_LIBRARIES}
)
