#ifndef GLOBALUNIT_HPP
#define GLOBALUNIT_HPP

#include "KeyboardUnit.hpp"
#include "MediaManager.hpp"

// Forward declarations for other engine classes
class TScreen;
class TOpenGlRealTimeLoop;

struct TGlobal {
    TKeyState* keystate;
    TScreen* screen;
    TOpenGlRealTimeLoop* loop;
    TGenericMediaManager* man;

    int mx, my; // Mouse coordinates

    TGlobal() : keystate(nullptr), screen(nullptr), loop(nullptr), man(nullptr), mx(0), my(0) {}
};

extern TGlobal global;

#endif // GLOBALUNIT_HPP