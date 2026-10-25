#include <stdio.h>
#include <windows.h>
#include "Graphics.h"

LRESULT CALLBACK WindowProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_KEYDOWN:

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Fetch the layout of the current window client area
            RECT client_rect;
            GetClientRect(hwnd, &client_rect);
            int win_width = client_rect.right - client_rect.left;
            int win_height = client_rect.bottom - client_rect.top;

            // Set up metadata mapping out our 2D array structure
            BITMAPINFO bmi = {0};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = SCREEN_WIDTH;
            bmi.bmiHeader.biHeight = -SCREEN_HEIGHT; // Negative value sets a top-down coordinate grid
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;          // 32 bits fits unsigned int data perfectly
            bmi.bmiHeader.biCompression = BI_RGB;

            // Direct pixel transfer from your 2D array to the operating system's window DC
            StretchDIBits(
                hdc,
                0, 0, win_width, win_height,   // Destination sizing
                0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, // Framebuffer sizing
                &frameBuffer,                  // Memory location pointer
                &bmi,
                DIB_RGB_COLORS,
                SRCCOPY
            );

            EndPaint(hwnd, &ps);
            return 0;
        }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int CreateAppWindow(HWND* out_hwnd)
{
    const char CLASS_NAME[] = "EngineWindow";

    WNDCLASS wc = {0};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;

    // Fix context visual trailing/flicker by adding a black background fallback brush
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

    if (!RegisterClass(&wc))
        return 0;

    // Calculate actual outer window frame bounds to accommodate target pixel space cleanly
    RECT wr = {0, 0, SCREEN_WIDTH * DISPLAY_SCALE, SCREEN_HEIGHT * DISPLAY_SCALE};
    AdjustWindowRect(&wr, WS_CAPTION | WS_SYSMENU, FALSE);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Engine",
        WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        wr.right - wr.left,  // Width adjusted for borders
        wr.bottom - wr.top,  // Height adjusted for title bar & borders
        NULL,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    if (hwnd == NULL)
        return 0;

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    // Output the window handle so your main application loop can trigger visual updates
    if (out_hwnd) {
        *out_hwnd = hwnd;
    }

    return 1;
}

// Adjusted method returning a boolean state tracking application execution loop lifecycle
BOOL ProcessWindowMessages()
{
    MSG msg;

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            return FALSE; // Loop terminating event caught

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return TRUE; // Active frame execution remains fine
}
