#if defined(_WIN32)
#   pragma comment(lib, "opengl32.lib")
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#   include <print>
#   include <platform/window.hpp>
#   include <platform/rtdispatcher.hpp>
#   include <utilities/definitions.hpp>

int WINAPI 
wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{

#if defined(ALPHA_ENGINE_CONSOLE) && ALPHA_ENGINE_CONSOLE != 0
    AllocConsole();
    freopen_s((FILE**)stdout,   "CONOUT$",  "w", stdout);
    freopen_s((FILE**)stderr,   "CONOUT$",  "w", stderr);
    freopen_s((FILE**)stdin,    "CONIN$",   "r", stdin);
#endif

    Window platform_window = Window(1280, 720, "Alpha Engine");
    platform_window.show();

    static bool application_running = true;
    while (application_running)
    {

        // Poll platform events.
        platform_window.poll_events();
        if (platform_window.should_close())
        {
            application_running = false;
            break;
        }

        rtdispatcher_swap_queues();
        while (!rtdispatcher_read_queue_is_empty())
        {
            RTEvent *current_event = rtdispatcher_get_current_event();
            switch (current_event->type)
            {

                case RTEventType_WindowResize:
                {
                    int32_t width = current_event->window_resize.width;
                    int32_t height = current_event->window_resize.width;
                    std::println("Window Resize: {}, {}", width, height);
                } break;

                default:
                {
                    // Pass, no implementation.
                } break;

            }
            rtdispatcher_pop_current_event();
        }

        platform_window.swap_buffers();

    }

    platform_window.close();

    return 0;

}

#endif 
/** No touchy.
#if defined(__unix__)
#   include <iostream>
#   include <X11/Xlib.h>

static bool quited = false;

void on_delete(Display * display, Window window)
{
    XDestroyWindow(display, window);
    quited = true;
}

int 
main(int argc, char **argv)
{

    Display * display = XOpenDisplay(NULL);
    if (NULL == display) {
        fprintf(stderr, "Failed to initialize display");
        return EXIT_FAILURE;
    }

    Window root = DefaultRootWindow(display);
    if (None == root) {
        fprintf(stderr, "No root window found");
        XCloseDisplay(display);
        return EXIT_FAILURE;
    }

    Window window = XCreateSimpleWindow(display, root, 0, 0, 800, 600, 0, 0, 0xffffffff);
    if (None == window) {
        fprintf(stderr, "Failed to create window");
        XCloseDisplay(display);
        return EXIT_FAILURE;
    }

    XMapWindow(display, window);
    XStoreName(display, window, "Alpha Engine");

    Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, & wm_delete_window, 1);

    XEvent event;
    while (!quited) {
        XNextEvent(display, &event);

        switch(event.type) {
        case ClientMessage:
            if(event.xclient.data.l[0] == wm_delete_window) {
                on_delete(event.xclient.display, event.xclient.window);
            }
            break;
        }
    }

    XCloseDisplay(display);

    return 0;
}

#endif
**/
