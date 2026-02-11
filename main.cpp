#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include "trader.h"
#include "company.h"   
#include "Stock.h"
#include "leaderboard.h"

using namespace std;
namespace fs = std::filesystem;

void traderMenu(Trader& trader, vector<Stock>& market);
void companyMenu(Company& company);

bool fileExists(const string& path) { 
    return fs::exists(path); 
}


void leaderboardMenu() {
    int ch;
    cout << "\n====== LEADERBOARD ======\n";
    cout << "1. Show Leaderboard\n2. Exit Leaderboard Menu\n";
    cin >> ch;

    while (true) {
        if (ch == 2) break;

        Leaderboard lb;

        cout << "\nSee Complete Leaderboard or Top 5 Players?\n";
        cout << "1. Complete Leaderboard\n2. Top 5 Players\n";
        int ch2;
        cin >> ch2;

        switch (ch2) {
            case 1: lb.show_leaderboard(); break;
            case 2: lb.show_top_5(); break;
            default: cout << "Invalid Choice\n";
        }

        cout << "\n1. Show Leaderboard\n2. Exit Leaderboard Menu\n";
        cin >> ch;
    }
}

bool isValid(string password) {
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

int main() {
    vector<Stock> market; 

    while (true) {
        cout << "\n====== STOCK MARKET SYSTEM ======\n";
        cout << "1. Trader Login\n";
        cout << "2. Trader Register\n";
        cout << "3. Company Login\n";
        cout << "4. Company Register\n";
        cout << "5. View Leaderboard\n";
        cout << "6. Exit\n";
        cout << "Select option: ";

        int choice;
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            string name;
            cout << "Enter Trader Name: ";
            getline(cin, name);
            string filename = "Traders/" + name + ".json";

            if (!fileExists(filename)) {
                cout << "No trader account found. Please register first.\n";
                continue;
            }

            Trader trader(name);
            if (trader.loginCheck(name)) {
                traderMenu(trader, market);
            } else {
                cout << "Login failed. Wrong password.\n";
            }
        }

        else if (choice == 2) { 
            Trader trader;
            trader.createNewAccount();
        }

        else if (choice == 3) {
            string name, pass;
            cout << "Enter Company Name: ";
            getline(cin, name);
            string filename = "companies/" + name + ".json";

            if (!fileExists(filename)) {
                cout << "No company found. Please register first.\n";
                continue;
            }

            Company company(name);
            cout << "Enter Password: ";
            getline(cin, pass);

            if (company.verifyPassword(pass)) {
                companyMenu(company);
            } else {
                cout << "Invalid password.\n";
            }
        }

        else if (choice == 4) {
            string name, pass;
            double val, rate;
            cout << "Enter Company Name: ";
            getline(cin, name);
            string filename = "companies/" + name + ".json";

            if (fileExists(filename)) {
                cout << "Company already exists. Try logging in.\n";
                continue;
            }

            cout << "Set Password: ";
            cin>> pass;
            while(!isValid(pass)) {
                cout<<"Password must be at least 8 characters long and should contain number,capital alphabet and a symbol.\n";
                cout<<"Re-enter password: ";
                cin>>pass;
            }
            cout << "Enter Initial Valuation: ";
            cin >> val;
            cout << "Enter SIP Return Rate (%): ";
            cin >> rate;
            cin.ignore();

            Company company(name, pass, val, rate);
            company.makeCompanyFile();
            cout << "Company registered successfully!\n";
        }

        else if (choice == 5) {
            leaderboardMenu();
        }

        else if (choice == 6) {
            cout << "Exiting...\n";
            break;
        }

        else {
            cout << "Invalid option. Try again.\n";
        }
    }

    return 0;
}

// ===== Trader Menu =====
void traderMenu(Trader& trader, vector<Stock>& market) {
    int choice = 0;
    while (choice != 9) {
        cout << "\n--- Trader Menu ---\n";
        cout << "1. View Market\n";
        cout << "2. Buy Stock\n";
        cout << "3. Sell Stock\n";
        cout << "4. View Investments\n";
        cout << "5. Deposit / Withdraw\n";
        cout << "6. View Details\n";
        cout << "7. Update Password\n";  
        cout << "8. SIP Calculator\n";   // ✅ Now before logout
        cout << "9. Logout\n";           // ✅ Logout last
        cout << "Choose: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {

        case 1:
            trader.viewMarket(market);
            break;

        case 2: {
            string cname; int qty;
            cout << "Enter Company Name: ";
            getline(cin, cname);
            cout << "Enter Quantity: ";
            cin >> qty; cin.ignore();
            trader.buyStock(cname, qty);
            break;
        }

        case 3: {
            string cname; int qty;
            cout << "Enter Company Name: ";
            getline(cin, cname);
            cout << "Enter Quantity: ";
            cin >> qty; cin.ignore();
            trader.sellStock(cname, qty);
            break;
        }

        case 4:
            trader.viewInvestments();
            break;

        case 5: {
            float amt;
            cout << "Enter amount (+deposit, -withdraw): ";
            cin >> amt;
            if (amt >= 0) trader.deposit(amt);
            else trader.withdraw(-amt);
            break;
        }

        case 6:
            trader.viewDetails();
            break;

        case 7: {
            string newPass;
            cout<<"Enter new password:\n";
            cin>>newPass;
            trader.updatePassword(newPass);
            break;
        }

        case 8: {
            string comp;
            cout << "Enter Company Name for SIP: ";
            getline(cin, comp);

            SIP sip(comp);
            float monthlyInvestment;
            cout << "Enter Monthly Investment Amount  ";
            cin >> monthlyInvestment;

            int years;
            cout << "Enter Tenure (Years): ";
            cin >> years;
            cin.ignore();

            int months = years * 12;
            float finalValue = sip.ProfitCalculator(monthlyInvestment, months);
            float invested = monthlyInvestment * months;
            float profit = finalValue - invested;

            cout << "\n======= SIP CALCULATION RESULT =======\n";
            cout << "Company           : " << comp << endl;
            cout << "Monthly Invest    : " << monthlyInvestment << endl;
            cout << "Tenure            : " << years << " years (" << months << " months)" << endl;
            cout << "Total Invested    : " << invested << endl;
            cout << "Expected Profit   : " << profit << endl;
            cout << "Final Maturity    : " << finalValue << endl;
            cout << "=====================================\n";
            break;
        }

        case 9:
            cout << "Logging out...\n";
            return;

        default:
            cout << "Invalid choice.\n";
        }
    }
}

// ===== Company Menu =====
void companyMenu(Company& company) {
    int choice = 0;
    while (choice != 5) {
        cout << "\n--- Company Menu ---\n";
        cout << "1. View Details\n";
        cout << "2. Raise Stock\n";
        cout << "3. Change SIP Rate\n";
        cout << "4. Update Valuation\n";
        cout << "5. Logout\n";
        cout << "Choose: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1: company.viewCompanyDetails(); break;
            case 2: {
                int qty; cout << "Enter stock quantity to raise: ";
                cin >> qty; 
                company.raiseStock(qty);
                break;
            }
            case 3: {
                double rate; cout << "Enter new SIP Rate: ";
                cin >> rate;
                company.setSipReturnRate(rate);
                break;
            }
            case 4: {
                double val; cout << "Enter new valuation: ";
                cin >> val;
                company.setValuation(val);
                break;
            }
            case 5: cout << "Logging out...\n"; break;
            default: cout << "Invalid option.\n";
        }
    }
}
