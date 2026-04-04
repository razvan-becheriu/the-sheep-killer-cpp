#include "MsxMath.hpp"
#include <cmath>
#include <algorithm>

TVector getVector(TFloat vx, TFloat vy, TFloat vz) {
    return {vx, vy, vz};
}

TVector Add(const TVector& Vec1, const TVector& Vec2) {
    return {Vec1.x + Vec2.x, Vec1.y + Vec2.y, Vec1.z + Vec2.z};
}

TVector Sub(const TVector& Vec1, const TVector& Vec2) {
    return {Vec1.x - Vec2.x, Vec1.y - Vec2.y, Vec1.z - Vec2.z};
}

TVector Cross(const TVector& Vec1, const TVector& Vec2) {
    return {
        Vec1.y * Vec2.z - Vec1.z * Vec2.y,
        Vec1.z * Vec2.x - Vec1.x * Vec2.z,
        Vec1.x * Vec2.y - Vec1.y * Vec2.x
    };
}

TFloat Magnitude(const TVector& Vec) {
    return std::sqrt(Vec.x * Vec.x + Vec.y * Vec.y + Vec.z * Vec.z);
}

void Normalize(TVector& Vec) {
    TFloat mag = Magnitude(Vec);
    if (mag > 0) {
        Vec.x /= mag; Vec.y /= mag; Vec.z /= mag;
    }
}

TFloat Dot(const TVector& Vec1, const TVector& Vec2) {
    return Vec1.x * Vec2.x + Vec1.y * Vec2.y + Vec1.z * Vec2.z;
}

void Scale(TVector& Vec, TFloat Factor) {
    Vec.x *= Factor; Vec.y *= Factor; Vec.z *= Factor;
}

TFloat Distance(const TVector& v1, const TVector& v2) {
    TFloat dx = v1.x - v2.x;
    TFloat dy = v1.y - v2.y;
    TFloat dz = v1.z - v2.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

void Interpolate(const TVector& v1, const TVector& v2, TFloat value, TVector& res) {
    res.x = v1.x + (v2.x - v1.x) * value;
    res.y = v1.y + (v2.y - v1.y) * value;
    res.z = v1.z + (v2.z - v1.z) * value;
}

TQuaternion Mult(const TQuaternion& a, const TQuaternion& b) {
    TQuaternion res;
    res.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
    res.x = a.y * b.z - a.z * b.y + a.w * b.x + b.w * a.x;
    res.y = a.z * b.x - a.x * b.z + a.w * b.y + b.w * a.y;
    res.z = a.x * b.y - a.y * b.x + a.w * b.z + b.w * a.z;
    return res;
}

TQuaternion Identity() {
    return {0, 0, 0, 1};
}

void SetAxisAngle(TQuaternion& q, TFloat angle, TFloat axisX, TFloat axisY, TFloat axisZ) {
    TFloat factor = axisX * axisX + axisY * axisY + axisZ * axisZ;
    if (factor == 0) factor = 0.000001f;
    TFloat scaleBy = 1.0f / std::sqrt(factor);
    q.w = std::cos(angle / 2.0f);
    TFloat sinHalfAngle = std::sin(angle / 2.0f);
    q.x = axisX * scaleBy * sinHalfAngle;
    q.y = axisY * scaleBy * sinHalfAngle;
    q.z = axisZ * scaleBy * sinHalfAngle;
}

TQuaternion FromEuler(TFloat x, TFloat y, TFloat z) {
    // Convert degrees to radians
    TFloat radX = x * M_PI / 180.0f;
    TFloat radY = y * M_PI / 180.0f;
    TFloat radZ = z * M_PI / 180.0f;

    TQuaternion qX, qY, qZ;
    SetAxisAngle(qX, radX, 1, 0, 0);
    SetAxisAngle(qY, radY, 0, 1, 0);
    SetAxisAngle(qZ, radZ, 0, 0, 1);

    TQuaternion res = Mult(qX, qY);
    res = Mult(res, qZ);
    return res;
}

bool SphereRayCollision(const TVector& rayOrigin, const TVector& rayDir, const TVector& spherePos, TFloat radius) {
    TVector dist = Sub(rayOrigin, spherePos);
    TFloat b = rayDir.x * dist.x + rayDir.y * dist.y + rayDir.z * dist.z;
    TFloat c = dist.x * dist.x + dist.y * dist.y + dist.z * dist.z - radius * radius;
    TFloat d = b * b - c;
    return d >= 0.0f;
}

void Rotate(TVector& v, const TQuaternion& r) {
    TQuaternion u = {v.x, v.y, v.z, 0};
    TQuaternion invR = {-r.x, -r.y, -r.z, r.w}; // Simplification assuming normalized
    TFloat magSq = r.x*r.x + r.y*r.y + r.z*r.z + r.w*r.w;
    invR.x /= magSq; invR.y /= magSq; invR.z /= magSq; invR.w /= magSq;

    TQuaternion h = Mult(r, u);
    TQuaternion finalU = Mult(h, invR);
    
    v.x = finalU.x;
    v.y = finalU.y;
    v.z = finalU.z;
}

TQuaternion Slerp(const TQuaternion& q1, const TQuaternion& q2, TFloat value) {
    TFloat cosom = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
    TQuaternion q2o = q2;

    if (cosom < 0.0f) {
        cosom = -cosom;
        q2o.x = -q2.x; q2o.y = -q2.y; q2o.z = -q2.z; q2o.w = -q2.w;
    }

    TFloat scale0, scale1;
    if ((1.0f - cosom) > EPSILON_VAL) {
        TFloat omega = std::acos(cosom);
        TFloat sinom = std::sin(omega);
        scale0 = std::sin((1.0f - value) * omega) / sinom;
        scale1 = std::sin(value * omega) / sinom;
    } else {
        scale0 = 1.0f - value;
        scale1 = value;
    }

    return {
        scale0 * q1.x + scale1 * q2o.x,
        scale0 * q1.y + scale1 * q2o.y,
        scale0 * q1.z + scale1 * q2o.z,
        scale0 * q1.w + scale1 * q2o.w
    };
}

void QuaternionToMatrix(const TQuaternion& q, float* matrix) {
    float x2 = q.x * q.x;
    float y2 = q.y * q.y;
    float z2 = q.z * q.z;
    float xy = q.x * q.y;
    float xz = q.x * q.z;
    float yz = q.y * q.z;
    float wx = q.w * q.x;
    float wy = q.w * q.y;
    float wz = q.w * q.z;

    matrix[0] = 1.0f - 2.0f * (y2 + z2);
    matrix[1] = 2.0f * (xy + wz);
    matrix[2] = 2.0f * (xz - wy);
    matrix[3] = 0.0f;

    matrix[4] = 2.0f * (xy - wz);
    matrix[5] = 1.0f - 2.0f * (x2 + z2);
    matrix[6] = 2.0f * (yz + wx);
    matrix[7] = 0.0f;

    matrix[8] = 2.0f * (xz + wy);
    matrix[9] = 2.0f * (yz - wx);
    matrix[10] = 1.0f - 2.0f * (x2 + y2);
    matrix[11] = 0.0f;

    matrix[12] = 0.0f;
    matrix[13] = 0.0f;
    matrix[14] = 0.0f;
    matrix[15] = 1.0f;
}