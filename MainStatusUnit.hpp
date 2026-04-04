#ifndef MAINSTATUSUNIT_HPP
#define MAINSTATUSUNIT_HPP

#include "StateUnit.hpp"
#include "GameUnit.hpp"
#include "FrustumUnit.hpp"
#include "ModelUnit.hpp"
#include <memory>

class TMainStatus : public TEventListener {
public:
    std::unique_ptr<TRealTimeModel> animFence;
    uint32_t lastTick;
    TBaseList list;
    TFrustumCuller culler;
    uint32_t starttime;
    TFloat timetoend;
    TVector campos, camtarget;

    TMainStatus();
    virtual ~TMainStatus() override;

    void DrawFrame(TFloat delta);
    bool TestOcclusion(TBase* a);
    void DrawField();

    virtual void DoEnter() override;
    virtual void DoExit() override;
    virtual void DoLoop() override;

    virtual void KeyDownEvent(const SDL_KeyboardEvent& event) override;
    virtual void KeyUpEvent(const SDL_KeyboardEvent& event) override;
    virtual void MouseMoveEvent(const SDL_MouseMotionEvent& event) override;

private:
    void Rectangle(float x, float z, float w, float h);
    void Rectangle2d(float x, float y, float w, float h);
};

extern TMainStatus* MainStatus;

#endif