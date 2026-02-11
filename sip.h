#ifndef SIP_H
#define SIP_H

#include <string>
using namespace std;

class SIP {
private:
    string company;
    int durationMonths;
    double interestRate;

public:
    SIP();
    SIP(string name, int duration, double rate);
    SIP(string name);

    double getRate() const { return interestRate; }
    double getInterestRate() const { return interestRate; }
    int getDuration() const { return durationMonths; }

    float ProfitCalculator(float monthlyInvestment, int months) const;

    void displaySIPDetails() const;
};

#endif
