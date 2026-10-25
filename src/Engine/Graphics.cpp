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

mesh MechCube;


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


void FillBuffer(unsigned int color) {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            frameBuffer[y][x] = color;
        }
    }
}


void MultiplyMatrixVector(vertex &i, vertex &o, mat4x4 &m) {
	o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
	o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
	o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
	float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

	if (w != 0.0f)
	{
		o.x /= w; o.y /= w; o.z /= w;
	}
}


void UpdateDisplay() {
        mat4x4 MatRotZ, MatRotX;
        fTheta += 0.02f;

		// Rotation Z
		MatRotZ.m[0][0] = cos(fTheta);
		MatRotZ.m[0][1] = sin(fTheta);
		MatRotZ.m[1][0] = -sin(fTheta);
		MatRotZ.m[1][1] = cos(fTheta);
		MatRotZ.m[2][2] = 1;
		MatRotZ.m[3][3] = 1;

		// Rotation X
		MatRotX.m[0][0] = 1;
		MatRotX.m[1][1] = cos(fTheta * 0.5f);
		MatRotX.m[1][2] = sin(fTheta * 0.5f);
		MatRotX.m[2][1] = -sin(fTheta * 0.5f);
		MatRotX.m[2][2] = cos(fTheta * 0.5f);
		MatRotX.m[3][3] = 1;

        // Draw Triangles
        for (auto tri : MechCube.Triangle) {
            triangle TriProjected, TriTranslated, TriRotatedZ, TriRotatedZX;

            // Rotates the vertices in the Z axis
            MultiplyMatrixVector(tri.p[0], TriRotatedZ.p[0], MatRotZ);
            MultiplyMatrixVector(tri.p[1], TriRotatedZ.p[1], MatRotZ);
            MultiplyMatrixVector(tri.p[2], TriRotatedZ.p[2], MatRotZ);

            // Rotates the vertices in the X axis
            MultiplyMatrixVector(TriRotatedZ.p[0], TriRotatedZX.p[0], MatRotX);
            MultiplyMatrixVector(TriRotatedZ.p[1], TriRotatedZX.p[1], MatRotX);
            MultiplyMatrixVector(TriRotatedZ.p[2], TriRotatedZX.p[2], MatRotX);


            // Move the vertices
            TriTranslated = TriRotatedZX;
            TriTranslated.p[0].z = TriRotatedZX.p[0].z + 3.0f;
            TriTranslated.p[1].z = TriRotatedZX.p[1].z + 3.0f;
            TriTranslated.p[2].z = TriRotatedZX.p[2].z + 3.0f;

            MultiplyMatrixVector(TriTranslated.p[0], TriProjected.p[0], matProj);
            MultiplyMatrixVector(TriTranslated.p[1], TriProjected.p[1], matProj);
            MultiplyMatrixVector(TriTranslated.p[2], TriProjected.p[2], matProj);

            // Scale into view
            TriProjected.p[0].x += 1.0f; TriProjected.p[0].y += 1.0;
            TriProjected.p[1].x += 1.0f; TriProjected.p[1].y += 1.0;
            TriProjected.p[2].x += 1.0f; TriProjected.p[2].y += 1.0;

            TriProjected.p[0].x *= 0.5f * (float)SCREEN_WIDTH; TriProjected.p[0].y *= 0.5f * (float)SCREEN_HEIGHT;
            TriProjected.p[1].x *= 0.5f * (float)SCREEN_WIDTH; TriProjected.p[1].y *= 0.5f * (float)SCREEN_HEIGHT;
            TriProjected.p[2].x *= 0.5f * (float)SCREEN_WIDTH; TriProjected.p[2].y *= 0.5f * (float)SCREEN_HEIGHT;

            DrawTriangle(TriProjected.p[0].x, TriProjected.p[0].y,
                TriProjected.p[1].x, TriProjected.p[1].y,
                TriProjected.p[2].x, TriProjected.p[2].y,
                0x00FFFFFF);
        }
}