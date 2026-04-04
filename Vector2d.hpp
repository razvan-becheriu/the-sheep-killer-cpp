#ifndef VECTOR2D_HPP
#define VECTOR2D_HPP

#include "Types3D.hpp"
#include <cmath>

struct TVector2D {
    TFloat x, y;
};

inline TFloat Magnitude(const TVector2D& v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

inline TVector2D Sub(const TVector2D& a, const TVector2D& b) {
    return { a.x - b.x, a.y - b.y };
}

inline TVector2D Rotate(const TVector2D& v, TFloat angle) {
    TFloat s = std::sin(angle);
    TFloat c = std::cos(angle);
    return { v.x * c + v.y * s, -v.x * s + v.y * c };
}

inline TFloat AngleDist(TFloat a, TFloat b) {
    TFloat x = a - b;
    TFloat y = (M_PI * 2.0f) - a + b;
    return (std::abs(x) < std::abs(y)) ? -x : y;
}

#endif