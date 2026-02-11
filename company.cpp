#include "Company.h"
#include "Stock.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;

Company::Company() {}

Company::Company(string name, string pass, double val, double sipRate)
    : companyName(name), password(pass), valuation(val), sip(name, 12, sipRate) {}

Company::Company(string name) {
    companyName = name;

    string companyPath = "companies/" + name + ".json";
    ifstream companyFile(companyPath);
    if (!companyFile.is_open()) {
        cerr << "Error: Could not open " << companyPath << endl;
        return;
    }

    json data;
    companyFile >> data;
    companyFile.close();

    password = data.value("password", "");
    valuation = data.value("valuation", 0.0);
    double rate = data.value("sipRate", 0.0);
    sip = SIP(name, 12, rate);

    string stockPath = "Stocks/" + name + ".json";
    ifstream stockFile(stockPath);

    if (stockFile.is_open()) {
        json sData; stockFile >> sData; stockFile.close();
        float price = sData.value("current_price", 100.0f);
        int available = sData.value("stock_available", 0);
        int sold = sData.value("stock_sold", 0);
        stock = Stock(name, price, available, sold);
    } else {
        stock = Stock(name, 100.0f, 0, 0);
        stock.updateDetail();
    }
}

void Company::makeCompanyFile() {
    fs::create_directories("companies");

    json data;
    data["name"] = companyName;
    data["password"] = password;
    data["valuation"] = valuation;
    data["sipRate"] = sip.getInterestRate();

    ofstream file("companies/" + companyName + ".json");
    file << data.dump(4);
    file.close();
}

void Company::makeStockFile() {
    fs::create_directories("Stocks");

    float price; 
    int available, sold;

    cout << "\n--- Enter Stock Details for " << companyName << " ---\n";
    cout << "Enter current stock price (Rs): ";
    cin >> price;
    cout << "Enter available stock quantity: ";
    cin >> available;
    cout << "Enter sold stock quantity: ";
    cin >> sold;

    stock = Stock(companyName, price, available, sold);
    stock.updateDetail();
}

void Company::setCompanyName(const string& name) { companyName = name; }
string Company::getCompanyName() const { return companyName; }

string Company::getCompanyID() const {
    return companyName + "_ID";
}

void Company::setPassword(const string& pass) { password = pass; }
bool Company::verifyPassword(const string& inputPass) const { return password == inputPass; }

void Company::setValuation(double val) { 
    valuation = val; 
    updateDetails(); 
}

double Company::getValuation() const { return valuation; }

void Company::setSipReturnRate(double rate) { 
    sip = SIP(companyName, 12, rate); 
    updateDetails(); 
}

double Company::getSipReturnRate() const { return sip.getInterestRate(); }

void Company::raiseStock(int quantity) {
    fs::create_directories("Stocks");

    string stockPath = "Stocks/" + companyName + ".json";
    if (fs::exists(stockPath)) {
        Stock existingStock(companyName);
        existingStock.addQuantity(quantity);
        existingStock.updateDetail();
        cout << quantity << " stocks added for " << companyName << "\n";
    } else {
        Stock newStock(companyName, 100.0f, quantity, 0);
        newStock.updateDetail();
        cout << "New stock file created & " << quantity << " stocks added.\n";
    }
}

void Company::updateDetails() {
    fs::create_directories("Companies");

    json data;
    data["name"] = companyName;
    data["password"] = password;
    data["valuation"] = valuation;
    data["sipRate"] = sip.getInterestRate();

    ofstream out("companies/" + companyName + ".json");
    out << data.dump(4);
    out.close();
}

void Company::viewCompanyDetails() {
    cout << "\n=== Company Details ===\n";
    cout << "Name: " << companyName << endl;
    cout << "Valuation: " << valuation << endl;
    cout << "SIP Rate: " << sip.getInterestRate() << "%" << endl;
    cout << "\n--- Stock Info ---\n";
    stock.display();
}
