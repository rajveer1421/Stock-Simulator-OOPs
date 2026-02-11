#ifndef TRADER_H
#define TRADER_H

#include <string>
#include <vector>
#include <unordered_map>
#include "Stock.h"
#include "Transaction.h"
#include "user.h"
using namespace std;

class Trader :public User {
private:
    string name;
    string password;
    float balance;
    float initial_balance;   // ✅ Added

    unordered_map<string, int> portfolio;
    vector<Transaction> transaction_history;
    

public:
    // Constructors
    Trader();
    Trader(string name);
    Trader(string name, string password, float initial_balance = 0);

    // Account management
    bool loginCheck(string traderName);
    void createNewAccount();
    void updateDetail();

    // Market actions
    void viewMarket(const vector<Stock>& market) const;
    void buyStock(const string& companyName, int quantity);
    void sellStock(const string& companyName, int quantity);

    // Portfolio
    void viewInvestments() const;
    float viewNetHoldings() const;
    void viewDetails() const;

    // Money
    void deposit(float amount);
    bool withdraw(float amount);

    // SIP calc
    float sipCalculator(float monthlyInvestment, float annualRate, int years) const;
    void updatePassword(const string& newPass);
    bool isValid(string password);
    // Getters
    string getName() const;
    float getBalance() const;
};

#endif
