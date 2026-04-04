#ifndef ANIMATIONHANDLER_HPP
#define ANIMATIONHANDLER_HPP

#include "ModelUnit.hpp"

class TAnimationHandler {
private:
    bool fAlreadyTriggered;
    TFloat fAnimPos;
    TFloat fAnimSeconds;
    int fAnimNum;
    TRealTimeModel* fModel;

public:
    TAnimationHandler(TRealTimeModel* aModel) 
        : fAlreadyTriggered(false), fAnimPos(0), fAnimSeconds(1.0f), fAnimNum(0), fModel(aModel) {}

    bool Update(TFloat delta) {
        fAnimPos += delta / fAnimSeconds;
        if (fAnimPos >= 1.0f) {
            fAlreadyTriggered = false;
            fAnimPos -= 1.0f;
            return true; // Animation finished/looped
        }
        return false;
    }

    bool Trigger() {
        bool result = !fAlreadyTriggered && fModel && fModel->triggerPassed(fAnimNum, fAnimPos);
        if (result) fAlreadyTriggered = true;
        return result;
    }

    void Start(int animNum, TFloat duration) {
        fAnimPos = 0;
        fAnimNum = animNum;
        fAnimSeconds = duration;
        fAlreadyTriggered = false;
    }

    void Interpolate() {
        if (fModel) fModel->interpolate(fAnimNum, fAnimPos);
    }

    TRealTimeModel* Model() const { return fModel; }
    int GetCurrentAnim() const { return fAnimNum; }
};

#endif