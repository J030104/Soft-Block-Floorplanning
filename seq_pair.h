#ifndef SEQ_PAIR_H
#define SEQ_PAIR_H

#include <vector>
#include <unordered_map>
#include "file_parser.h"

using namespace std;

typedef struct SP {
    int v;
    int max_w, max_h;
    vector<int> seqPos; // Gamma+
    vector<int> seqNeg; // Gamma-
    unordered_map<int, int> match;

    SP();
    SP(const vector<Macro>& M);
    void random_P(); // Random Permutation
    int getCost();
    int perturb(); // SA
    void translate(); // To be called in perturb and constructor
    void calcCoords(vector<Macro>& M); // LCS
    void calcCost(const vector<Macro>& M); // LCS
} SP;

void rand_rot(vector<Macro>& M);

#endif