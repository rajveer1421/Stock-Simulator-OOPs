#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <vector>
#include "json.hpp"
using json = nlohmann::json;

class Leaderboard {
private:
    std::string tradersFolder = "Traders";
    std::vector<json> rankings;

public:
    Leaderboard();
    void generateLeaderboard();
    void sortLeaderboard(std::vector<json>& rankings);
    void show_leaderboard();
    void show_top_5();
};

#endif
