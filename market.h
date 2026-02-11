#ifndef MARKET_H
#define MARKET_H

#include <vector>
#include "Stock.h"

class Market {
private:
    Market() = delete; 
    ~Market() = delete;
public:
    static std::vector<Stock> loadMarketStocks();

    static void displayMarket();
};

#endif