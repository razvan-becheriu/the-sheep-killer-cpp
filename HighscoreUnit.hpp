#ifndef HIGHSCOREUNIT_HPP
#define HIGHSCOREUNIT_HPP

#include <string>
#include <vector>

struct THighscoreEntry {
    int score;
    std::string name;
};

class THighscore {
private:
    std::vector<THighscoreEntry> entries;
    std::string GetFilePath() const;

public:
    void Load();
    void Save();
    void AddScore(int score, const std::string& name);
    
    size_t Count() const { return entries.size(); }
    int GetScore(int index) const { return entries[index].score; }
    std::string GetName(int index) const { return entries[index].name; }
};

#endif