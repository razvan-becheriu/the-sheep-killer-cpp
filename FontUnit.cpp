#include "FontUnit.hpp"
#include "GlobalUnit.hpp"
#include "TextureUnit.hpp"
#include <GL/gl.h>

const int CHAR_W = 32;
const int CHAR_H = 64;

void DrawRaceChar(int x, int y, char c, TFloat scale) {
    if (c == ' ') return;
    int i = (int)c - 48;
    float tx = ((i % 8) * CHAR_W) / 256.0f;
    float ty = ((i / 8) * CHAR_H) / 256.0f;
    float w = CHAR_W / 256.0f;
    float h = CHAR_H / 256.0f;

    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(tx, ty); glVertex2f(x, y);
    glTexCoord2f(tx, ty + h); glVertex2f(x, y + CHAR_H * scale);
    glTexCoord2f(tx + w, ty); glVertex2f(x + CHAR_W * scale, y);
    glTexCoord2f(tx + w, ty + h); glVertex2f(x + CHAR_W * scale, y + CHAR_H * scale);
    glEnd();
}

void DrawRaceString(int x, int y, const std::string& s, TFloat scale) {
    glColor3f(1, 1, 1);
    TTexture* tex = global.man->getTextureByName("numbers");
    if (tex) tex->bind();
    
    for (size_t i = 0; i < s.length(); ++i) {
        DrawRaceChar(x + i * (int)(CHAR_W * scale), y, s[i], scale);
    }
}

// Simple 8x8 font implementation for FPS and DEBUG info (default scale)
void DrawString(int x, int y, const std::string& s) {
    DrawString(x, y, s, 1.0f);
}

// Simple 8x8 font implementation for FPS and DEBUG info
void DrawString(int x, int y, const std::string& s, TFloat scale) {
    TTexture* tex = global.man->getTextureByName("font");
    if (tex) tex->bind();
    
    for (size_t i = 0; i < s.length(); ++i) {
        int idx = (int)s[i] - 32;
        float tx = (float)(idx % 16) * 8.0f / 128.0f;
        float ty = (float)(idx / 16) * 8.0f / 128.0f + 0.5f;
        float tw = 8.0f / 128.0f;
        float th = 8.0f / 128.0f;
        
        float cx = (float)x + (float)i * 7.0f * scale;
        float cy = (float)y;
        float cw = 8.0f * scale;
        float ch = 8.0f * scale;

        glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(tx, ty); glVertex2f(cx, cy);
        glTexCoord2f(tx, ty + th); glVertex2f(cx, cy + ch);
        glTexCoord2f(tx + tw, ty); glVertex2f(cx + cw, cy);
        glTexCoord2f(tx + tw, ty + th); glVertex2f(cx + cw, cy + ch);
        glEnd();
    }
}