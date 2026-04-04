#include "IntroStatusUnit.hpp"
#include "GlobalUnit.hpp"
#include "OrthoUnit.hpp"
#include "GameUnit.hpp"
#include "LevelStatusUnit.hpp"
#include "FontUnit.hpp"
#include "TextureUnit.hpp"
#include "SoundUnit.hpp"
#include "ScreenUnit.hpp"
#include <GL/glu.h>
#include <algorithm>

TIntroStatus* IntroStatus = nullptr;

TIntroStatus::TIntroStatus() : lastTick(0), music(nullptr) {
    music = Mix_LoadMUS("media/yeeha.xm");
    if (music) {
        Mix_PlayMusic(music, -1);
    }
}

TIntroStatus::~TIntroStatus() {
    if (music) Mix_FreeMusic(music);
}

void TIntroStatus::DoEnter() {
    global.keystate->reset();
    lastTick = SDL_GetTicks();
}

void TIntroStatus::DoExit() {
    global.man->getSoundByName("x2")->play();
}

void TIntroStatus::DoLoop() {
    uint32_t now = SDL_GetTicks();
    uint32_t duration = now - lastTick;
    lastTick = now;
    TFloat s = std::max(0.001f, (TFloat)duration / 1000.0f);
    DrawFrame(s);
}

void TIntroStatus::DrawFrame(TFloat delta) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(20, 20, 20, 0, 0, 0, 0, 1, 0);

    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    BeginOrtho(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    TTexture* intro = global.man->getTextureByName("intro");
    if (intro) {
        intro->bind();
        glColor3f(1, 1, 1);
        glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0, 0); glVertex2f(0, 0);
        glTexCoord2f(0, 1); glVertex2f(0, (float)SCREEN_HEIGHT);
        glTexCoord2f(1, 0); glVertex2f((float)SCREEN_WIDTH, 0);
        glTexCoord2f(1, 1); glVertex2f((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
        glEnd();
    }

    EndOrtho();
    SDL_GL_SwapWindow(global.screen->GetWindow());
}

void TIntroStatus::KeyDownEvent(const SDL_KeyboardEvent& event) {
    if (event.keysym.sym == SDLK_SPACE) {
        ChangeListener(LevelStatus);
    }
    if (event.keysym.sym == SDLK_ESCAPE) {
        SDL_Event quitEv; quitEv.type = SDL_QUIT; SDL_PushEvent(&quitEv);
    }
}

void TIntroStatus::MouseMoveEvent(const SDL_MouseMotionEvent& event) {
    global.mx = event.x;
    global.my = event.y;
}