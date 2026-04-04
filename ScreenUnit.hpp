#ifndef SCREENUNIT_HPP
#define SCREENUNIT_HPP

#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <string>

class TScreen {
private:
    std::string fTitle;
    int fWidth, fHeight, fBpp;
    float fFarClip;
    bool fFullScreen;
    SDL_Window* fWindow;
    SDL_GLContext fContext;

    bool InitSdl();
    void InitGL();
    bool SetUpWindow(int width, int height, float farclip);

public:
    TScreen(const std::string& aTitle, int aWidth, int aHeight, int aBpp, bool aFullscreen, float afarclip = 450.0f);
    ~TScreen();

    bool Initialize();
    void Finalize();
    bool IsActive() const { return fWindow != nullptr; }

    SDL_Window* GetWindow() const { return fWindow; }
};

#endif