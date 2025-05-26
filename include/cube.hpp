#ifndef CUBE_H
#define CUBE_H

class Cube {
public:
    Cube();
    void draw(float angleX, float angleY);
    void clear();

private:
    void project(float x, float y, float z, int& sx, int& sy);
    float vertices[8][3];
    int edges[12][2];
};

#endif // CUBE_H
