#include<iostream>
using namespace std;
#define token_amount 5000
#include<fstream>
#include "json.hpp" 
using json = nlohmann::json;
#include<filesystem>
#include<iomanip>
class System
{// dummy code for testing can be changed
    public:
    float get_worth(json user){
        return ((user["stocks_number"].get<float>()*user["stock_worth"].get<float>())+user["current_money"].get<float>()); 
    }
};
class Leaderboard{
    json user;
    string folder_path="UserData";
    vector<json> rankings;
    public:
    Leaderboard(){
        for(auto entry:filesystem::directory_iterator(folder_path)){
            ifstream in(entry.path());
            in>>user;
            System s;
            float worth=s.get_worth(user);
            float diff=worth-token_amount;
            float percent_change=(diff/token_amount)*100;
            string change;
            if (percent_change > 0) {
                change=to_string(percent_change) + "%"+"profit";
            } else if (percent_change < 0) {
                change=to_string(percent_change) + "%"+"loss";
            } else {
                change="constant";
            }
            json temp;
            temp["username"]=user["username"];
            temp["worth"]=worth;
            temp["change"]=change;
            rankings.push_back(temp);
            in.close();
        }
        sort(rankings);
        ofstream out("Leaderboard.json");
        for(int i=0;i<rankings.size();i++){
            rankings[i]["rank"]=i+1;
        }
        json output=rankings;
        out<<output.dump(6);
        out.close();
    }
    void sort(vector<json>&rankings){
        for(int i=rankings.size()-1;i>=0;i--){
            for(int j=0;j<i;j++){
                if(rankings[j]["worth"]<rankings[j+1]["worth"]){
                    swap(rankings[j],rankings[j+1]);
                }
            }
        }
    }
    void show_leaderboard() {
        ifstream in("Leaderboard.json");
        json data;
        in >> data;

        cout << "\n==================== LEADERBOARD ====================\n";
        cout << left << setw(6) << "RANK"
            << setw(15) << "USERNAME"
            << setw(20) << "CURRENT WORTH"
            << "PERCENT CHANGE" << "\n";
        cout << string(65, '-') << "\n";

        for (auto &temp : data) {
            int rank = temp["rank"].get<int>();
            string username = temp["username"].get<string>();
            float worth = temp["worth"].get<float>();
            string change = temp["change"].get<string>();

            cout << left << setw(6) << rank
                << setw(15) << username
                << setw(20) << worth
                << change << "\n";
        }

        in.close();
    }


    void show_top_5() {
        ifstream in("Leaderboard.json");
        json data;
        in >> data;

        cout << "\n==================== TOP 5 PLAYERS ====================\n";
        cout << left << setw(8)  << "RANK"
            << setw(20) << "USERNAME"
            << setw(20) << "CURRENT WORTH"
            << setw(20) << "PERCENT CHANGE" << "\n";
        cout << string(70, '-') << "\n";

        for (int i = 0; i < min(5, (int)data.size()); i++) {
            json temp = data[i];
            int rank = temp["rank"].get<int>();
            string username = temp["username"].get<string>();
            float worth = temp["worth"].get<float>();
            string change = temp["change"].get<string>();

            cout << left << setw(8)  << rank
                << setw(20) << username
                << setw(20) << worth
                << setw(20) << change
                << "\n";
        }

        in.close();
    }
    ~Leaderboard()=default;
};
int main() {
    int ch;
    cout<<"1.Show Leaderboard\n2.EXIT\n";
    cin>>ch;
    while(1){
        if(ch==2){
            break;
        }
        Leaderboard lb;
        cout<<"See Complete Leaderboard or Top 5 Players?\n1.Complete Leaderboard\n2.Top 5 Players\n";
        int ch2;
        cin>>ch2;
        switch(ch2){
            case 1:
                lb.show_leaderboard();
                break;
            case 2:
                lb.show_top_5();
                break;
            default:
                cout<<"Invalid Choice\n";
                break;
        }
        cout<<"1.Show Leaderboard\n2.EXIT\n";
        cin>>ch;            
    }
    return 0;
}