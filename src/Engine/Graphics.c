#include <stdlib.h>
#include <math.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define DISPLAY_SCALE 1

unsigned int frameBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

void ClearBuffer() {
    memset(frameBuffer, 0, sizeof(frameBuffer[0][0]) * SCREEN_HEIGHT * SCREEN_WIDTH);
}