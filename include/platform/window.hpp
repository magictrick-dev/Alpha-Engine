#pragma once
#include <utilities/definitions.hpp>
#include <string>

#define ALPHA_WINDOW_TITLE_MAX 256

struct NativeHandle
{
    void *window_handle = nullptr;
    void *display_handle = nullptr;
};

class Window
{

    public:
                        Window(int32_t width, int32_t height, const std::string &title);
                        Window(const Window &) = delete;
                       ~Window();

        Window&         operator=(const Window &) = delete;

        NativeHandle    get_native_handle() const;

        void            set_size(uint32_t width, uint32_t height);
        void            set_title(const std::string &title);

        bool            should_close() const;

        int32_t         get_width() const;
        int32_t         get_height() const;
        int32_t         get_framebuffer_width() const;
        int32_t         get_framebuffer_height() const;


    private:
        struct Implementation;
        std::unique_ptr<Implementation> impl;

};

