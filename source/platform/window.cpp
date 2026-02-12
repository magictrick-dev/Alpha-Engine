#include <platform/window.hpp>
#include <platform/rtdispatcher.hpp>

// -------------------------------------------------------------------------------------------------
// HWND / Win32 Window
// -------------------------------------------------------------------------------------------------

#if defined(_WIN32)
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>

LRESULT CALLBACK
wWindowProcedure(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{

    HRESULT result;

    switch (message)
    {

        case WM_SIZE:
        {
            int32_t width = LOWORD(lParam);
            int32_t height = HIWORD(lParam);
            rtdispatcher_push_event({
                .type = RTEventType_WindowResize,
                .window_resize = {
                    .width = width,
                    .height = height,
                },
            });
        } break;

        case WM_CLOSE:
        {
            PostQuitMessage(0);
        } break;

        default:
        {
            result = DefWindowProcW(window, message, wParam, lParam);
        } break;

    }

    return result;

}

struct Window::Implementation
{

    HWND window_handle = NULL;
    HDC device_context = NULL;

};

Window::
Window(int32_t width, int32_t height, const std::string &title)
    : impl(std::make_unique<Implementation>())
{

    WNDCLASSEXW window_class = {};
    window_class.cbSize         = sizeof(window_class);
    window_class.lpfnWndProc    = &wWindowProcedure;
    window_class.hInstance      = GetModuleHandleW(NULL);
    window_class.hIcon          = LoadIconA(NULL, IDI_APPLICATION);
    window_class.hCursor        = LoadCursorA(NULL, IDC_ARROW);
    window_class.hbrBackground  = (HBRUSH)GetStockObject(BLACK_BRUSH);
    window_class.lpszClassName  = L"alphaWindowClass";
    RegisterClassExW(&window_class);

    RECT client_rect = { 0, 0, width, height };
    AdjustWindowRect(&client_rect, WS_OVERLAPPEDWINDOW, FALSE);
    int32_t actual_width = client_rect.right - client_rect.left;
    int32_t actual_height = client_rect.bottom - client_rect.top;

    HWND window_handle = CreateWindowExW(NULL,
            window_class.lpszClassName,
            L"Alpha Engine",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            actual_width, actual_height,
            NULL,
            NULL,
            window_class.hInstance,
            NULL);

    if (window_handle != INVALID_HANDLE_VALUE || window_handle != NULL)
    {
        impl->window_handle = window_handle;
        impl->device_context = GetDC(window_handle);
    }

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

void Window::
show()
{
    ShowWindow(impl->window_handle, SW_SHOWNORMAL);
}

void Window::
poll_events()
{

}

void Window::
swap_buffers()
{

}

bool Window::
should_close() const
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return false;
}

bool Window::
is_valid() const
{
    bool valid_handle = (impl->window_handle != NULL);
    return valid_handle;
}

int32_t Window::
get_width() const
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return 0;
}

int32_t Window::
get_height() const
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return 0;
}

int32_t Window::
get_framebuffer_width() const
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return 0;
}

int32_t Window::
get_framebuffer_height() const
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return 0;
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

void Window::
show()
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return;
}

void Window::
poll_events()
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return;
}

void Window::
swap_buffers()
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return;
}

bool Window::
should_close() const
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return true;
}

bool Window::
is_valid() const
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return false;
}

int32_t Window::
get_width() const
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return 0;
}

int32_t Window::
get_height() const
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return 0;
}

int32_t Window::
get_framebuffer_width() const
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return 0;
}

int32_t Window::
get_framebuffer_height() const
{
    ALPHA_ASSERT_NO_IMPLEMENTATION();
    return 0;
}

#endif
