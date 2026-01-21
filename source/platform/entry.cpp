#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <utilities/definitions.hpp>

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

    }

    DestroyWindow(window_handle);

    return 0;

}
