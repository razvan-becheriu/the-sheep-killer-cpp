#ifndef BASEUNIT_HPP
#define BASEUNIT_HPP

#include "MsxMath.hpp"
#include <vector>
#include <memory>

class TBase {
public:
    TVector position;
    TQuaternion rotation;
    TFloat scale;
    bool active;

    TBase() : position({0,0,0}), rotation(Identity()), scale(1.0f), active(true) {}
    virtual ~TBase() = default;

    virtual void Update(TFloat dt) {}
    virtual void Render() {}
};

// Equivalent to the Pascal TBaseList or TObjectList
class TBaseManager {
public:
    std::vector<std::unique_ptr<TBase>> objects;
    void UpdateAll(TFloat dt);
    void RenderAll();
};

#endif // BASEUNIT_HPP