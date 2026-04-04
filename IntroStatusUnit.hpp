#ifndef INTROSTATUSUNIT_HPP
#define INTROSTATUSUNIT_HPP

#include "StateUnit.hpp"
#include "Types3D.hpp"
#include <SDL2/SDL_mixer.h>

class TIntroStatus : public TEventListener {
public:
    uint32_t lastTick;
    Mix_Music* music;

    TIntroStatus();
    virtual ~TIntroStatus() override;

    void DrawFrame(TFloat delta);

    virtual void DoEnter() override;
    virtual void DoExit() override;
    virtual void DoLoop() override;

    virtual void KeyDownEvent(const SDL_KeyboardEvent& event) override;
    virtual void MouseMoveEvent(const SDL_MouseMotionEvent& event) override;
};

extern TIntroStatus* IntroStatus;

#endif