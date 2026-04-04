#ifndef MAINUNIT_HPP
#define MAINUNIT_HPP

#include "GlobalUnit.hpp"

class TProg {
public:
    TProg();
    ~TProg();

    void Start();
    void Cleanup();

private:
    void InitSystems();
    void MainLoop();
};

#endif // MAINUNIT_HPP