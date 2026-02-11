#include "sip.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;

SIP::SIP() : company(""), durationMonths(12), interestRate(0.0) {}

SIP::SIP(string name, int duration, double rate)
    : company(name), durationMonths(duration), interestRate(rate) {}

SIP::SIP(string name) {
    company = name;
    durationMonths = 12;
    interestRate = 0.0;

    string path = "companies/" + name + ".json";
    ifstream file(path);
    if (!file.is_open()) return;

    json data;
    file >> data;
    file.close();

    if (data.contains("sipRate"))
        interestRate = data["sipRate"].get<double>();
}

float SIP::ProfitCalculator(float monthlyInvestment, int months) const {
    if (monthlyInvestment <= 0 || months <= 0) return 0;

    double r = interestRate / 100.0 / 12.0; 
    double total = 0;

    for (int i = 0; i < months; i++)
        total = (total + monthlyInvestment) * (1 + r);

    return total;
}

void SIP::displaySIPDetails() const {
    cout << "\n--- SIP DETAILS ---\n";
    cout << "Company: " << company << endl;
    cout << "Default Tenure: " << durationMonths << " months\n";
    cout << "SIP Rate: " << interestRate << "%\n";
}
