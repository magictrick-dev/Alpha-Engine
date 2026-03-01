#pragma once
#include <cstdint>

enum WindowFlags : uint32_t
{

    WindowFlags_DisableResize       = (1UL << 0),
    WindowFlags_ShowWindow          = (1UL << 2),
    WindowFlags_MaximizeWindow      = (1UL << 3),
    WindowFlags_CaptureCursor       = (1UL << 4),

    WindowFlags_DisableOpenGL       = (1UL << 31), // For Vulkan support.

    WindowFlags_Default = (WindowFlags_ShowWindow & WindowFlags_MaximizeWindow),

};
      
bool pde_window_create_window(int32_t width, int32_t height, const char *name, WindowFlags flags);
void pde_window_destroy_window();
void pde_window_poll_events();
void pde_window_cursor_capture(bool enabled);
void pde_window_show(bool enabled);
int32_t pde_window_get_width();
int32_t pde_window_get_height();
int32_t pde_window_get_framebuffer_width();
int32_t pde_window_get_framebuffer_height();
int32_t pde_window_get_close_requests();
bool pde_window_is_minimized();
bool pde_window_is_maximized();
bool pde_window_is_focused();
bool pde_window_is_visible();
bool pde_window_should_close();
void* pde_window_get_window_handle();
