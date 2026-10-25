using namespace std;

#include "Engine/Window.h"
#include "Engine/Graphics.h"
#include "Engine/TimeKeep.h"

#define PI 3.14159

typedef struct {
    float m[4][4] = { 0 };
} mat4x4;

mat4x4 matProj;

mesh MechCube;

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


void Init() {

    MechCube.Triangle = {

		// SOUTH
		{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

		// EAST                                                      
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

		// NORTH                                                     
		{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

		// WEST                                                      
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

		// TOP                                                       
		{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

		// BOTTOM                                                    
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },
    };

    float fNear = 0.1f;
    float fFar = 1000.0f;
    float fFov = 90.0f;
    float fAspectRatio = (float)SCREEN_HEIGHT / (float)SCREEN_WIDTH;
    float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * PI);

	matProj.m[0][0] = fAspectRatio * fFovRad;
	matProj.m[1][1] = fFovRad;
	matProj.m[2][2] = fFar / (fFar - fNear);
	matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
	matProj.m[2][3] = 1.0f;
	matProj.m[3][3] = 0.0f;
}


int main(void) {
    HWND hwnd = NULL;
    
    if (!CreateAppWindow(&hwnd)) {
        return -1;
    }

    Init();

    // Set FPS to 60
    init_timer(60.0);

    BOOL running = TRUE;
    while (running) {
        // Process standard OS messages
        running = ProcessWindowMessages();

        ClearBuffer();
        // Command Windows to instantly update the viewport display area
        InvalidateRect(hwnd, NULL, FALSE);

        // Draw Triangles

        for (auto tri : MechCube.Triangle) {
            triangle TriProjected;
            MultiplyMatrixVector(tri.p[0], TriProjected.p[0], matProj);
            MultiplyMatrixVector(tri.p[1], TriProjected.p[1], matProj);
            MultiplyMatrixVector(tri.p[2], TriProjected.p[2], matProj);

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

        // Force the loop to pause if it's moving too fast
        cap_frame_rate();
    }

    cleanup_timer();
    return 0;
}