#ifndef STOCK_H
#define STOCK_H

#include <string>
#include <iostream>
using namespace std;
#include "trader.h"
#include "company.h"
class Stock {
private:
    string company_name;
    float current_price;
    int stock_available;
    int stock_sold;
    friend class Trader;
    friend class Company;

public:
    Stock();
    Stock(string company_name);
    Stock(string company_name, float current_price, int stock_available, int stock_sold);

    void display() const;
    void updateDetail();

    string getCompanyName() const;
    string getStockId() const;
    float getPrice() const;
    int getQuantity() const;

    void setPrice(float price);
    void setQuantity(int quantity);

    void addQuantity(int qty);  
    void soldQuantity(int qty); 
    void stockLoss(int qty);  

    void increasePrice(int qty);
    void decreasePrice(int qty);
};

#endif
