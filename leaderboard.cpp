#include "leaderboard.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;

float getCurrentWorth(json &traderData) {
    float balance = traderData.value("balance", 0.0f);
    float totalWorth = balance;

    if (traderData.contains("portfolio")) {
        for (auto &p : traderData["portfolio"]) {
            string cname = p.value("company_name", "");
            int qty = p.value("quantity", 0);

            string stockFile = "Stocks/" + cname + ".json";
            ifstream sf(stockFile);
            if (!sf.is_open()) continue;

            json sdata; 
            sf >> sdata; 
            sf.close();

            float price = sdata.value("current_price", 0.0f);
            totalWorth += qty * price;
        }
    }
    return totalWorth;
}

Leaderboard::Leaderboard() {
    generateLeaderboard();
}

void Leaderboard::generateLeaderboard() {
    rankings.clear();
    
    if (!fs::exists(tradersFolder)) return;

    for (auto &entry : fs::directory_iterator(tradersFolder)) {
        ifstream in(entry.path());
        if (!in.is_open()) continue;
        
        json user; 
        in >> user;
        in.close();

        float initial_balance = user.value("initial_balance", 0.0f);
        float worth = getCurrentWorth(user);
        float diff = worth - initial_balance;
        float percent_change = (initial_balance > 0) ? (diff / initial_balance) * 100 : 0;

        string change;
        if (percent_change > 0)
            change = to_string(percent_change) + "% profit";
        else if (percent_change < 0)
            change = to_string(percent_change) + "% loss";
        else
            change = "constant";

        json tmp;
        tmp["username"] = user.value("name", "Unknown");
        tmp["worth"] = worth;
        tmp["change"] = change;

        rankings.push_back(tmp);
    }

    sortLeaderboard(rankings);

    for (int i = 0; i < rankings.size(); i++)
        rankings[i]["rank"] = i + 1;

    ofstream out("Leaderboard.json");
    out << setw(4) << rankings;
    out.close();
}

void Leaderboard::sortLeaderboard(vector<json> &rankings) {
    for (int i = rankings.size() - 1; i >= 0; i--) {
        for (int j = 0; j < i; j++) {
            if (rankings[j]["worth"] < rankings[j + 1]["worth"]) {
                swap(rankings[j], rankings[j + 1]);
            }
        }
    }
}

void Leaderboard::show_leaderboard() {
    ifstream in("Leaderboard.json");
    json data; in >> data;

    cout << "\n==================== LEADERBOARD ====================\n";
    cout << left << setw(6) << "RANK" 
         << setw(15) << "USERNAME" 
         << setw(20) << "CURRENT WORTH" 
         << "PERCENT CHANGE\n";
    cout << string(65, '-') << "\n";

    for (auto &tmp : data) {
        cout << left << setw(6) << tmp["rank"].get<int>()
             << setw(15) << tmp["username"].get<string>()
             << setw(20) << tmp["worth"].get<float>()
             << tmp["change"].get<string>() << "\n";
    }
}

void Leaderboard::show_top_5() {
    ifstream in("Leaderboard.json");
    json data; in >> data;

    cout << "\n==================== TOP 5 PLAYERS ====================\n";
    cout << left << setw(8) << "RANK"
         << setw(20) << "USERNAME"
         << setw(20) << "WORTH"
         << setw(20) << "CHANGE\n";
    cout << string(70, '-') << "\n";

    for (int i = 0; i < min(5,(int)data.size()); i++) {
        json tmp = data[i];
        cout << left << setw(8) << tmp["rank"].get<int>()
             << setw(20) << tmp["username"].get<string>()
             << setw(20) << tmp["worth"].get<float>()
             << setw(20) << tmp["change"].get<string>() << "\n";
    }
}
