#ifndef SOUNDUNIT_HPP
#define SOUNDUNIT_HPP

#include <SDL2/SDL_mixer.h>
#include <string>

class TSound {
private:
    Mix_Chunk* fChunk;
    std::string fName;

public:
    TSound(const std::string& filename);
    ~TSound();

    void play();
    std::string GetName() const { return fName; }
    void SetName(const std::string& name) { fName = name; }
};

bool InitSound();
void CloseSound();

#endif