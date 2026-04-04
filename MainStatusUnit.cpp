#include "MainStatusUnit.hpp"
#include "GlobalUnit.hpp"
#include "DrawModelUnit.hpp"
#include "OrthoUnit.hpp"
#include "TextureUnit.hpp"
#include "ScreenUnit.hpp"
#include "FontUnit.hpp"
#include "SoundUnit.hpp"
#include "LevelStatusUnit.hpp"
#include "GameOverStatusUnit.hpp"
#include <GL/glu.h>
#include <algorithm>

TMainStatus* MainStatus = nullptr;

static std::string MillisToString(uint32_t millis) {
    uint32_t total_seconds = millis / 1000;
    uint32_t minutes = total_seconds / 60;
    uint32_t seconds = total_seconds % 60;
    char buf[16];
    // Pascal source uses format('%2D:%2D') and replaces spaces with '0'
    snprintf(buf, sizeof(buf), "%02u:%02u", minutes, seconds);
    return std::string(buf);
}

TMainStatus::TMainStatus() : lastTick(0), timetoend(0) {
    animFence = std::make_unique<TRealTimeModel>();
    animFence->load("media/fence.model", global.man);
    list.Restart();
}

TMainStatus::~TMainStatus() {}

void TMainStatus::DoEnter() {
    timetoend = 0;
    global.keystate->reset();
    // Capture the mouse and hide the cursor for a smoother rotation experience
    SDL_SetRelativeMouseMode(SDL_TRUE);
    starttime = SDL_GetTicks();
    lastTick = starttime;
}

void TMainStatus::DoExit() {
    // Release the mouse when exiting the main game status
    SDL_SetRelativeMouseMode(SDL_FALSE);
    global.man->getSoundByName("x2")->play();
}

void TMainStatus::Rectangle(float x, float z, float w, float h) {
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(0, 0); glVertex3f(x, 0, z);
    glTexCoord2f(0, 1); glVertex3f(x, 0, z + h);
    glTexCoord2f(1, 0); glVertex3f(x + w, 0, z);
    glTexCoord2f(1, 1); glVertex3f(x + w, 0, z + h);
    glEnd();
}

void TMainStatus::Rectangle2d(float x, float y, float w, float h) {
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(0, 0); glVertex2f(x, y);
    glTexCoord2f(0, 1); glVertex2f(x, y + h);
    glTexCoord2f(1, 0); glVertex2f(x + w, y);
    glTexCoord2f(1, 1); glVertex2f(x + w, y + h);
    glEnd();
}

void TMainStatus::DrawField() {
    glPushMatrix();
    glTranslatef(0, 0, 2);
    for (int x = 0; x < AREASIZE; ++x) {
        DrawRealTimeModel(list.animFence);
        glTranslatef(0, 0, 4);
    }
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2, 0, 0);
    glRotatef(-90, 0, 1, 0);
    for (int x = 0; x < AREASIZE; ++x) {
        DrawRealTimeModel(list.animFence);
        glTranslatef(0, 0, -4);
    }
    glPopMatrix();

    glPushMatrix();
    glTranslatef(AREASIZE * TILESIZE, 0, 2);
    glRotatef(180, 0, 1, 0);
    for (int x = 0; x < AREASIZE; ++x) {
        DrawRealTimeModel(list.animFence);
        glTranslatef(0, 0, -4);
    }
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2, 0, AREASIZE * TILESIZE);
    glRotatef(90, 0, 1, 0);
    for (int x = 0; x < AREASIZE; ++x) {
        DrawRealTimeModel(list.animFence);
        glTranslatef(0, 0, 4);
    }
    glPopMatrix();

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glColor3f(1, 1, 1);
    global.man->getTextureByName("tile1")->bind();

    for (int x = 0; x < AREASIZE; ++x) {
        for (int y = 0; y < AREASIZE; ++y) {
            Rectangle(x * 4, y * 4, 4, 4);
        }
    }
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D); // Cleanup: Ensure textures are off after ground tiles
}

void TMainStatus::DoLoop() {
    uint32_t now = SDL_GetTicks();
    uint32_t duration = now - lastTick;
    lastTick = now;
    TFloat s = std::max(0.001f, (TFloat)duration / 1000.0f);
    DrawFrame(s);
}

