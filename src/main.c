#include <stdio.h>
#include "Engine/Window.h"
#include "Engine/Graphics.h"
#include "Engine/TimeKeep.h"

int main(void) {
    HWND hwnd = NULL;
    
    if (!CreateAppWindow(&hwnd)) {
        return -1;
    }

    // Target a smooth retro cap, like 60 FPS
    init_timer(60.0);

    BOOL running = TRUE;
    while (running) {
        // 1. Process standard OS messages
        running = ProcessWindowMessages();

        ClearBuffer();
        // 3. Command Windows to instantly update the viewport display area
        InvalidateRect(hwnd, NULL, FALSE);

        // 4. Force the loop to pause if it's moving too fast
        cap_frame_rate();
    }

    cleanup_timer();
    return 0;
}