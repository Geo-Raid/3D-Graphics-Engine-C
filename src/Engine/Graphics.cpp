#include <stdlib.h>
#include <math.h>
#include <vector>

#include <fstream>
#include <strstream>
#include <algorithm>

#define SCREEN_WIDTH 1500
#define SCREEN_HEIGHT 1000

#define DISPLAY_SCALE 1

unsigned int frameBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];


struct vec3d{
    float x, y, z;
} ;

struct triangle {
    vec3d p[3];
    unsigned int Color;
};

struct mesh{
    vector<triangle> Triangle;

    bool LoadFromObjectFile(string sFilename) {
        ifstream f(sFilename);
        if (!f.is_open())
            return false;

        // Local cache of verts
        vector<vec3d> verts;

        while (!f.eof()) {
            char line[128];
            f.getline(line, 128);

            strstream s;
            s << line;
            
            char type;
            if (line[0] == 'v') {
                vec3d v;
                s >> type >> v.x >> v.y >> v.z;
                verts.push_back(v);
            }

            if (line[0] == 'f') {
                int f[3];
                s >> type >> f[0] >> f[1] >> f[2];
                Triangle.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
            }
        }

        return true;
    }

};

vec3d vCamera = { 0.0f, 0.0f, 0.0f };

mesh MeshCube;


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


void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, unsigned int color)
{
    // Sort vertices by their Y coordinate (top to bottom)
    if (y1 > y2) { swap(x1, x2); swap(y1, y2); }
    if (y1 > y3) { swap(x1, x3); swap(y1, y3); }
    if (y2 > y3) { swap(x2, x3); swap(y2, y3); }

    // Calculate the triangle's bounding box
    int minY = max(0, y1);
    int maxY = min(SCREEN_HEIGHT - 1, y3);

    for (int y = minY; y <= maxY; y++)
    {
        // Find intersections with the two sides
        float t1 = (y2 == y1) ? 0.0f : (float)(y - y1) / (y2 - y1);
        float t2 = (y3 == y1) ? 0.0f : (float)(y - y1) / (y3 - y1);

        float xa = x1 + t1 * (x2 - x1);
        float xb = x1 + t2 * (x3 - x1);

        // Below the middle vertex, use the lower-left edge
        if (y > y2)
        {
            t1 = (float)(y - y2) / (y3 - y2);
            xa = x2 + t1 * (x3 - x2);
        }

        int startX = max(0, (int)min(xa, xb));
        int endX = min(SCREEN_WIDTH - 1, (int)max(xa, xb));

        // Fill the horizontal scanline
        for (int x = startX; x <= endX; x++)
        {
            frameBuffer[y][x] = color;
        }
    }
}


void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, unsigned int color) {
    DrawLine(x1, y1, x2, y2, color);
    DrawLine(x2, y2, x3, y3, color);
    DrawLine(x3, y3, x1, y1, color);
}


void FillBuffer(unsigned int color) {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            frameBuffer[y][x] = color;
        }
    }
}


