#ifndef STATEUNIT_HPP
#define STATEUNIT_HPP

#include <SDL2/SDL.h>

class TEventListener {
public:
    bool active;
    virtual ~TEventListener() = default;

    virtual void DoEnter() = 0;
    virtual void DoExit() = 0;
    virtual void DoLoop() = 0;

    virtual void KeyDownEvent(const SDL_KeyboardEvent& event) {}
    virtual void KeyUpEvent(const SDL_KeyboardEvent& event) {}
    virtual void MouseDownEvent(const SDL_MouseButtonEvent& event) {}
    virtual void MouseUpEvent(const SDL_MouseButtonEvent& event) {}
    virtual void MouseMoveEvent(const SDL_MouseMotionEvent& event) {}
};

extern TEventListener* ActiveStatus;

// Equivalent to Pascal procedure ChangeListener
void ChangeListener(TEventListener* newState);

#endif