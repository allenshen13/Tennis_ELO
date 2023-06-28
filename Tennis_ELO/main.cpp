// Tennis_ELO
// main.cpp
// Allen Shen

#include <iostream>
#include <string>

#include "rankings.h"

using namespace std;

int main(int argc, const char* argv[]) {
    string infile = "/Users/shena/Documents/Projects/Tennis_ELO/Tennis_ELO/girlsData.txt";
    string outfile = "/Users/shena/Documents/Projects/Tennis_ELO/Tennis_ELO/girlsResults.txt";
    TennisRankings rankings(infile);
    rankings.calculate(outfile);
    return 0;
}