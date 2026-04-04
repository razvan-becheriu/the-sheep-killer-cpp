#ifndef MSXMATH_HPP
#define MSXMATH_HPP

#include "Types3D.hpp"

struct TVector {
    union {
        TFloat v[3];
        struct { TFloat x, y, z; };
    };
};

struct TQuaternion {
    union {
        TFloat v[4];
        struct { TFloat x, y, z, w; };
    };
};

// Vector Operations
TVector getVector(TFloat vx, TFloat vy, TFloat vz);
TVector Add(const TVector& Vec1, const TVector& Vec2);
TVector Sub(const TVector& Vec1, const TVector& Vec2);
TVector Cross(const TVector& Vec1, const TVector& Vec2);
TVector Normalized(const TVector& Vec);
void Normalize(TVector& Vec);
TFloat Magnitude(const TVector& Vec);
TFloat Dot(const TVector& Vec1, const TVector& Vec2);
void Scale(TVector& Vec, TFloat Factor);
TFloat Distance(const TVector& v1, const TVector& v2);
void Interpolate(const TVector& v1, const TVector& v2, TFloat value, TVector& res);

// Quaternion Operations
TQuaternion Mult(const TQuaternion& a, const TQuaternion& b);
TQuaternion Identity();
void Normalize(TQuaternion& q);
void SetAxisAngle(TQuaternion& q, TFloat angle, TFloat axisX, TFloat axisY, TFloat axisZ);
void Rotate(TVector& v, const TQuaternion& r);
TQuaternion FromEuler(TFloat x, TFloat y, TFloat z);
TQuaternion Slerp(const TQuaternion& q1, const TQuaternion& q2, TFloat value);
void QuaternionToMatrix(const TQuaternion& q, float* matrix);

// Collision
bool SphereRayCollision(const TVector& rayOrigin, const TVector& RayDirection, const TVector& spherepos, TFloat radius);

#endif // MSXMATH_HPP