#define WIN32_LEAN_AND_MEAN
#pragma comment(lib, "opengl32.lib")
#include <windows.h>
#include <glad/glad.h>
#include <glad/glad_wgl.h>
#include <utilities/definitions.hpp>

static int32_t framebuffer_width = 1280;
static int32_t framebuffer_height = 720;

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
            printf("Window resized: %i, %i\n", width, height);
            framebuffer_width = width;
            framebuffer_height = height;
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

int WINAPI 
wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{

#if defined(ALPHA_ENGINE_CONSOLE) && ALPHA_ENGINE_CONSOLE != 0
    AllocConsole();
    freopen_s((FILE**)stdout,   "CONOUT$",  "w", stdout);
    freopen_s((FILE**)stderr,   "CONOUT$",  "w", stderr);
    freopen_s((FILE**)stdin,    "CONIN$",   "r", stdin);
#endif

    WNDCLASSEXW window_class = {};
    window_class.cbSize = sizeof(window_class);
    window_class.lpfnWndProc = &wWindowProcedure;
    window_class.hInstance = GetModuleHandleW(NULL);
    window_class.hIcon = LoadIconA(NULL, IDI_APPLICATION);
    window_class.hCursor = LoadCursorA(NULL, IDC_ARROW);
    window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    window_class.lpszClassName = L"alphaWindowClass";
    RegisterClassExW(&window_class);

#   define CLIENT_WIDTH 1280
#   define CLIENT_HEIGHT 720
    RECT client_rect = { 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT };
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
            hInstance,
            NULL);

    if (window_handle == INVALID_HANDLE_VALUE || window_handle == NULL)
    {
        MessageBoxA(NULL, "Failed to create window.", "Application Error", MB_OK);
        return 0;
    }

    HDC device_context = GetDC(window_handle);
    HGLRC render_context;

    // TODO(Chris): We don't actually have a nice method of handling context creation
    //              failures, so we will want to do something more robust than just
    //              message boxes at some point.
    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pixel_format = ChoosePixelFormat(device_context, &pfd);
    if (pixel_format == 0)
    {
        MessageBoxA(window_handle, "Failed to choose pixel format", "Error", MB_OK | MB_ICONERROR);
        return false;
    }

    if (!SetPixelFormat(device_context, pixel_format, &pfd))
    {
        MessageBoxA(window_handle, "Failed to set pixel format", "Error", MB_OK | MB_ICONERROR);
        return false;
    }

    HGLRC temp_context = wglCreateContext(device_context);
    if (!temp_context)
    {
        MessageBoxA(window_handle, "Failed to create temporary WGL context", "Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    if (!wglMakeCurrent(device_context, temp_context))
    {
        MessageBoxA(window_handle, "Failed to activate temporary WGL context", "Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    if (!gladLoadGL())
    {
        MessageBox(window_handle, "Failed to load OpenGL functions with GLAD", "Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    if (!gladLoadWGL(device_context))
    {
        MessageBoxA(window_handle, "Failed to load WGL functions with GLAD", "Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    if (GLAD_WGL_ARB_create_context)
    {
        int attribs[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
            WGL_CONTEXT_MINOR_VERSION_ARB, 6,
            WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0
        };

        render_context = wglCreateContextAttribsARB(device_context, 0, attribs);

        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(temp_context);

        if (!render_context)
        {
            MessageBoxA(window_handle, "Failed to create modern OpenGL context", "Error", MB_OK | MB_ICONERROR);
            return false;
        }

    }
    else
    {
        MessageBoxA(window_handle, "Failed to create modern OpenGL context", "Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    if (!wglMakeCurrent(device_context, render_context))
    {
        MessageBox(window_handle, "Failed to activate OpenGL render context", "Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    wglSwapIntervalEXT(1);
    glEnable(GL_DEPTH_TEST);
    
    ShowWindow(window_handle, nCmdShow);

    static bool application_running = true;
    while (application_running)
    {

        MSG message = {};
        while (PeekMessageW(&message, 0, 0, 0, PM_REMOVE))
        {

            TranslateMessage(&message);
            DispatchMessageW(&message);

            if (message.message == WM_QUIT)
            {
                application_running = false;
            }

        }

        glViewport(0, 0, framebuffer_width, framebuffer_height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);
        glClearColor(0.105f, 0.11f, 0.1f, 1.0f);

        SwapBuffers(device_context);

    }

    DestroyWindow(window_handle);

    return 0;

}
