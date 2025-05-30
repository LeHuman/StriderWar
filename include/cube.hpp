#pragma once

#include "fixed.hpp"

class Cube {
public:
    Cube();
    void draw(Fixed &angleX, Fixed &angleY);

private:
    void project(Fixed &x, Fixed &y, Fixed &z, int &sx, int &sy);
    Fixed vertices[8][3];
    int edges[12][2];
};
