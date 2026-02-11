#include "Stock.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;
namespace fs = std::filesystem;

Stock::Stock()
    : company_name("Unknown"), current_price(0.0f), stock_available(0), stock_sold(0) {}

Stock::Stock(string company_name, float current_price, int stock_available, int stock_sold)
    : company_name(company_name), current_price(current_price), stock_available(stock_available), stock_sold(stock_sold) {}

Stock::Stock(string company_name) {
    this->company_name = company_name;

    string filename = "Stocks/" + company_name + ".json";
    ifstream file(filename);

    if (!file.is_open()) {
        current_price = 0.0f;
        stock_available = 0;
        stock_sold = 0;
        return;
    }

    json data;
    file >> data;
    file.close();

    this->company_name = data.value("company_name", company_name);
    current_price = data.value("current_price", 0.0f);
    stock_available = data.value("stock_available", 0);
    stock_sold = data.value("stock_sold", 0);
}

void Stock::display() const {
    cout << "\n------------------ STOCK DETAIL ------------------\n";
    cout << "Company Name: " << company_name << endl;
    cout << "Current Price: Rs. " << fixed << setprecision(2) << current_price << endl;
    cout << "Available Inventory: " << stock_available << endl;
    cout << "Total Sold Units: " << stock_sold << endl;
    cout << "--------------------------------------------------\n";
}

void Stock::updateDetail() {
    fs::create_directories("Stocks");

    string filename = "Stocks/" + company_name + ".json";
    json data;

    data["company_name"] = company_name;
    data["current_price"] = current_price;
    data["stock_available"] = stock_available;
    data["stock_sold"] = stock_sold;

    ofstream file(filename);
    file << setw(4) << data;
    file.close();
}

string Stock::getCompanyName() const { return company_name; }
float Stock::getPrice() const { return current_price; }
int Stock::getQuantity() const { return stock_available; }
string Stock::getStockId() const { return company_name; }

void Stock::addQuantity(int qty) {
    if (qty > 0) {
        stock_available += qty;
        updateDetail();
    }
}

void Stock::soldQuantity(int qty) {
    if (qty > stock_available) {
        cerr << "Error: Cannot sell, not enough stock\n";
        return;
    }

    stock_available -= qty;
    stock_sold += qty;
    increasePrice(qty);
    updateDetail();
}

void Stock::increasePrice(int qty) {
    float increment = current_price * 0.02f * qty;
    current_price += increment;

    if (current_price < 1.0f) current_price = 1.0f;
}

void Stock::decreasePrice(int qty) {
    float decrement = current_price * 0.02f * qty;
    current_price -= decrement;

    if (current_price < 1.0f) current_price = 1.0f;
}

void Stock::stockLoss(int qty) {
    if (qty > 0) {
        stock_available += qty;
        stock_sold -= qty;
        decreasePrice(qty);
        updateDetail();
    }
}
