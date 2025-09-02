// ============== Position.h ==============
#ifndef POSITION_H
#define POSITION_H

#include <cmath>  // Add this for sqrt
#include <cstdlib> // Add this for abs

struct Position {
    double x, y;
    Position(double xx = 0, double yy = 0) : x(xx), y(yy) {}

    bool operator==(const Position& other) const {
        return (abs(x - other.x) < 0.1 && abs(y - other.y) < 0.1);
    }
};

// Utility functions
double Distance(Position a, Position b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

#endif