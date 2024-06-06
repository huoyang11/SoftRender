#include "window.h"

#include <string>
#include <assert.h>

#include <Windows.h>

struct window_instance {
    HDC hDC;
    HWND hwnd;
    HINSTANCE hInstance;
};

LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_SIZE:
        {
            int width = LOWORD(lParam);  // Macro to get the low-order word.
            int height = HIWORD(lParam); // Macro to get the high-order word.
        }
        break;
        default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

window::window(uint16_t w,uint16_t h) : m_width(w),m_height(h)
{
    struct window_instance *instance = new window_instance;
    // Register the window class.
    const char CLASS_NAME[]  = "soft render window";

    HINSTANCE hInstance = GetModuleHandle(0);
    WNDCLASS wc = { };
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        "Learn to Program Windows",     // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, w, h,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        throw std::string("create windows error!");
    }

    ShowWindow(hwnd, SW_SHOWNORMAL);

    instance->hDC = GetDC(hwnd);
    instance->hInstance = hInstance;
    instance->hwnd = hwnd;

    this->instance = instance;
}

window::~window()
{

}

void window::run()
{
    MSG message;
    while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
}

void window::set_image(const image& img)
{
    uint16_t img_height = img.get_height();
    uint16_t img_widht = img.get_widht();
    assert(img.get_height() <= m_height);
    assert(img.get_widht() <= m_width);

    //for (int i = 0;i < img_widht;i++) {
    //    for (int j = 0;j < img_height;j++) {
    //        color r = img.get_color(i,j);
    //        SetPixel(((struct window_instance *)instance)->hDC,i,j,RGB(r.rgba[0],r.rgba[1],r.rgba[2]));            
    //    }
    //}

    //to bgra
    int size = img.width * img.height;
    color* data = (color*)calloc(1,sizeof(color) * size);
    for (int i = 0; i < size; i++) {
        (data + i)->rgba[2] = (img.data + i)->rgba[0];
        (data + i)->rgba[1] = (img.data + i)->rgba[1];
        (data + i)->rgba[0] = (img.data + i)->rgba[2];
    }

    HDC hWndDc =((struct window_instance*)instance)->hDC;
    HBITMAP hBitmap = ::CreateBitmap(img_widht, img_height, 1, 32, data);
    HDC hMemDc = ::CreateCompatibleDC(hWndDc);
    HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDc, hBitmap);
    BitBlt(hWndDc, 0, 0, img_widht, img_height, hMemDc, 0, 0, SRCCOPY);

    SelectObject(hMemDc, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hMemDc);
    //ReleaseDC(((struct window_instance*)instance)->hwnd, hWndDc);
    free(data);
}