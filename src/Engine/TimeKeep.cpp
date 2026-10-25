#include <windows.h>
#include <stdint.h>

// Global timing variables
LARGE_INTEGER timer_frequency;
LARGE_INTEGER frame_start_time;
double target_frame_time_ms;

// Initialise the timer variables and set your target FPS
void init_timer(double target_fps) {
    // Get the hardware timer frequency (ticks per second)
    QueryPerformanceFrequency(&timer_frequency);
    
    // Calculate how many milliseconds each frame should ideally take
    target_frame_time_ms = 1000.0 / target_fps;
    
    // Initialise the starting frame mark
    QueryPerformanceCounter(&frame_start_time);
    
    // Request 1ms timer precision from Windows (vital for accurate Sleep timings)
    timeBeginPeriod(1); 
}

// Cleans up system timer adjustments on exit
void cleanup_timer(void) {
    timeEndPeriod(1);
}

// Call this at the very end of your main while() loop to enforce the cap
void cap_frame_rate(void) {
    LARGE_INTEGER frame_end_time;
    QueryPerformanceCounter(&frame_end_time);

    // Calculate time elapsed during this frame in milliseconds
    double elapsed_ms = (double)(frame_end_time.QuadPart - frame_start_time.QuadPart) * 1000.0 / (double)timer_frequency.QuadPart;

    // If the frame finished early, sleep for the remaining duration
    if (elapsed_ms < target_frame_time_ms) {
        DWORD sleep_time = (DWORD)(target_frame_time_ms - elapsed_ms);
        if (sleep_time > 0) {
            Sleep(sleep_time);
        }

        // Spin-lock loop for microsecond-precise alignment to perfect frame targets
        while (1) {
            QueryPerformanceCounter(&frame_end_time);
            elapsed_ms = (double)(frame_end_time.QuadPart - frame_start_time.QuadPart) * 1000.0 / (double)timer_frequency.QuadPart;
            if (elapsed_ms >= target_frame_time_ms) {
                break;
            }
        }
    }

    // Set the baseline counter mark for the next frame iteration
    QueryPerformanceCounter(&frame_start_time);
}