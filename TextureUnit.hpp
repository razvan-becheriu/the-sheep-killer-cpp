#ifndef TEXTUREUNIT_HPP
#define TEXTUREUNIT_HPP

#include <string>
#include <GL/gl.h>

class TTexture {
public:
    std::string name;
    GLuint textureID;

    TTexture();
    ~TTexture();

    bool LoadFrom(const std::string& filename);
    void bind();
};

#endif