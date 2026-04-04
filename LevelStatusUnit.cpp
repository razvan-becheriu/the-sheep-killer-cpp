#include "LevelStatusUnit.hpp"
#include "GlobalUnit.hpp"
#include "OrthoUnit.hpp"
#include "MainStatusUnit.hpp"
#include "DrawModelUnit.hpp"
#include "FontUnit.hpp"
#include "TextureUnit.hpp"
#include "SoundUnit.hpp"
#include "ScreenUnit.hpp"
#include <GL/glu.h>
#include <algorithm>

TLevelStatus* LevelStatus = nullptr;

TLevelStatus::TLevelStatus() : lastTick(0) {}
TLevelStatus::~TLevelStatus() {}

void TLevelStatus::DoEnter() {
    model = std::make_unique<TRealTimeModel>();
    model->load("media/viking_weapon.model", global.man);
    anim = std::make_unique<TAnimationHandler>(model.get());
    anim->Start(1, 1.0f);
    anim->Interpolate();
    lastTick = SDL_GetTicks();
}

void TLevelStatus::DoExit() {
    global.man->getSoundByName("x2")->play();
    anim.reset();
    model.reset();
}

void TLevelStatus::DoLoop() {
    uint32_t now = SDL_GetTicks();
    uint32_t duration = now - lastTick;
    lastTick = now;
    TFloat s = std::max(0.001f, (TFloat)duration / 1000.0f);
    DrawFrame(s);
}

void TLevelStatus::DrawFrame(TFloat delta) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(20, 25, 20, 0, 5, 0, 0, 1, 0);

    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    anim->Update(delta);
    anim->Interpolate();
    DrawRealTimeModel(model.get());

    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    BeginOrtho(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    glColor3f(1, 1, 1);
    TTexture* wri = global.man->getTextureByName("wri");
    if (wri) {
        wri->bind();
        glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0, 0.3f);  glVertex2f((SCREEN_WIDTH / 2.0f) - 100.0f, (SCREEN_HEIGHT / 2.0f) - 120.0f);
        glTexCoord2f(0, 0.65f); glVertex2f((SCREEN_WIDTH / 2.0f) - 100.0f, (SCREEN_HEIGHT / 2.0f) - 50.0f);
        glTexCoord2f(1, 0.3f);  glVertex2f((SCREEN_WIDTH / 2.0f) + 40.0f,  (SCREEN_HEIGHT / 2.0f) - 120.0f);
        glTexCoord2f(1, 0.65f); glVertex2f((SCREEN_WIDTH / 2.0f) + 40.0f,  (SCREEN_HEIGHT / 2.0f) - 50.0f);
        glEnd();
    }

    DrawRaceString((int)(SCREEN_WIDTH / 2.0f) + 40, (int)(SCREEN_HEIGHT / 2.0f) - 120, std::to_string(MainStatus->list.level), 1.0f);
    
    glColor3f(0, 0, 0);
    DrawString((SCREEN_WIDTH / 2) - 70, (SCREEN_HEIGHT * 3 / 4), "Press space to start!");

    EndOrtho();
    SDL_GL_SwapWindow(global.screen->GetWindow());
}

void TLevelStatus::KeyDownEvent(const SDL_KeyboardEvent& event) {
    if (event.keysym.sym == SDLK_SPACE) ChangeListener(MainStatus);
    if (event.keysym.sym == SDLK_ESCAPE) { SDL_Event q; q.type = SDL_QUIT; SDL_PushEvent(&q); }
}

void TLevelStatus::MouseMoveEvent(const SDL_MouseMotionEvent& event) { global.mx = event.x; global.my = event.y; }