#include <stdlib.h>
#include <math.h>
#include <vector>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define DISPLAY_SCALE 1

unsigned int frameBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];


typedef struct {
    float x, y, z;
} vertex;

typedef struct {
    vertex p[3];
} triangle;

typedef struct {
    vector<triangle> Triangle;
} mesh;


void DrawLine(int x1, int y1, int x2, int y2, unsigned int color) {
    int dx = abs(x2 - x1);
    int stepX = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1);
    int stepY = y1 < y2 ? 1 : -1;
    int error = dx + dy;

    while (!(x1 == x2) || !(y1 == y2)) {
        if (!(x1 < 0 || x1 > SCREEN_WIDTH -1 || x2 < 0 || x2 > SCREEN_WIDTH -1 || y1 < 0 || y1 > SCREEN_HEIGHT -1 || y2 < 0 || y2 > SCREEN_HEIGHT -1))
            frameBuffer[y1][x1] = color;

        int doubledError = 2 * error;
        if (doubledError >= dy) {
            error += dy;
            x1 += stepX;
        }
        if (doubledError <= dx) {
            error += dx;
            y1 += stepY;
        }
    }
}


void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, unsigned int color) {
    DrawLine(x1, y1, x2, y2, 0x00FFFFFF);
    DrawLine(x2, y2, x3, y3, 0x00FFFFFF);
    DrawLine(x3, y3, x1, y1, 0x00FFFFFF);
}


void ClearBuffer() {
    memset(frameBuffer, 0, sizeof(frameBuffer[0][0]) * SCREEN_HEIGHT * SCREEN_WIDTH);
}