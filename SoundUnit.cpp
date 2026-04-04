#include "SoundUnit.hpp"
#include "Logger.hpp"
#include <SDL2/SDL.h> // Required for SDL_InitSubSystem and SDL_INIT_AUDIO
#include <iostream>

TSound::TSound(const std::string& filename) : fChunk(nullptr), fName("") {
    Log("loading " + filename);
    fChunk = Mix_LoadWAV(filename.c_str());
    if (!fChunk) {
        Log("Unable to load wav " + filename);
    }
}

TSound::~TSound() {
    if (fChunk) {
        Mix_FreeChunk(fChunk);
        fChunk = nullptr;
    }
}

void TSound::play() {
    if (fChunk) {
        Mix_PlayChannel(-1, fChunk, 0);
    }
}

bool InitSound() {
    Log("Initializing Sound and Mixer");
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        Log("No sdl audio: " + std::string(SDL_GetError()));
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) != 0) {
        Log("No sdl mixer: " + std::string(Mix_GetError()));
        return false;
    }

    Mix_Init(MIX_INIT_MOD);
    return true;
}

void CloseSound() {
    Mix_HaltChannel(-1);
    Mix_CloseAudio();
    Mix_Quit();
}