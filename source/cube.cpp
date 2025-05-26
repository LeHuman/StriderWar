#include "cube.hpp"

#include <math.h>

#include "graphics.hpp"

Cube::Cube() {
    float size = 100.0f;
    vertices[0][0] = -size;
    vertices[0][1] = -size;
    vertices[0][2] = -size;
    vertices[1][0] = size;
    vertices[1][1] = -size;
    vertices[1][2] = -size;
    vertices[2][0] = size;
    vertices[2][1] = size;
    vertices[2][2] = -size;
    vertices[3][0] = -size;
    vertices[3][1] = size;
    vertices[3][2] = -size;
    vertices[4][0] = -size;
    vertices[4][1] = -size;
    vertices[4][2] = size;
    vertices[5][0] = size;
    vertices[5][1] = -size;
    vertices[5][2] = size;
    vertices[6][0] = size;
    vertices[6][1] = size;
    vertices[6][2] = size;
    vertices[7][0] = -size;
    vertices[7][1] = size;
    vertices[7][2] = size;

    int e[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};
    for (int i = 0; i < 12; ++i) {
        edges[i][0] = e[i][0];
        edges[i][1] = e[i][1];
    }
}

void Cube::project(float x, float y, float z, int &sx, int &sy) {
    float scale = 100.0f / (z + 200.0f);
    sx = (int)(x * scale + (640 / 2));
    sy = (int)(y * scale + (200 / 2));
}

float rotated[8][3];
int result[12][4] = {0};

void Cube::draw(float angleX, float angleY) {
    for (int i = 0; i < 8; ++i) {
        float x = vertices[i][0];
        float y = vertices[i][1];
        float z = vertices[i][2];

        float rx = y * sin(angleX) + z * cos(angleX);
        float ry = y * cos(angleX) - z * sin(angleX);
        y = ry;
        z = rx;

        float rz = z * cos(angleY) - x * sin(angleY);
        float rx2 = z * sin(angleY) + x * cos(angleY);
        x = rx2;
        z = rz;

        rotated[i][0] = x;
        rotated[i][1] = y;
        rotated[i][2] = z;
    }

    for (int i = 0; i < 12; ++i) {
        int &x1 = result[i][0];
        int &y1 = result[i][1];
        int &x2 = result[i][2];
        int &y2 = result[i][3];

        Graphics::line(x1, y1, x2, y2, 0);

        project(rotated[edges[i][0]][0], rotated[edges[i][0]][1], rotated[edges[i][0]][2], x1, y1);
        project(rotated[edges[i][1]][0], rotated[edges[i][1]][1], rotated[edges[i][1]][2], x2, y2);

        Graphics::line(x1, y1, x2, y2, 1);
    }
}
