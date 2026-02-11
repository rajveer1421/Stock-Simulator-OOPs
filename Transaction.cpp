#include "Transaction.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

using namespace std;

void Transaction::setTimestamp() {
    time_t now = time(0);
    tm* ltm = localtime(&now);

    stringstream ss;
    ss << 1900 + ltm->tm_year << "-"
       << setfill('0') << setw(2) << 1 + ltm->tm_mon << "-"
       << setfill('0') << setw(2) << ltm->tm_mday << " "
       << setfill('0') << setw(2) << ltm->tm_hour << ":"
       << setfill('0') << setw(2) << ltm->tm_min << ":"
       << setfill('0') << setw(2) << ltm->tm_sec;

    timestamp = ss.str();
}

Transaction::Transaction() : type("none"), stock_id(""), quantity(0), price(0.0f), timestamp("") {}

Transaction::Transaction(string type, string stock_id, int quantity, float price)
    : type(type), stock_id(stock_id), quantity(quantity), price(price) {
    setTimestamp();
}

void Transaction::recordBuy(Stock& stock, int qty) {
    type = "buy";
    stock_id = stock.getStockId();
    quantity = qty;
    price = stock.getPrice();
    setTimestamp();

    cout << " Bought " << qty 
         << " shares of " << stock.getCompanyName() 
         << " @ Rs. " << fixed << setprecision(2) << price 
         << " each\n";
}

void Transaction::recordSell(Stock& stock, int qty) {
    type = "sell";
    stock_id = stock.getStockId();
    quantity = qty;
    price = stock.getPrice();
    setTimestamp();

    cout << " Sold " << qty 
         << " shares of " << stock.getCompanyName() 
         << " @ Rs. " << fixed << setprecision(2) << price 
         << " each\n";
}


void Transaction::display() const {
    if (type == "none") return;

    cout << "\n--- TRANSACTION ---\n";
    cout << "Time: " << timestamp << endl;
    cout << "Type: " << type << endl;
    cout << "Stock: " << stock_id << endl;
    cout << "Qty: " << quantity << endl;
    cout << "Price: Rs. " << fixed << setprecision(2) << price << endl;
    cout << "--------------------\n";
}

string Transaction::getType() const { return type; }
string Transaction::getStockId() const { return stock_id; }
int Transaction::getQuantity() const { return quantity; }
float Transaction::getPrice() const { return price; }
string Transaction::getTimestamp() const { return timestamp; }
