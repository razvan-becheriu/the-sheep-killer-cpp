#ifndef ORTHOUNIT_HPP
#define ORTHOUNIT_HPP

#include <GL/gl.h>

inline void BeginOrtho(float width, float height) {
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    // glOrtho(left, right, bottom, top, near, far)
    // Pascal: glOrtho(0, Width, Height, 0, 0, 100);
    glOrtho(0, width, height, 0, 0, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

inline void EndOrtho() {
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

inline void SetMaterialColor(float r, float g, float b) {
    float color[] = {r, g, b, 1.0f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
    glMaterialfv(GL_FRONT, GL_AMBIENT, color);
}

#endif