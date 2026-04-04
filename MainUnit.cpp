#include "MainUnit.hpp"
#include "ScreenUnit.hpp"
#include "MainStatusUnit.hpp"
#include "MediaManagerUnit.hpp"
#include "OrthoUnit.hpp"
#include "SoundUnit.hpp"
#include "IntroStatusUnit.hpp"
#include "LevelStatusUnit.hpp"
#include "GameOverStatusUnit.hpp"
#include <iostream>

TGlobal global; // Define the global instance

TProg::TProg() {}
TProg::~TProg() {}

void TProg::InitSystems() {
    // The original Pascal source suggests a '-fs' flag. 
    // You should ideally parse command line arguments to set this variable.
    bool startFullscreen = true; 

    global.screen = new TScreen("The Sheep Killer!", SCREEN_WIDTH, SCREEN_HEIGHT, 32, startFullscreen);
    if (!global.screen->Initialize()) {
        throw std::runtime_error("Failed to initialize screen");
    }

    // --- Loading Screen ---
    TFileMediaManager* fileMan = new TFileMediaManager();
    fileMan->getTextureByName("loading")->bind();
    BeginOrtho(SCREEN_WIDTH, SCREEN_HEIGHT);
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(0, 0); glVertex2f((SCREEN_WIDTH - 256) / 2.0f, (SCREEN_HEIGHT - 256) / 2.0f);
    glTexCoord2f(0, 1); glVertex2f((SCREEN_WIDTH - 256) / 2.0f, (SCREEN_HEIGHT + 256) / 2.0f);
    glTexCoord2f(1, 0); glVertex2f((SCREEN_WIDTH + 256) / 2.0f, (SCREEN_HEIGHT - 256) / 2.0f);
    glTexCoord2f(1, 1); glVertex2f((SCREEN_WIDTH + 256) / 2.0f, (SCREEN_HEIGHT + 256) / 2.0f);
    glEnd();
    EndOrtho();
    SDL_GL_SwapWindow(global.screen->GetWindow());

    if (!InitSound()) {
        std::cerr << "Sound system failed to initialize" << std::endl;
    }

    global.keystate = new TKeyState();
    fileMan->LoadGameAssets();
    global.man = (TGenericMediaManager*)fileMan;
    
    // OpenGL generic setup
    glMatrixMode(GL_MODELVIEW);
    glClearColor(0.7f, 1.0f, 1.0f, 0.0f);

    glDisable(GL_LIGHTING); // Lighting starts OFF by default in Pascal

    // Default static light source setup (matching Pascal constants)
    GLfloat lightAmbient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat lightDiffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    GLfloat lightPosition[] = { 128.0f, 200.0f, 128.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glEnable(GL_LIGHT0);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    // Load initial game state
    MainStatus = new TMainStatus();
    IntroStatus = new TIntroStatus();
    LevelStatus = new TLevelStatus();
    gameoverstatus = new TGameOverStatus();
    ChangeListener(IntroStatus);
}

void TProg::Start() {
    InitSystems();
    MainLoop();
}

void TProg::MainLoop() {
    SDL_Event event;
    bool running = true;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;

            // Update OpenGL viewport when the window size changes (like on fullscreen toggle)
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                glViewport(0, 0, event.window.data1, event.window.data2);
            }

            // Handle Fullscreen Toggle (Alt + Enter)
            if (event.type == SDL_KEYDOWN && 
                event.key.keysym.sym == SDLK_RETURN && 
                (event.key.keysym.mod & KMOD_ALT)) {
                
                Uint32 flags = SDL_GetWindowFlags(global.screen->GetWindow());
                if (flags & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP)) {
                    SDL_SetWindowFullscreen(global.screen->GetWindow(), 0);
                } else {
                    SDL_SetWindowFullscreen(global.screen->GetWindow(), SDL_WINDOW_FULLSCREEN_DESKTOP);
                }
            }

            if (ActiveStatus) {
                if (event.type == SDL_KEYDOWN) ActiveStatus->KeyDownEvent(event.key);
                if (event.type == SDL_KEYUP) ActiveStatus->KeyUpEvent(event.key);
                if (event.type == SDL_MOUSEMOTION) ActiveStatus->MouseMoveEvent(event.motion);
            }
        }

        if (ActiveStatus && ActiveStatus->active) {
            ActiveStatus->DoLoop();
        }
        
        SDL_Delay(1); // Prevent 100% CPU usage
    }
}

void TProg::Cleanup() {
    // 1. Stop the loop and clear the active status reference
    ActiveStatus = nullptr;

    // 2. Delete game states (this also destroys the TBaseList and its models)
    delete MainStatus; MainStatus = nullptr;
    delete IntroStatus; IntroStatus = nullptr;
    delete LevelStatus; LevelStatus = nullptr;
    delete gameoverstatus; gameoverstatus = nullptr;

    // 3. Delete Media Manager (this destroys all Textures, Meshes, and Sounds)
    // This MUST happen while the GL Context and Mixer are still active.
    delete global.man; global.man = nullptr;

    // 4. Shut down audio and video subsystems
    CloseSound();
    if (global.screen) global.screen->Finalize();

    // 5. Final memory cleanup for core systems
    delete global.screen;
    global.screen = nullptr;
    delete global.keystate;
    global.keystate = nullptr;
}