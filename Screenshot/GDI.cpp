#include "pch.h"
#include "Screenshot.h"

#pragma comment(lib,"gdi32.lib")

using BitmapCallback = std::function<void(const void* data, int width, int height)>;

// Blt: [](HDC hscreen, HDC hdc) -> void
template<class Blt>
static bool CaptureImpl(RECT rect, HWND hwnd, const BitmapCallback& callback, const Blt& blt)
{
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    info.bmiHeader.biSizeImage = width * height * 4;

    bool ret = false;
    HDC hscreen = ::GetDC(hwnd);
    HDC hdc = ::CreateCompatibleDC(hscreen);
    void* data = nullptr;
    if (HBITMAP hbmp = ::CreateDIBSection(hdc, &info, DIB_RGB_COLORS, &data, NULL, NULL)) {
        ::SelectObject(hdc, hbmp);
        blt(hscreen, hdc);
        callback(data, width, height);
        ::DeleteObject(hbmp);
        ret = true;
    }
    ::DeleteDC(hdc);
    ::ReleaseDC(hwnd, hscreen);
    return ret;
}


bool CaptureScreen(RECT rect, const BitmapCallback& callback)
{
    int x = rect.left;
    int y = rect.top;
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    return CaptureImpl(rect, nullptr, callback, [&](HDC hscreen, HDC hdc) {
        ::StretchBlt(hdc, 0, 0, width, height, hscreen, x, y, width, height, SRCCOPY);
        });
}

bool CaptureEntireScreen(const BitmapCallback& callback)
{
    int x = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
    int y = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
    int width = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int height = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);
    return CaptureScreen({ x, y, width + x, height + y }, callback);
}

bool CaptureWindow(HWND hwnd, const BitmapCallback& callback)
{
    RECT rect{};
    ::GetWindowRect(hwnd, &rect);

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    return CaptureImpl(rect, hwnd, callback, [&](HDC hscreen, HDC hdc) {
        // BitBlt() can't capture Chrome, Edge, etc. PrintWindow() with PW_RENDERFULLCONTENT can do it.
        //::BitBlt(hdc, 0, 0, width, height, hscreen, 0, 0, SRCCOPY);
        ::PrintWindow(hwnd, hdc, PW_RENDERFULLCONTENT);
        });
}



void TestGDI()
{
    // DIB image is BGRA and upside-down so need to flip y
    CaptureEntireScreen([](const void* data, int w, int h) {
        SaveAsPNG("CaptureEntireScreen.png", w, h, w * 4, data, true);
        });
}
