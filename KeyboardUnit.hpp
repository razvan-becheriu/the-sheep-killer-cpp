#ifndef KEYBOARDUNIT_HPP
#define KEYBOARDUNIT_HPP

#include <map>
#include <SDL2/SDL.h> // Assuming SDL2

class TKeyState {
public:
    bool left, shift, right, up, down, tab, ctrl, space, enter;
    std::map<SDL_Keycode, bool> keyStates; // To track all keys

    TKeyState();
    void processKeyUp(SDL_Keycode key);
    void processKeyDown(SDL_Keycode key);
    void reset();
};

#endif // KEYBOARDUNIT_HPP