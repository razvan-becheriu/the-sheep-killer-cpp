#include "ScreenUnit.hpp"
#include "Logger.hpp"
#include <iostream>

TScreen::TScreen(const std::string& aTitle, int aWidth, int aHeight, int aBpp, bool aFullscreen, float afarclip)
    : fTitle(aTitle), fWidth(aWidth), fHeight(aHeight), fBpp(aBpp), fFarClip(afarclip), 
      fFullScreen(aFullscreen), fWindow(nullptr), fContext(nullptr) {}

TScreen::~TScreen() {
    Finalize();
}

bool TScreen::InitSdl() {
    Log("Initializing SDL");
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;

    uint32_t flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    if (fFullScreen) flags |= SDL_WINDOW_FULLSCREEN;

    fWindow = SDL_CreateWindow(fTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                fWidth, fHeight, flags);
    
    if (!fWindow) return false;

    fContext = SDL_GL_CreateContext(fWindow);
    SDL_GL_SetSwapInterval(1); // VSync

    return true;
}

void TScreen::InitGL() {
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    SetUpWindow(fWidth, fHeight, fFarClip);
    
    float color[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
    glMaterialfv(GL_FRONT, GL_AMBIENT, color);
}

bool TScreen::SetUpWindow(int width, int height, float farclip) {
    if (height == 0) height = 1;
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)width / (double)height, 2.0, farclip);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    return true;
}

bool TScreen::Initialize() {
    if (InitSdl()) {
        InitGL();
        return true;
    }
    return false;
}

void TScreen::Finalize() {
    Log("Finalizing SDL");
    if (fContext) {
        SDL_GL_DeleteContext(fContext);
        fContext = nullptr;
    }
    if (fWindow) {
        SDL_DestroyWindow(fWindow);
        fWindow = nullptr;
    }
    SDL_Quit();
}