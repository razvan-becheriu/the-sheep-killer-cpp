#include <iostream>
#include "MainUnit.hpp"

int main(int argc, char* argv[]) {
    try {
        TProg prog;
        prog.Start();
        prog.Cleanup();
    } catch (const std::exception& e) {
        std::cerr << "Application error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}