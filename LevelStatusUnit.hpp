#ifndef LEVELSTATUSUNIT_HPP
#define LEVELSTATUSUNIT_HPP

#include "StateUnit.hpp"
#include "ModelUnit.hpp"
#include "AnimationHandler.hpp"

class TLevelStatus : public TEventListener {
public:
    uint32_t lastTick;
    std::unique_ptr<TRealTimeModel> model;
    std::unique_ptr<TAnimationHandler> anim;

    TLevelStatus();
    virtual ~TLevelStatus() override;

    void DrawFrame(TFloat delta);

    virtual void DoEnter() override;
    virtual void DoExit() override;
    virtual void DoLoop() override;

    virtual void KeyDownEvent(const SDL_KeyboardEvent& event) override;
    virtual void MouseMoveEvent(const SDL_MouseMotionEvent& event) override;
};

extern TLevelStatus* LevelStatus;

#endif