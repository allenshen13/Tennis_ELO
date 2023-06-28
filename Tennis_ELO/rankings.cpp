#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include <map>
#include <iomanip>

#include "rankings.h"
#include "Utility.h"

using namespace std;

TennisRankings::TennisRankings(string filename) {
    TennisRankings::read_data(filename);
}

void TennisRankings::read_data(string& filename) {
    int num_players;
    ifstream fin;
    fin.open(filename);
    if (!fin.is_open()) {
        cout << "Error: " << filename << "  is an invalid filename" << endl;
        exit(1);
    }
    fin >> num_players;
    string name;
    double pre_rating;
    for (int i = 0; i < num_players; i++) {
        fin >> name >> pre_rating;

        player_data[name] = PlayerData(pre_rating, int(0), int(0));
    }


    pre_data = player_data; //create a copy of player data to hold initial ratings


    string result;
    string name1;
    string name2;
    char dash;

    while (getline(fin, result)) {
       
        if (result == "")
            continue;
        istringstream sin(result);
        sin >> name1;
        if (player_data.find(name1) == player_data.end()) {
            cout << "Error: " << name1 << "  is an invalid player name" << endl;
            exit(1);
        }
        player_data[name1].wins++;
        sin >> name2;
        if (player_data.find(name2) == player_data.end()) {
            cout << "Error: " << name2 << "  is an invalid player name" << endl;
            exit(1);
        }
        player_data[name2].losses++;

        int numSets;
        sin >> numSets;
        int score1;
        int score2;
        int p1Score = 0;
        int p2Score = 0;

        vector<int> mScore; //holds match score in vector, ex. [6][3][6][4] -> 6-3, 6-4
        for (int i = 0; i < numSets; i++) {
            sin >> score1 >> dash >> score2;
            p1Score += score1;
            p2Score += score2;
            mScore.push_back(score1);
            mScore.push_back(score2);
        }
        scores.push_back(mScore);
        
        vector<PlayerScore> matchData;  
        matchData.push_back(PlayerScore(name1, p1Score));
        matchData.push_back(PlayerScore(name2, p2Score));
        matches.push_back(matchData); // holds total games earned and match and the corresponding player name
    }
    fin.close();
}
   


void TennisRankings::calculate(string filename) {
    ofstream fout;
    fout.open(filename);
    if (!fout.is_open()) {
        cout << "Error: " << filename << "  is an invalid filename" << endl;
        exit(1);
    }
   
    fout << "\n \n";
    fout << "\t \t \t \t \t ----------" << endl;
    fout << "\t \t \t \t \t START DATA" << endl;
    fout << "\t \t \t \t \t ----------" << endl;
    TennisRankings::write_preliminary_data(fout); //prints initial table including rank, name, rating, wins, losses


    fout << "\n \n";
    fout << "\t \t \t \t \t ----------" << endl;
    fout << "\t \t \t \t \t MATCH DATA" << endl;
    fout << "\t \t \t \t \t ----------" << endl;
    fout << "-----------------------------------------------------------------" << endl;


    int matchNum = 0; //counts number of matches played so far
    for (auto m : matches) {
        TennisRankings::play_match(m, fout, matchNum);
        matchNum++;
    }
    fout << "\n \n";
    fout << "\t \t \t \t \t ----------" << endl;
    fout << "\t \t \t \t \t  END DATA" << endl;
    fout << "\t \t \t \t \t ---------- " << endl;

    TennisRankings::write_end_data(fout);
    fout.close();
}

void TennisRankings::update(vector<PlayerScore>& cup_data) {
    // initializes change for each player as 0 for this cup
    string p1Name = cup_data[0].name;
    string p2Name = cup_data[1].name;
    int s1 = cup_data[0].score; //player 1 score
    int s2 = cup_data[1].score; //player 2 score
    double r1 = player_data[p1Name].rating;
    double r2 = player_data[p2Name].rating;
    double diff = r1 - r2;
    
    bool upset = false;
    if (diff < 0) {
        upset = true; // if someone won who isn't supposed to win, aka an upset
    }

    //finds expected value between player 1 and 2
    double expected = pow(abs(diff) + 1.95, 1.63);
    double actual = double(s1) - double(s2);

    const double factor = 0.97;
   
    
    double scoreDiff = actual - expected;

    double mWeight1 = Utility::match_weight[played[p1Name]]; // match weighting based on number of matches played
    double mWeight2 = Utility::match_weight[played[p2Name]];

    if (upset) {
        double change = abs(sqrt(abs(actual + expected))/ r1);
        player_data[p1Name].rating += change * mWeight1 * factor;
        player_data[p2Name].rating -= change * mWeight2 * factor;
    }// in the case of an upset
    else if(scoreDiff > 0) { // no upset
        player_data[p1Name].rating += factor * pow(scoreDiff, 0.35) / 10 * mWeight1;
        player_data[p2Name].rating -= factor * pow(scoreDiff, 0.35) / 10 * mWeight2;
    }
    else {
        player_data[p1Name].rating += factor * pow(abs(scoreDiff), 0.35) / 10 * mWeight1;
        player_data[p2Name].rating -= factor * pow(abs(scoreDiff), 0.35) / 10 * mWeight2;
    }


    played[p1Name]++;
    played[p2Name]++;



}

