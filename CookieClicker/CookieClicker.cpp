#include <windows.h>
#include <cstdio>
#include <regex>
#include <vector>
#include <algorithm>
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"winmm.lib")

// Cookie Clicker ( http://orteil.dashnet.org/cookieclicker/ ) 連打ツール
// golden cookie 自動クリック機能搭載。


// thanks: http://www.cplusplus.com/forum/lounge/17053/

void LeftClick()
{
    INPUT    Input={0};													// Create our input.

    Input.type        = INPUT_MOUSE;									// Let input know we are using the mouse.
    Input.mi.dwFlags  = MOUSEEVENTF_LEFTDOWN;							// We are setting left mouse button down.
    SendInput( 1, &Input, sizeof(INPUT) );								// Send the input.

    ZeroMemory(&Input,sizeof(INPUT));									// Fills a block of memory with zeros.
    Input.type        = INPUT_MOUSE;									// Let input know we are using the mouse.
    Input.mi.dwFlags  = MOUSEEVENTF_LEFTUP;								// We are setting left mouse button up.
    SendInput( 1, &Input, sizeof(INPUT) );								// Send the input.
}

void SetMousePosition(const POINT& mp)
{
    long fScreenWidth	    = GetSystemMetrics( SM_CXSCREEN ) - 1; 
    long fScreenHeight	    = GetSystemMetrics( SM_CYSCREEN ) - 1; 

    // http://msdn.microsoft.com/en-us/library/ms646260(VS.85).aspx
    // If MOUSEEVENTF_ABSOLUTE value is specified, dx and dy contain normalized absolute coordinates between 0 and 65,535.
    // The event procedure maps these coordinates onto the display surface.
    // Coordinate (0,0) maps onto the upper-left corner of the display surface, (65535,65535) maps onto the lower-right corner.
    float fx		        = mp.x * ( 65535.0f / fScreenWidth  );
    float fy		        = mp.y * ( 65535.0f / fScreenHeight );		  

    INPUT Input             = { 0 };			
    Input.type		        = INPUT_MOUSE;

    Input.mi.dwFlags	    = MOUSEEVENTF_MOVE|MOUSEEVENTF_ABSOLUTE;

    Input.mi.dx		        = (long)fx;
    Input.mi.dy		        = (long)fy;

    SendInput(1,&Input,sizeof(INPUT));
}


BOOL CALLBACK Callback(HWND hwnd, LPARAM lParam)
{
    char title[4096];
    ::GetWindowText(hwnd, title, _countof(title));
    if(strstr(title, " - Cookie Clicker")) {
        *(HWND*)lParam = hwnd;
    }
    return TRUE;
}
HWND GetCookiesWindow()
{
    HWND ret = 0;
    ::EnumWindows(&Callback, (LPARAM)&ret);
    return ret;
}


union BGRA
{
    struct { BYTE b,g,r,a; };
    BYTE v[4];

          BYTE& operator[](size_t i)       { return v[i]; }
    const BYTE& operator[](size_t i) const { return v[i]; }
};
inline bool operator==(const BGRA &a, const BGRA &b) { return memcmp(a.v, b.v, 4)==0; }
inline bool operator!=(const BGRA &a, const BGRA &b) { return memcmp(a.v, b.v, 4)!=0; }
inline bool operator> (const BGRA &a, const BGRA &b) { return a[0]> b[0] && a[1]> b[1] && a[2]> b[2]; }
inline bool operator< (const BGRA &a, const BGRA &b) { return a[0]< b[0] && a[1]< b[1] && a[2]< b[2]; }
inline bool operator>=(const BGRA &a, const BGRA &b) { return a[0]>=b[0] && a[1]>=b[1] && a[2]>=b[2]; }
inline bool operator<=(const BGRA &a, const BGRA &b) { return a[0]<=b[0] && a[1]<=b[1] && a[2]<=b[2]; }


class Bitmap
{
public:
    Bitmap();
    ~Bitmap();
    int getWidth() const  { return m_info.bmiHeader.biWidth; }
    int getHeight() const { return m_info.bmiHeader.biHeight; }
    BGRA* operator[](size_t yindex) { return &m_data[yindex*getWidth()]; }
    void clear();
    bool copyFromHWND(HWND wnd);
    bool writeToFile(const char *path);

private:
    BITMAPINFO m_info;
    HDC m_hdc;
    HBITMAP m_hbmp;
    BGRA *m_data;
};

class Application
{
public:
    enum State {
        St_Ready,
        St_Running,
        St_Stop,
    };
    Application();
    ~Application();
    void exec();
    bool searchAndClickGoldenCookie();

private:
    State m_state;
    HWND m_hwnd;
    POINT m_window_pos;
    POINT m_click_pos;
    Bitmap m_basebmp;
};



Bitmap::Bitmap()
    : m_hdc(), m_hbmp(), m_data()
{
}

Bitmap::~Bitmap()
{
    clear();
}

void Bitmap::clear()
{
    if(m_hbmp){ ::DeleteObject(m_hbmp); m_hbmp=nullptr; }
    if(m_hdc) { ::DeleteDC(m_hdc); m_hdc=nullptr; }

}