void MultiplyMatrixVector(vec3d &i, vec3d &o, mat4x4 &m) {
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

        vector<triangle> VecTriangleToRaster;

        // Draw Triangles
        for (auto tri : MeshCube.Triangle) {
            triangle TriProjected, TriTranslated, TriRotatedZ, TriRotatedZX;

            // Rotates the vertices in the Z axis
            MultiplyMatrixVector(tri.p[0], TriRotatedZ.p[0], MatRotZ);
            MultiplyMatrixVector(tri.p[1], TriRotatedZ.p[1], MatRotZ);
            MultiplyMatrixVector(tri.p[2], TriRotatedZ.p[2], MatRotZ);

            // Rotates the vertices in the X axis
            MultiplyMatrixVector(TriRotatedZ.p[0], TriRotatedZX.p[0], MatRotX);
            MultiplyMatrixVector(TriRotatedZ.p[1], TriRotatedZX.p[1], MatRotX);
            MultiplyMatrixVector(TriRotatedZ.p[2], TriRotatedZX.p[2], MatRotX);


            // Offset into the screen
            TriTranslated = TriRotatedZX;
            TriTranslated.p[0].z = TriRotatedZX.p[0].z + 8.0f;
            TriTranslated.p[1].z = TriRotatedZX.p[1].z + 8.0f;
            TriTranslated.p[2].z = TriRotatedZX.p[2].z + 8.0f;

            vec3d normal, line1, line2;
            line1.x = TriTranslated.p[1].x - TriTranslated.p[0].x;
            line1.y = TriTranslated.p[1].y - TriTranslated.p[0].y;
            line1.z = TriTranslated.p[1].z - TriTranslated.p[0].z;
            
            line2.x = TriTranslated.p[2].x - TriTranslated.p[0].x;
            line2.y = TriTranslated.p[2].y - TriTranslated.p[0].y;
            line2.z = TriTranslated.p[2].z - TriTranslated.p[0].z;

            normal.x = line1.y * line2.z - line1.z * line2.y;
            normal.y = line1.z * line2.x - line1.x * line2.z;
            normal.z = line1.x * line2.y - line1.y * line2.x;

			// It's normally normal to normalise the normal
			float l = sqrtf(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
			normal.x /= l; normal.y /= l; normal.z /= l;

			//if (normal.z < 0)
			if(normal.x * (TriTranslated.p[0].x - vCamera.x) + 
			   normal.y * (TriTranslated.p[0].y - vCamera.y) +
			   normal.z * (TriTranslated.p[0].z - vCamera.z) < 0.0f)
			{
                vec3d LightDirection = { 0.0f, 0.0f, 1.0f };
			    float l = sqrtf(LightDirection.x*LightDirection.x + LightDirection.y*LightDirection.y + LightDirection.z*LightDirection.z);
			    LightDirection.x /= l; LightDirection.y /= l; LightDirection.z /= l;

                float dp = normal.x * LightDirection.x + normal.y * LightDirection.y + normal.z * LightDirection.z;
                unsigned int Shade = 255 * abs(dp);
                Shade += Shade * 0x10000 + Shade * 0x100;

                TriTranslated.Color = Shade;

                // Project triangles from 3D to 2D
                MultiplyMatrixVector(TriTranslated.p[0], TriProjected.p[0], matProj);
                MultiplyMatrixVector(TriTranslated.p[1], TriProjected.p[1], matProj);
                MultiplyMatrixVector(TriTranslated.p[2], TriProjected.p[2], matProj);
                TriProjected.Color = TriTranslated.Color;

                // Scale into view
                TriProjected.p[0].x += 1.0f; TriProjected.p[0].y += 1.0;
                TriProjected.p[1].x += 1.0f; TriProjected.p[1].y += 1.0;
                TriProjected.p[2].x += 1.0f; TriProjected.p[2].y += 1.0;

                TriProjected.p[0].x *= 0.5f * (float)SCREEN_WIDTH;
                TriProjected.p[0].y *= 0.5f * (float)SCREEN_HEIGHT;
                TriProjected.p[1].x *= 0.5f * (float)SCREEN_WIDTH;
                TriProjected.p[1].y *= 0.5f * (float)SCREEN_HEIGHT;
                TriProjected.p[2].x *= 0.5f * (float)SCREEN_WIDTH;
                TriProjected.p[2].y *= 0.5f * (float)SCREEN_HEIGHT;

                VecTriangleToRaster.push_back(TriProjected);
            }

            // Sort triangles from back to front
            sort(VecTriangleToRaster.begin(),  VecTriangleToRaster.end(), [](triangle &t1, triangle &t2) {
                float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
                float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
                return z1 > z2;
            });

            for (auto &TriProjected : VecTriangleToRaster) {
                // Rasterrize triangles
                FillTriangle(TriProjected.p[0].x, TriProjected.p[0].y,
                    TriProjected.p[1].x, TriProjected.p[1].y,
                    TriProjected.p[2].x, TriProjected.p[2].y,
                    TriProjected.Color);

                DrawTriangle(TriProjected.p[0].x, TriProjected.p[0].y,
                    TriProjected.p[1].x, TriProjected.p[1].y,
                    TriProjected.p[2].x, TriProjected.p[2].y,
                    0x00);
            }

        }
}