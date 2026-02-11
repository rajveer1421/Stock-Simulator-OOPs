#include "trader.h"
#include "Stock.h"
#include "Transaction.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include "json.hpp"
#include <filesystem>
#include "market.h"

using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;

Trader::Trader() : name("Unknown"), password(""), balance(0), initial_balance(0) {}

Trader::Trader(string traderName) : name(traderName) {
    string path = "Traders/" + traderName + ".json";
    if (!fs::exists(path)) {
        cout << "No trader file found for " << name << ". Initialized empty trader.\n";
        password = "";
        balance = 0;
        initial_balance = 0;
        return;
    }

    ifstream file(path);
    json data;
    file >> data;
    file.close();

    password = data.value("password", "");
    balance = data.value("balance", 0.0f);
    initial_balance = data.value("initial_balance", balance); // ✅ Load initial balance

    portfolio.clear();
    if (data.contains("portfolio")) {
        for (auto& s : data["portfolio"]) {
            portfolio[s.value("company_name", string())] = s.value("quantity", 0);
        }
    }

    transaction_history.clear();
    if (data.contains("transactions")) {
        for (auto& t : data["transactions"]) {
            transaction_history.emplace_back(
                Transaction(t.value("type", string()),
                            t.value("stock_id", string()),
                            t.value("quantity", 0),
                            t.value("price", 0.0f))
            );
        }
    }
}

Trader::Trader(string traderName, string pass, float initial_balance)
    : name(traderName), password(pass), balance(initial_balance), initial_balance(initial_balance) {}

bool Trader::loginCheck(string traderName) {
    string path = "Traders/" + traderName + ".json";
    if (!fs::exists(path)) {
        cout << "No trader account found for '" << traderName << "'.\n";
        return false;
    }

    ifstream file(path);
    json data;
    file >> data;
    file.close();

    cout << "Enter password: ";
    string inputPass;
    cin >> inputPass;

    if (data.value("password", string()) != inputPass) {
        cout << "Incorrect password.\n";
        return false;
    }

    name = traderName;
    password = data.value("password", string());
    balance = data.value("balance", 0.0f);
    initial_balance = data.value("initial_balance", balance); 

    portfolio.clear();
    transaction_history.clear();

    if (data.contains("portfolio")) {
        for (auto& s : data["portfolio"])
            portfolio[s.value("company_name", string())] = s.value("quantity", 0);
    }

    if (data.contains("transactions")) {
        for (auto& t : data["transactions"]) {
            transaction_history.emplace_back(
                Transaction(t.value("type", string()),
                            t.value("stock_id", string()),
                            t.value("quantity", 0),
                            t.value("price", 0.0f))
            );
        }
    }

    cout << "Login successful! Welcome back, " << name << ".\n";
    return true;
}
bool Trader::isValid(string password) {
    if (password.length() <= 8)
        return false;

    bool hasUpper = false, hasDigit = false, hasSymbol = false;

    for (char c : password) {
        if (isupper(c)) hasUpper = true;
        else if (isdigit(c)) hasDigit = true;
        else if (!isalnum(c)) hasSymbol = true;  
    }

    return hasUpper && hasDigit && hasSymbol;
}

void Trader::createNewAccount() {
    fs::create_directories("Traders");

    cout << "Enter name: ";
    cin >> name;
    cout << "Set password: ";
    cin >> password;
    while(!isValid(password)) {
        cout<<"Password must be at least 8 characters long and should contain number,capital alphabet and a symbol.\n";
        cout<<"Re-enter password: ";
        cin>>password;
    }
    cout << "Enter initial balance: ";
    cin >> balance;
    initial_balance = balance;
    cin.ignore();

    string filename = "Traders/" + name + ".json";
    if (fs::exists(filename)) {
        cout << "Trader already exists.\n";
        return;
    }

    json data;
    data["name"] = name;
    data["password"] = password;
    data["initial_balance"] = initial_balance;
    data["balance"] = balance;
    data["portfolio"] = json::array();
    data["transactions"] = json::array();

    ofstream file(filename);
    file << setw(4) << data;
    file.close();
    cout << "Account created successfully.\n";
}

