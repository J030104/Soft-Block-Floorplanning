#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <iterator>
#include <algorithm>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include "seq_pair.h"
#include "file_parser.h"

using namespace std;

SP::SP() {

}

SP::SP(const vector<Macro>& M) {
    v = M.size();
    seqPos.resize(v);
    seqNeg.resize(v);
    
    // Mapping the macro to its area
    vector<pair<int, int>> area(M.size());
    for (int i = 0; i < M.size(); ++i) {
        Macro m = M[i];
        area[i].first = i;
        area[i].second = m.w * m.h;
    }
    // Sorting : Larger ---> Smaller
    // sort(area.begin(), area.end(), 
    //     [](const pair<int, int>& a, const pair<int, int>& b) {
    //         return a.second > b.second; });
    
    // Initialization
    for (int i = 0; i < v; ++i) {
        // Larger macros  -> left-down
        // Smaller macros -> right-up
        // seqNeg[i] = area[i].first;
        seqPos[i] = i; // Simple
        seqNeg[i] = i; // Simple
    }
    this->random_P();
    
    // Special: from left-down to right-up
    // seqNeg = {9, 0, 8, 1, 7, 2, 6, 3, 5, 4};
    // seqPos = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    // for (int i = 1; i < v; i += 2) {
    //     seqNeg[i] = i / 2;
    // }
    // for (int i = 0; i < v; i += 2) {
    //     seqNeg[i] = (v - 1) - i / 2;
    // }

    // areaLB = 0;
    int areaLB = 0;
    for(const auto& i : area) {
        areaLB += i.second;
    }
    cout << "\nLower bound of the total area: " << areaLB << endl;

    // cout << "Gamma + : \n";
    // for (const auto block : seqPos) {
    //     cout << block << " ";
    // }
    // cout << endl;
    
    // cout << "Gamma - : \n";
    // for (const auto block : seqNeg) {
    //     cout << block << " ";
    // }
    // cout << endl;
}

int SP::perturb() {
    random_device rd; // Obtain a random seed
    mt19937 gen(rd()); // Seed the generator

    uniform_int_distribution<int> mode(1, 2);
    int m = mode(gen);
    switch (m) {
        int num1, num2;
        case 1: // Single Swap
        {
            uniform_int_distribution<int> sel(0, 1);
            vector<int>& seq = (sel(gen) == 0) ? seqPos : seqNeg;
            uniform_int_distribution<int> elem(0, v - 1);
            num1 = elem(gen);
            do {
                num2 = elem(gen);
            } while (num2 == num1); // Make num2 != num1
            swap(seq[num1], seq[num2]);
            break;
        }
        case 2: // Double Swap
        {
            uniform_int_distribution<int> elem(0, v - 1);
            num1 = elem(gen);
            do {
                num2 = elem(gen);
            } while (num2 == num1); // Make num2 != num1
            swap(seqPos[num1], seqPos[num2]);
            swap(seqNeg[num1], seqNeg[num2]);
            break;
        }
        default:
            exit(55);
    }

    return m;
}

void SP::random_P() {
    random_device rd;
    mt19937 gen(rd());

    shuffle(seqPos.begin(), seqPos.end(), gen);
    shuffle(seqNeg.begin(), seqNeg.end(), gen);
}

void SP::translate() { // Help in finding the indices of a macro in two sequences
    for (int i = 0; i < v; ++i) {
        // int pos_index = 0;
        // auto it = find(seqPos.begin(), seqPos.end(), i);
        // if (it != seqPos.end()) {
        //     int index = distance(seqPos.begin(), it);
        //     // y = v - 1 - index;
        //     pos_index = index;
        // } else {
        //     cout << "Error: Invalid Packing - seqPos" << endl;
        //     exit(-87);
        // }

        int neg_index = 0;
        auto it = find(seqNeg.begin(), seqNeg.end(), i);
        if (it != seqNeg.end()) {
            int index = distance(seqNeg.begin(), it);
            neg_index = index;
        } else {
            cout << "Error: Invalid Packing - seqNeg" << endl;
            exit(-88);
        }
        // match[i] = make_pair(pos_index, neg_index); // memory wastage and time-inefficient 
        match[i] = neg_index;
    }

}

void SP::calcCoords(vector<Macro>& M) {
    this->translate(); // Translate the SPs to coords first
    vector<int> seqPos_R = seqPos;
    reverse(seqPos_R.begin(), seqPos_R.end());

    vector<int> L(M.size(), 0);
    for (int i = 0; i < v; ++i) {
        // Current block IDs
        int b = seqPos[i];
        // The indices in the Gamma-
        // int p = match[b].second;
        int p = match[b];

        M[b].x1 = L[p];
        int t = M[b].x1 + M[b].w;
        for (int j = p; j < v; ++j) {
            if (t > L[j]) {
                L[j] = t;
            }
            else {
                break;
            }
        }
    }
    // max_w = *max_element(L.begin(), L.end()); // Not necessary
    max_w = L.back();
    
    fill(L.begin(), L.end(), 0); // Using resize() here does not work
    for (int i = 0; i < v; ++i) {
        // Current block IDs
        int b = seqPos_R[i];
        // The indices in the Gamma-
        // int p = match[b].second;
        int p = match[b];

        M[b].y1 = L[p];
        int t = M[b].y1 + M[b].h;
        for (int j = p; j < v; ++j) {
            if (t > L[j]) {
                L[j] = t;
            }
            else {
                break;
            }
        }
    }
    max_h = L.back();
    
    for (auto& i : M) {
        i.findx2y2();
    }
}

void SP::calcCost(const vector<Macro>& M) {
    this->translate(); // Translate the SPs to coords first
    vector<int> seqPos_R = seqPos;
    reverse(seqPos_R.begin(), seqPos_R.end());

    vector<int> L(M.size(), 0);
    for (int i = 0; i < v; ++i) {
        // Current block IDs
        int b = seqPos[i];
        int p = match[b];

        int t = L[p] + M[b].w;
        for (int j = p; j < v; ++j) {
            if (t > L[j]) {
                L[j] = t;
            }
            else {
                break;
            }
        }
    }
    max_w = L.back();
    
    fill(L.begin(), L.end(), 0); // Using resize() here does not work
    for (int i = 0; i < v; ++i) {
        // Current block IDs
        int b = seqPos_R[i];
        int p = match[b];

        int t = L[p] + M[b].h;
        for (int j = p; j < v; ++j) {
            if (t > L[j]) {
                L[j] = t;
            }
            else {
                break;
            }
        }
    }
    max_h = L.back();
}

int SP::getCost() {
    // Cost = w * h * (max(w, h) / min(w, h))
    // It's actually calculating the area by squaring the longest side.
    // However, taking the longer side as the cost may be wise sometimes
    if (max_w > max_h) {
        // return max_w * max_w;
        return max_w;
    } else {
        // return max_h * max_h;
        return max_h;
    }
}

void rand_rot(vector<Macro>& M) {
    random_device rd;
    mt19937 gen(rd());

    unordered_set<int> num; // It contains the elements to be rotated
    uniform_int_distribution<int> rs(1, 4); // 4 is chosen for no reason
    uniform_int_distribution<int> elem(0, M.size() - 1);
    int rand_size = rs(gen);

    while (num.size() < rand_size) {
        num.insert(elem(gen));
    }
    for (const auto& i : num) {
        M[i].swapwh();
    }
}