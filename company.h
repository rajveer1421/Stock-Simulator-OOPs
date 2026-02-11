#ifndef COMPANY_H
#define COMPANY_H

#include <iostream>
#include <string>
#include <fstream>
#include "json.hpp"
#include "Stock.h"
#include "SIP.h"

using namespace std;
using json = nlohmann::json;

class Company {
private:
    string companyName;
    string password;
    double valuation;
    Stock stock;
    SIP sip;

public:
    Company();
    Company(string name, string pass, double val, double sipRate);
    Company(string name);

    void makeCompanyFile();
    void makeStockFile();

    void setCompanyName(const string& name);
    string getCompanyName() const;
    string getCompanyID() const;

    void setPassword(const string& pass);
    bool verifyPassword(const string& inputPass) const;

    void setValuation(double val); 
    double getValuation() const;

    void setSipReturnRate(double rate);
    double getSipReturnRate() const;

    void raiseStock(int quantity);
    void updateDetails(); 
    void viewCompanyDetails();
};

#endif
