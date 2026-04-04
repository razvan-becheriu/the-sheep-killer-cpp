#ifndef VIKING_HPP
#define VIKING_HPP

#include "BaseUnit.hpp"
#include "ModelUnit.hpp" // Changed from Model.hpp to ModelUnit.hpp

class TViking : public TBase {
public:
    TRealTimeModel* model; // Changed from TModel* to TRealTimeModel*
    TFloat speed;
    TFloat health;

    TViking();
    virtual ~TViking() override = default;

    virtual void Update(TFloat dt) override;
    virtual void Render() override;
    
    void MoveForward(TFloat amount);
};

#endif // VIKING_HPP