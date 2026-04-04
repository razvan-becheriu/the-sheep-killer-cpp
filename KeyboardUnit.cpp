#include "KeyboardUnit.hpp"

TKeyState::TKeyState() : left(false), shift(false), right(false), up(false), down(false), tab(false), ctrl(false), space(false), enter(false) {}

void TKeyState::reset() {
    left = false;
    shift = false;
    right = false;
    up = false;
    down = false;
    tab = false;
    ctrl = false;
    space = false;
    enter = false;
    keyStates.clear();
}

void TKeyState::processKeyUp(SDL_Keycode key) {
    keyStates[key] = false;
    switch (key) {
        case SDLK_RIGHT: right = false; break;
        case SDLK_LEFT: left = false; break;
        case SDLK_UP: up = false; break;
        case SDLK_TAB: tab = false; break;
        case SDLK_RCTRL:
        case SDLK_LCTRL: ctrl = false; break;
        case SDLK_SPACE: space = false; break;
        case SDLK_RETURN: enter = false; break;
        case SDLK_DOWN: down = false; break;
        case SDLK_RSHIFT:
        case SDLK_LSHIFT: shift = false; break;
        default: break;
    }
}

void TKeyState::processKeyDown(SDL_Keycode key) {
    keyStates[key] = true;
    switch (key) {
        case SDLK_RIGHT: right = true; break;
        case SDLK_LEFT: left = true; break;
        case SDLK_UP: up = true; break;
        case SDLK_TAB: tab = true; break;
        case SDLK_RCTRL:
        case SDLK_LCTRL: ctrl = true; break;
        case SDLK_SPACE: space = true; break;
        case SDLK_RETURN: enter = true; break;
        case SDLK_DOWN: down = true; break;
        case SDLK_RSHIFT:
        case SDLK_LSHIFT: shift = true; break;
        default: break;
    }
}