void TMainStatus::DrawFrame(TFloat delta) {
    list.Update(delta);

    // Accumulate time if either the level is won or the player is dead
    if (list.numSheep == 0 || (list.player && list.player->dead)) {
        timetoend += delta;
    }

    // Transition to next level after a short delay if all sheep are gone
    if (timetoend > 5.0f) {
       if (list.numSheep == 0) {
            list.NextLevel();
            ChangeListener(LevelStatus);
            return;
        } else if (list.player && list.player->dead) {
            ChangeListener(gameoverstatus);
            return;
        }
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    campos = { 20.0f + list.player->position.x, 20.0f, 20.0f + list.player->position.y };
    camtarget = { list.player->position.x, 0, list.player->position.y };

    gluLookAt(campos.x, campos.y, campos.z, camtarget.x, camtarget.y, camtarget.z, 0, 1, 0);
    culler.Calculate();

    // Replicate Pascal's Stage Setup: Lighting ON, Textures OFF for untextured models
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);

    DrawField();

    for (auto& obj : list.objects) {
        if (culler.IsSphereWithin(obj->position.x, 3, obj->position.y, 3)) {
            if (obj->dead) {
                // Force the model to the last frame of the death animation if the unit is dead
                obj->anim->Model()->interpolate(ANIM_DIE, 0.999f);
            } else {
                obj->anim->Interpolate();
            }
            glPushMatrix();
            glTranslatef(obj->position.x, 0, obj->position.y);
            glRotatef(obj->angle * 180.0f / M_PI, 0, 1, 0);
            
            // Transparent occlusion check simplification
            bool isOccluded = (obj.get() != list.player && TestOcclusion(obj.get()));
            if (isOccluded) {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
            }
            
            DrawRealTimeModel(obj->anim->Model());
            
            if (isOccluded) glDisable(GL_BLEND);
            glPopMatrix();
        }
    }

    // --- 2D HUD Rendering ---
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    BeginOrtho(SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Debug Info
    glColor3f(0, 0, 0);
    DrawString(10, 10, "FPS: " + std::to_string((int)(1.0f / delta)));
    DrawString(100, 10, "TIME: " + MillisToString(SDL_GetTicks() - starttime));

    // HUD Textures
    glColor3f(1, 1, 1);

    // Title Sprite (Logo)
    TTexture* title = global.man->getTextureByName("title");
    if (title) {
        title->bind();
        Rectangle2d(0, -20, 200, 200);
    }

    // Life
    TTexture* heart = global.man->getTextureByName("heart");
    if (heart) {
        heart->bind();
        Rectangle2d(0, (float)SCREEN_HEIGHT - 120, 32, 32);
        DrawRaceString(40, SCREEN_HEIGHT - 120, std::to_string(std::max(0, list.player->life)), 0.5f);
    }

    // Sheep Counter
    TTexture* sheep = global.man->getTextureByName("sheep");
    if (sheep) {
        sheep->bind();
        Rectangle2d(0, (float)SCREEN_HEIGHT - 80, 32, 32);
        DrawRaceString(40, SCREEN_HEIGHT - 80, std::to_string(list.numSheep), 0.5f);
    }

    // Points
    TTexture* point = global.man->getTextureByName("point");
    if (point) {
        point->bind();
        Rectangle2d(0, (float)SCREEN_HEIGHT - 40, 32, 32);
        DrawRaceString(40, SCREEN_HEIGHT - 40, std::to_string(list.score), 0.5f);
    }

    EndOrtho();

    SDL_GL_SwapWindow(global.screen->GetWindow());
}

bool TMainStatus::TestOcclusion(TBase* a) {
    TVector raytarget = { list.player->position.x, list.player->modelsize, list.player->position.y };
    raytarget = Sub(raytarget, campos);
    Normalize(raytarget);

    TVector spherepos = { a->position.x, a->modelsize, a->position.y };
    return SphereRayCollision(campos, raytarget, spherepos, a->modelsize);
}

void TMainStatus::KeyDownEvent(const SDL_KeyboardEvent& event) {
    global.keystate->processKeyDown(event.keysym.sym);
    if (event.keysym.sym == SDLK_ESCAPE) {
        SDL_Event quitEv;
        quitEv.type = SDL_QUIT;
        SDL_PushEvent(&quitEv);
    }
}

void TMainStatus::KeyUpEvent(const SDL_KeyboardEvent& event) {
    global.keystate->processKeyUp(event.keysym.sym);
}

void TMainStatus::MouseMoveEvent(const SDL_MouseMotionEvent& event) {
    global.mx = event.x;
    global.my = event.y;

    // Apply relative horizontal mouse movement to the player's rotation angle
    if (list.player && !list.player->dead) {
        const TFloat sensitivity = 0.005f;
        list.player->angle -= (TFloat)event.xrel * sensitivity;
    }
}