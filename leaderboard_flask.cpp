#include "leaderboard.h"
#include <iostream>
using namespace std;

int main() {
    cout << "⚙️ Generating Leaderboard (Flask Mode)..." << endl;
    Leaderboard lb;
    lb.generateLeaderboard();
    cout << "✅ Leaderboard.json updated successfully!" << endl;
    return 0;
}
