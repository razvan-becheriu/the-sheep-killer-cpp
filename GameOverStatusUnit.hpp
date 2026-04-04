#ifndef GAMEOVERSTATUSUNIT_HPP
#define GAMEOVERSTATUSUNIT_HPP

#include "StateUnit.hpp"
#include "HighscoreUnit.hpp"
#include "Types3D.hpp"
#include <memory>

class TGameOverStatus : public TEventListener {
public:
    THighscore highscore;
    uint32_t lastTick;
    std::string playerName;

    TGameOverStatus();
    virtual ~TGameOverStatus() override;

    void DrawFrame(TFloat delta);

    virtual void DoEnter() override;
    virtual void DoExit() override;
    virtual void DoLoop() override;

    virtual void KeyDownEvent(const SDL_KeyboardEvent& event) override;
    virtual void MouseMoveEvent(const SDL_MouseMotionEvent& event) override;
};

extern TGameOverStatus* gameoverstatus;

#endif