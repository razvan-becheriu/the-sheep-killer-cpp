#include "HighscoreUnit.hpp"
#include "Utils.hpp"
#include <fstream>
#include <algorithm>
#include <filesystem>

std::string THighscore::GetFilePath() const {
    std::string base = Utils::GetPersonalPath();
#ifdef _WIN32
    return base + "\\thesheepkiller\\highscores.txt";
#else
    return base + "/.thesheepkiller/highscores.txt";
#endif
}

void THighscore::AddScore(int score, const std::string& name) {
    auto it = std::find_if(entries.begin(), entries.end(), [&](const THighscoreEntry& e) {
        return e.name == name;
    });

    if (it != entries.end()) {
        if (score > it->score) {
            it->score = score;
        } else {
            return;
        }
    } else {
        entries.push_back({score, name});
    }

    std::sort(entries.begin(), entries.end(), [](const THighscoreEntry& a, const THighscoreEntry& b) {
        return a.score > b.score;
    });
    if (entries.size() > 10) entries.resize(10);
}

void THighscore::Load() {
    entries.clear();
    std::ifstream file(GetFilePath());
    if (!file.is_open()) {
        for(int i=0; i<10; ++i) entries.push_back({0, "no one"});
        return;
    }

    int s;
    std::string n;
    while (file >> s >> std::ws && std::getline(file, n)) {
        entries.push_back({s, n});
    }
    if (entries.empty()) {
        for(int i=0; i<10; ++i) entries.push_back({0, "no one"});
    }
}

void THighscore::Save() {
    std::string path = GetFilePath();
    std::filesystem::create_directories(std::filesystem::path(path).parent_path());
    
    std::ofstream file(path);
    for (const auto& e : entries) {
        file << e.score << " " << e.name << "\n";
    }
}