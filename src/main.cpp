using namespace std;

#define PI 3.14159

typedef struct mat4x4 {
    float m[4][4] = { 0 };
} mat4x4;

mat4x4 matProj;

unsigned int fElapsedTime = 0;
float fTheta = 0.0f;

#include "Engine/Window.h"
#include "Engine/Graphics.h"
#include "Engine/TimeKeep.h"


void Init() {

    /*
    MeshCube.Triangle = {

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
    MechCube.Triangle = {

		// SOUTH
		{ 0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f,    1.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f,    0.5f, 1.0f, 0.5f,    1.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f,    0.5f, 1.0f, 0.5f,    1.0f, 0.0f, 1.0f },
        { 1.0f, 0.0f, 1.0f,    0.5f, 1.0f, 0.5f,    0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f,    0.5f, 1.0f, 0.5f,    0.0f, 0.0f, 0.0f }
    };
    */

    MeshCube.LoadFromObjectFile("../Assets/VideoShip.obj");

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

        FillBuffer(0); // fills the color the the window to black

        UpdateDisplay();

        // Command Windows to instantly update the viewport display area
        InvalidateRect(hwnd, NULL, FALSE);

        fElapsedTime += 1; // Increments every frame to keep count of up time

        // Force the loop to pause if it's moving too fast
        cap_frame_rate();
    }

    cleanup_timer();
    return 0;
}