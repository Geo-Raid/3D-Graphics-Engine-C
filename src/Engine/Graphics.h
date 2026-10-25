#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Graphics.c"

extern unsigned int frameBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

extern int DrawLine(int x1, int y1, int x2, int y2, unsigned int color);

extern int DrawObject(char *name);

extern void ClearBuffer();

#endif