void Trader::updateDetail() {
    fs::create_directories("Traders");

    string filename = "Traders/" + name + ".json";
    json data;

    data["name"] = name;
    data["password"] = password;
    data["initial_balance"] = initial_balance; 
    data["balance"] = balance;

    data["portfolio"] = json::array();
    for (auto& p : portfolio) {
        json entry;
        entry["company_name"] = p.first;
        entry["quantity"] = p.second;
        data["portfolio"].push_back(entry);
    }

    data["transactions"] = json::array();
    for (auto& t : transaction_history) {
        json tr;
        tr["type"] = t.getType();
        tr["stock_id"] = t.getStockId();
        tr["quantity"] = t.getQuantity();
        tr["price"] = t.getPrice();
        data["transactions"].push_back(tr);
    }

    ofstream file(filename);
    file << setw(4) << data;
    file.close();
}

void Trader::viewMarket(const vector<Stock>&) const {
    cout << "\n------ Available Stocks in Market ------\n";
    Market::displayMarket();
    cout << "---------------------------------------\n";
}

void Trader::buyStock(const string& companyName, int quantity) {
    string stockFile = "Stocks/" + companyName + ".json";
    ifstream file(stockFile);
    if (!file.is_open()) { cout << "Stock not found.\n"; return; }

    json data;
    file >> data;
    file.close();

    Stock stock(companyName, data["current_price"], data["stock_available"], data["stock_sold"]);
    float total = stock.getPrice() * quantity;

    if (total > balance) { cout << "Insufficient balance.\n"; return; }
    if (quantity > stock.getQuantity()) { cout << "Not enough stock.\n"; return; }

    stock.soldQuantity(quantity);
    balance -= total;
    portfolio[companyName] += quantity;

    Transaction t;
    t.recordBuy(stock, quantity);
    transaction_history.push_back(t);

    stock.updateDetail();
    updateDetail();
}

void Trader::sellStock(const string& companyName, int quantity) {
    auto it = portfolio.find(companyName);
    if (it == portfolio.end() || it->second < quantity) {
        cout << "Not enough stock.\n";
        return;
    }

    string stockFile = "Stocks/" + companyName + ".json";
    ifstream file(stockFile);
    json data; file >> data; file.close();

    Stock stock(companyName, data["current_price"], data["stock_available"], data["stock_sold"]);
    float total = stock.getPrice() * quantity;

    it->second -= quantity;
    if (it->second == 0) portfolio.erase(it);

    balance += total;
    stock.stockLoss(quantity);

    Transaction t;
    t.recordSell(stock, quantity);
    transaction_history.push_back(t);

    stock.updateDetail();
    updateDetail();
}

void Trader::viewInvestments() const {
    cout << "\n------ Portfolio ------\n";
    if (portfolio.empty()) { cout << "No holdings.\n"; return; }

    for (const auto& p : portfolio) {
        string stockFile = "Stocks/" + p.first + ".json";
        ifstream file(stockFile);
        float price = 0.0f;

        if (file.is_open()) {
            json data; file >> data;
            price = data.value("current_price", 0.0f);
        }

        cout << p.first 
             << " | Qty: " << p.second
             << " | Price: Rs. " << price
             << " | Value: Rs. " << price * p.second << endl;
    }
}

float Trader::viewNetHoldings() const {
    float total = balance;
    for (const auto& p : portfolio) {
        string stockFile = "Stocks/" + p.first + ".json";
        ifstream file(stockFile);
        json s; file >> s;
        total += s.value("current_price", 0.0f) * p.second;
    }
    return total;
}

void Trader::deposit(float amount) {
    if (amount <= 0) { cout << "Invalid amount.\n"; return; }
    balance += amount;
    updateDetail();
}

bool Trader::withdraw(float amount) {
    if (amount <= 0 || amount > balance) return false;
    balance -= amount;
    updateDetail();
    return true;
}

void Trader::viewDetails() const {
    cout << "\n=== Trader Details ===\n";
    cout << "Name: " << name << "\n";
    cout << "Initial Balance: Rs. " << initial_balance << "\n";  // ✅ show it
    cout << "Current Balance: Rs. " << balance << "\n";
    cout << "Holdings count: " << portfolio.size() << "\n";
}

float Trader::sipCalculator(float monthlyInvestment, float annualRate, int years) const {
    if (monthlyInvestment <= 0 || years <= 0) return 0.0f;
    float r = annualRate / 100.0f;
    float rm = r / 12.0f;
    int n = years * 12;
    return monthlyInvestment * ((pow(1.0f + rm, n) - 1) / rm) * (1.0f + rm);
}
void Trader::updatePassword(const string& newPass) {
    password = newPass;

    updateDetail();

    cout << "Password updated successfully!\n";
}


string Trader::getName() const { return name; }
float Trader::getBalance() const { return balance; }
