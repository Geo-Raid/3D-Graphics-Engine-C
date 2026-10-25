#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Graphics.cpp"

extern unsigned int frameBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

extern void DrawLine(int x1, int y1, int x2, int y2, unsigned int color);
extern void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, unsigned int color);

extern void ClearBuffer();

#endif