#include <windows.h>
#include <cstdio>
#include <regex>
#pragma comment(lib,"user32.lib")

// Cookie Clicker ( http://orteil.dashnet.org/cookieclicker/ ) 連打ツール
// 
// Cookie Clicker のウィンドウを探して所定位置でクリック連打。
// Chrome 以外では動作未確認。


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
    if(std::regex_search(title, std::regex("cookies - Cookie Clicker"))) {
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

int main()
{
    HWND hwnd = GetCookiesWindow();
    if(hwnd) {
        RECT rect;
        ::GetWindowRect(hwnd, &rect);

        POINT pos;
        pos.x = rect.left + 250;
        pos.y = rect.top + 500;

        enum State {
            St_Ready,
            St_Running,
            St_Stop,
        };
        State state = St_Ready;

        printf(
            "'B' クリック開始\n"
            "'P' 中断\n"
            "'E' 終了\n"
            "連打中はマウスカーソルを動かせなくなるのでご注意ください\n");

        while(state!=St_Stop) {
            if(state==St_Running) {
                SetMousePosition(pos);
                LeftClick();
                ::Sleep(4);
            }
            else {
                ::Sleep(100);
            }

            if(GetAsyncKeyState('B')) {
                state = St_Running;
            }
            else if(GetAsyncKeyState('P')) {
                state = St_Ready;
            }
            else if(GetAsyncKeyState('E')) {
                state = St_Stop;
            }
        }
    }
}

// $ cl /EHsc /O2 /Zi CookieClicker.cpp