bool Bitmap::copyFromHWND(HWND wnd)
{
    clear();

    RECT rect;
    ::GetWindowRect(wnd, &rect);
    HDC dc = ::GetDC(wnd);
    m_hdc = ::CreateCompatibleDC(dc);
    int width = rect.right-rect.left;
    int height = rect.bottom-rect.top;
    memset(&m_info, 0, sizeof(m_info));
    m_info.bmiHeader.biSize = sizeof(m_info.bmiHeader);
    m_info.bmiHeader.biWidth = width;
    m_info.bmiHeader.biHeight = height;
    m_info.bmiHeader.biPlanes = 1;
    m_info.bmiHeader.biBitCount = 32;
    m_info.bmiHeader.biCompression = BI_RGB;
    m_info.bmiHeader.biSizeImage = width*height*4;
    m_info.bmiHeader.biClrUsed = 0;
    m_info.bmiHeader.biClrImportant = 0;
    m_hbmp = ::CreateDIBSection(m_hdc, &m_info, DIB_RGB_COLORS, (void**)(&m_data), NULL, NULL);
    ::SelectObject(m_hdc, m_hbmp);
    ::BitBlt(m_hdc, 0, 0, width, height, dc, 0, 0, SRCCOPY);
    ::ReleaseDC(wnd, dc);
    return true;
}

bool Bitmap::writeToFile( const char *path )
{
    BITMAPFILEHEADER head;
    memset(&head, 0, sizeof(head));
    head.bfType = 'MB';
    head.bfOffBits = 54;

    if(FILE *f=fopen(path,"wb")) {
        fwrite(&head, sizeof(head), 1, f);
        fwrite(&m_info.bmiHeader, sizeof(m_info.bmiHeader), 1, f);
        fwrite(m_data, getHeight()*getWidth()*4, 1, f);
        fclose(f);
        return true;
    }
    return false;
}


Application::Application()
    : m_state(St_Stop)
    , m_hwnd()
{
    //::Sleep(1000);
    //m_hwnd = ::GetForegroundWindow(); // for debug

    m_hwnd = GetCookiesWindow();
    if(m_hwnd) {
        RECT rect;
        ::GetWindowRect(m_hwnd, &rect);
        m_window_pos.x = rect.left;
        m_window_pos.y = rect.top;
        m_state = St_Ready;

        printf("連打したい位置をクリックしてください\n");
        fflush(stdout);
        ::Sleep(500);
        for(;;) {
            if(::GetKeyState(VK_LBUTTON) & 0x80) {
                CURSORINFO ci;
                ci.cbSize = sizeof(ci);
                ::GetCursorInfo(&ci);
                m_click_pos = ci.ptScreenPos;
                break;
            }
            ::Sleep(10);
        }
        printf("x:%d, y:%d\n\n", m_click_pos.x, m_click_pos.y);

        printf(
            "'B' クリック開始\n"
            "'P' 中断\n"
            "'E' 終了\n"
            "連打中はマウスカーソルを動かせなくなるのでご注意ください\n");
        fflush(stdout);
    }
    else {
        printf("ブラウザで Cookie Clicker を開いた状態で再度このプログラムを起動してください。\n");
        fflush(stdout);
        ::Sleep(2000);
    }
}

Application::~Application()
{
}

void Application::exec()
{
    DWORD last_search_golden_cookie = ::timeGetTime();
    while(m_state!=St_Stop) {
        if(m_state==St_Running) {
            SetMousePosition(m_click_pos);
            LeftClick();
            ::Sleep(5);

            // 2.5 秒毎に golden cookie を探す
            if(::timeGetTime()-last_search_golden_cookie > 2500) {
                searchAndClickGoldenCookie();
                last_search_golden_cookie = ::timeGetTime();
            }
        }
        else {
            ::Sleep(100);
        }

        if(GetAsyncKeyState('B')) {
            m_basebmp.copyFromHWND(m_hwnd);
            //m_basebmp.writeToFile("test.bmp"); // for debug
            m_state = St_Running;
        }
        else if(GetAsyncKeyState('P')) {
            m_state = St_Ready;
        }
        else if(GetAsyncKeyState('E')) {
            m_state = St_Stop;
        }
    }
}

bool Application::searchAndClickGoldenCookie()
{
    const BGRA low  = {{70, 160, 190, 0}};  // 
    const BGRA high = {{150, 230, 240, 0}}; // golden cookie の主要部分の色の範囲

    bool ret = false;
    Bitmap current;
    current.copyFromHWND(m_hwnd);
    for(int y=20; y<m_basebmp.getHeight()-20 && y<current.getHeight()-20; y+=10) {
    for(int x=20; x<m_basebmp.getWidth()-20 && x<current.getWidth()-20; x+=10) {
        BGRA color = current[y][x];
        if(color==m_basebmp[y][x]) { continue; } // 開始直後から変化がなければ飛ばす

        // golden cookie の色に近い場合、周囲の色もそうか判定。
        // 周囲の色もそれっぽければクリック。
        if(color>low && color<high) {
            int score = 0;
            for(int ty=-20; ty<=20; ty+=4) {
            for(int tx=-20; tx<=20; tx+=4) {
                color = current[y+ty][x+tx];
                if(color>low && color<high) { ++score; }
            }
            }
            if(score>10) {
                POINT click_target = {m_window_pos.x+x, m_window_pos.y+(m_basebmp.getHeight()-y)};
                SetMousePosition(click_target);
                LeftClick();
                ret = true;
            }
        }
    }
    }
    return true;
}


int main()
{
    Application app;
    app.exec();
}

// $ cl /EHsc /O2 /Zi CookieClicker.cpp
