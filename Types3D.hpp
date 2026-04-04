#ifndef TYPES3D_HPP
#define TYPES3D_HPP

#include <cmath>

const double EPSILON_VAL = 0.00000000001;

typedef float TFloat;

struct TVertex {
    TFloat x, y, z;    // Coordinates
    TFloat nx, ny, nz; // Normal
    TFloat tu, tv;     // Texture coordinates
};

struct Tuv {
    TFloat u, v;
};

struct TFace {
    int point[3];      // Indices to vertices
    TFloat nx, ny, nz; // Face normal
    Tuv uv[3];         // Face UV map
    bool smooth;       // Smooth shading flag
};

struct TRGB {
    TFloat r, g, b;
    TFloat& operator[](int i) { return (i == 0) ? r : (i == 1 ? g : b); }
    const TFloat& operator[](int i) const { return (i == 0) ? r : (i == 1 ? g : b); }
};

#endif // TYPES3D_HPP