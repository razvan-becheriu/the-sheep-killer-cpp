#include "GameOverStatusUnit.hpp"
#include "GlobalUnit.hpp"
#include "OrthoUnit.hpp"
#include "IntroStatusUnit.hpp"
#include "MainStatusUnit.hpp"
#include "FontUnit.hpp"
#include "TextureUnit.hpp"
#include "SoundUnit.hpp"
#include "ScreenUnit.hpp"
#include <GL/glu.h>
#include <algorithm>

TGameOverStatus* gameoverstatus = nullptr;

TGameOverStatus::TGameOverStatus() : lastTick(0) {}
TGameOverStatus::~TGameOverStatus() {}

void TGameOverStatus::DoEnter() {
    global.keystate->reset();
    highscore.Load();
    playerName = "";
    lastTick = SDL_GetTicks();
}

void TGameOverStatus::DoExit() {
    MainStatus->list.Restart();
    global.man->getSoundByName("x2")->play();
}

void TGameOverStatus::DoLoop() {
    uint32_t now = SDL_GetTicks();
    uint32_t duration = now - lastTick;
    lastTick = now;
    TFloat s = std::max(0.001f, (TFloat)duration / 1000.0f);
    DrawFrame(s);
}

void TGameOverStatus::DrawFrame(TFloat delta) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(20, 20, 20, 0, 0, 0, 0, 1, 0);

    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    BeginOrtho(SCREEN_WIDTH, SCREEN_HEIGHT);

    TTexture* goTex = global.man->getTextureByName("gameover");
    if (goTex) {
        goTex->bind();
        glColor3f(1, 1, 1);
        glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0, 0); glVertex2f(0, 0);
        glTexCoord2f(0, 1); glVertex2f(0, (float)SCREEN_HEIGHT);
        glTexCoord2f(1, 0); glVertex2f((float)SCREEN_WIDTH, 0);
        glTexCoord2f(1, 1); glVertex2f((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
        glEnd();
    }

    std::string scoreStr = std::to_string(MainStatus->list.score);
    // Center the current score display
    int scoreX = (SCREEN_WIDTH / 2) - (int)(scoreStr.length() * 32 / 2);
    DrawRaceString(scoreX, (int)(SCREEN_HEIGHT * 0.5f), scoreStr, 1.0f);
    
    glColor3f(0, 0, 0);

    // Name Entry Display
    std::string prompt = "ENTER NAME: " + playerName + "_";
    int promptX = (SCREEN_WIDTH / 2) - (int)(prompt.length() * 7 / 2);
    DrawString(promptX, (int)(SCREEN_HEIGHT * 0.65f), prompt);
    
    int listBaseY = (int)(SCREEN_HEIGHT * 0.7f);
    DrawString((SCREEN_WIDTH / 2) - 42, listBaseY, "*HIGHSCORES*");

    for (size_t i = 0; i < highscore.Count(); ++i) {
        int rowY = listBaseY + 25 + (int)i * 15;
        DrawString((SCREEN_WIDTH / 2) - 80, rowY, std::to_string(highscore.GetScore((int)i)));
        DrawString((SCREEN_WIDTH / 2) - 10, rowY, highscore.GetName((int)i));
    }

    EndOrtho();
    SDL_GL_SwapWindow(global.screen->GetWindow());
}

void TGameOverStatus::KeyDownEvent(const SDL_KeyboardEvent& event) {
    if (event.keysym.sym == SDLK_RETURN || event.keysym.sym == SDLK_KP_ENTER) {
        if (!playerName.empty()) {
            highscore.AddScore(MainStatus->list.score, playerName);
            highscore.Save();
            playerName = "";
        }
        ChangeListener(IntroStatus);
    } else if (event.keysym.sym == SDLK_BACKSPACE) {
        if (!playerName.empty()) playerName.pop_back();
    } else if (event.keysym.sym >= 32 && event.keysym.sym <= 126) {
        if (playerName.length() < 15) {
            char c = (char)event.keysym.sym;
            if (event.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT)) c = toupper(c);
            playerName += c;
        }
    }

    if (event.keysym.sym == SDLK_ESCAPE) {
        SDL_Event quitEv; quitEv.type = SDL_QUIT; SDL_PushEvent(&quitEv);
    }
}

void TGameOverStatus::MouseMoveEvent(const SDL_MouseMotionEvent& event) {
    global.mx = event.x;
    global.my = event.y;
}