void TennisRankings::play_match(vector<PlayerScore>& mData,
    ofstream& fout, int matchNum) {
    
    fout.precision(3);


    string p1 = mData[0].name;
    string p2 = mData[1].name;
    fout << "\t     " << p1 <<  "(" << player_data[p1].rating << ") " <<
        "def. " << p2 << "(" << player_data[p2].rating << "): ";

    
    for (int i = 0; i < scores[matchNum].size(); i++) {

        if ((i + 1) % 2 == 0) {
            fout << "-";
        }
        if (i % 2 == 0 && i != 0) {
            fout << " ";
        }
        fout << scores[matchNum][i];

    }
    fout << endl;

   
    update(mData);


    fout << "\t     New Ratings: " << p1 << ": " << player_data[p1].rating << " and " <<
        p2 << ": " << player_data[p2].rating << endl;

    fout << "-----------------------------------------------------------------" << endl;
    

}


void TennisRankings::write_preliminary_data(ofstream& fout) {
    fout.precision(3);
    fout << left << "------------------------------------------------------------------------" << endl;
    fout << left << setw(15) << "Rank" << setw(10) << "Name" << setw(10) << right << "Rating" <<
        setw(15) << "Wins" << setw(15) << "Losses" << endl;
    fout << left << "------------------------------------------------------------------------" << endl;
    map<double, vector<triplet>, greater<double>> ratings_temp;
    for (auto pair : player_data) {
        triplet t;
        t.win = pair.second.wins;
        t.loss = pair.second.losses;
        t.name = pair.first;
        ratings_temp[pair.second.rating].push_back(t);
    }
    int rank = 1;
    for (auto pair : ratings_temp) {
        for (int i = 0; i < pair.second.size(); i++) {
            fout << left << setw(15) << rank << setw(10) << pair.second[i].name << setw(10) << right <<
                pair.first << setw(14) << pair.second[i].win << setw(14) <<
                pair.second[i].loss << endl;
                pre_rank[pair.second[i].name] = rank;
                rank++;

        }
    }
    fout << endl;
}
void TennisRankings::write_end_data(ofstream& fout) {
    fout.precision(3);
    fout << left << "--------------------------------------------------------------------------" << endl;
    fout << left << setw(15) << "Rank" << setw(10) << "Name" << setw(10) << right << "Rating" <<
        setw(20) << "Rating Change" << setw(18) << "Prev. Ranking" << endl;
    fout << left << "--------------------------------------------------------------------------" << endl;
    map<double, vector<triplet>, greater<double>> ratings_temp;
    for (auto pair : player_data) {
        triplet t;
        t.win = pair.second.wins;
        t.loss = pair.second.losses;
        t.name = pair.first;
        ratings_temp[pair.second.rating].push_back(t);
    }

    int rank = 1;
    for (auto pair : ratings_temp) {
        for (int i = 0; i < pair.second.size(); i++) {
            double ratChange = pair.first - pre_data[pair.second[i].name].rating;
            ratChange = round(ratChange * 100.0) / 100.0;
            string rChange;
            if (ratChange > 0) {
                rChange = "+" + to_string(ratChange);
            }
            else {
                rChange = to_string(ratChange);
            }

            

           
            fout << left << setw(15) << rank << setw(10) << pair.second[i].name << setw(10) << right <<
                pair.first << setw(16) << rChange.substr(0, 5) << setw(18) <<
                pre_rank[pair.second[i].name] << endl;
           
            rank++;
        }
    }
    fout << endl;
}

