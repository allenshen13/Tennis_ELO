//
//  rankings.h
//  tennis ranking system
//  Created by Allen Shen 
//

#ifndef rankings_h
#define rankings_h

#include <string>
#include <vector>
#include <unordered_map>
#include <vector>
#include <utility>
#include "utility.h"

using std::string;
using std::unordered_map;
using std::vector;
using std::ofstream;
using std::pair;

class TennisRankings {
public:
    // INPUT: input filename
    TennisRankings(string filename);

    // INPUT: output filename
    void calculate(string filename);

private:

  
   
    //--------------------------------------------------------------------------
    // HELPER STRUCTS
    //--------------------------------------------------------------------------
    // represents player data of one score
    struct PlayerScore {
        PlayerScore(string& name, double score) : name{ name }, score{ score } {}
        string name;
        double score;
    };

    // attributes of a player
    struct PlayerData {
        PlayerData() : rating{ 0 }, wins{ 0 }, losses{ 0 } {}
        PlayerData(double rating, int wins, int losses) : rating{ rating }, wins{ wins }, losses{ losses } {}
        double rating;
        int wins;
        int losses;
    };

    struct triplet {
        string name;
        int win;
        int loss;

    };

    //--------------------------------------------------------------------------
    // PRIVATE MEMBER FUNCTIONS
    //--------------------------------------------------------------------------
    // reads in data from file
    void read_data(string& filename);
    // updates rankings based on one cup
    void update(vector<PlayerScore>& matches);
    void write_data(ofstream& fout);
    void write_preliminary_data(ofstream& fout);
    void play_match(vector<PlayerScore>& matches, ofstream& fout, int matchNum);
    void write_end_data(ofstream& fout);

    //--------------------------------------------------------------------------
    // PRIVATE MEMBER VARIABLES
    //--------------------------------------------------------------------------
    unordered_map<string, PlayerData> player_data;
    vector<vector<PlayerScore>> matches;
    vector<vector<int>> scores;
    unordered_map<string, PlayerData> pre_data;
    unordered_map<string, double> pre_rank;
    unordered_map<string, int> played; // counts number of matches played for each player
};


#endif /* rankings_h */
