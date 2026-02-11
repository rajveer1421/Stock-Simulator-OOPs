#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <iostream>
#include "Stock.h" 
#include "trader.h"
using namespace std;

class Transaction {
private:
    string type;   
    string stock_id;
    int quantity;
    float price;
    string timestamp; 

    void setTimestamp(); 
    friend class Trader;
public:
    Transaction();
    Transaction(string type, string stock_id, int quantity, float price); 

    void recordBuy(Stock& stock, int qty);
    void recordSell(Stock& stock, int qty);

    void display() const;

    string getType() const;
    string getStockId() const;
    int getQuantity() const;
    float getPrice() const;
    string getTimestamp() const;
};

#endif