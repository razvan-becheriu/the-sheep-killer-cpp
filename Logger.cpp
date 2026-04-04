#include "Logger.hpp"
#include <fstream>
#include <chrono>
#include <iomanip>
#include <ctime>

void Log(const std::string& s) {
    std::ofstream logFile("game.log", std::ios_base::app);
    if (logFile.is_open()) {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        
        // Pascal: TimeToStr(now)
        logFile << std::put_time(std::localtime(&now_c), "%H:%M:%S") 
                << " " << s << "\n";
        
        logFile.close();
    }
}