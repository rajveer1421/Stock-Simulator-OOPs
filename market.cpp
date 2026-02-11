#include "market.h"
#include <filesystem>
#include <iostream>
#include "json.hpp"
#include <fstream>

namespace fs = std::filesystem;
using namespace std;
using json = nlohmann::json;

vector<Stock> Market::loadMarketStocks() {
    vector<Stock> stocks;
    string folder = "Stocks/";

    if (!fs::exists(folder)) {
        cerr << "⚠️ Stocks folder not found!\n";
        return stocks;
    }

    for (const auto& entry : fs::directory_iterator(folder)) {
        if (entry.path().extension() == ".json") {
            string filename = entry.path().string();
            ifstream file(filename);

            if (!file.is_open()) {
                cerr << "❌ Could not open " << filename << endl;
                continue;
            }

            try {
                json data;
                file >> data;
                file.close();

                string comp = data.value("company_name", "Unknown");
                float price = data.value("current_price", 0.0f);
                int avail = data.value("stock_available", 0);
                int sold = data.value("stock_sold", 0);

                Stock s(comp, price, avail, sold);
                stocks.push_back(s);

            } catch (json::parse_error& e) {
                cerr << "❌ Error parsing " << filename << ": " << e.what() << endl;
            }
        }
    }

    return stocks;
}

void Market::displayMarket() {
    vector<Stock> stocks = loadMarketStocks();

    if (stocks.empty()) {
        cout << "\n📉 No stocks available in the market.\n";
        return;
    }

    cout << "\n======= AVAILABLE STOCKS IN MARKET =======\n";
    for (const auto& s : stocks) {
        s.display();
    }
    cout << "==========================================\n";
}
