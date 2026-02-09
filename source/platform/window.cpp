#include <platform/window.hpp>

// -------------------------------------------------------------------------------------------------
// HWND / Win32 Window
// -------------------------------------------------------------------------------------------------

#if defined(_WIN32)
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>

struct Window::Implementation
{

};

Window::
Window(int32_t width, int32_t height, const std::string &title)
    : impl(std::make_unique<Implementation>())
{
  
}

Window::
~Window()
{

}

NativeHandle Window::
get_native_handle() const
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return { nullptr, nullptr };
}

void Window::
set_size(uint32_t width, uint32_t height)
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return;
}

void Window::
set_title(const std::string &title)
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return;
}

bool Window::
should_close() const
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return;
}

int32_t Window::
get_width() const
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return;
}

int32_t Window::
get_height() const
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return;
}

int32_t Window::
get_framebuffer_width() const
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return;
}

int32_t Window::
get_framebuffer_height() const
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return;
}

#endif

// -------------------------------------------------------------------------------------------------
// X11 / Linux Window
// -------------------------------------------------------------------------------------------------

#if defined(__unix__)
#   include <X11/Xlib.h>

struct Window::Implementation
{

};

Window::
Window(int32_t width, int32_t height, const std::string &title)
    : impl(std::make_unique<Implementation>())
{
  
}

Window::
~Window()
{

}

NativeHandle Window::
get_native_handle() const
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return { nullptr, nullptr };
}

void Window::
set_size(uint32_t width, uint32_t height)
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return;
}

void Window::
set_title(const std::string &title)
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return;
}

bool Window::
should_close() const
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return;
}

int32_t Window::
get_width() const
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return;
}

int32_t Window::
get_height() const
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return;
}

int32_t Window::
get_framebuffer_width() const
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return;
}

int32_t Window::
get_framebuffer_height() const
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return;
}

#endif
