#ifndef FRUSTUMUNIT_HPP
#define FRUSTUMUNIT_HPP

#include <GL/gl.h>

class TFrustumCuller {
public:
    float frustum[6][4];

    void Calculate();
    bool IsPointWithin(float x, float y, float z) const;
    bool IsSphereWithin(float x, float y, float z, float radius) const;

private:
    void NormalizePlane(int side);
};

#endif