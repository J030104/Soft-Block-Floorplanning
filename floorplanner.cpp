// floorplanner.cpp
// Author: YYChen - May 5th, 2024 - Taipei

// This program was designed to optimize floorplanning.
// Maximum input size: 100
// Time limit: 600 seconds

/* Program flowchart
* -> Parse input file
* -> Floorplanning
*   -> Initialization
*   -> Simulated Annealing
*   -> Rotation
* -> Output result
*   -> Program output file (.txt)
*   -> gnuplot plot file (.gp)
*   -> Image file (Visualization) (.png)
*/

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <climits>
#include <random>
#include <chrono>
#include <iomanip>
#include "file_parser.h"
#include "seq_pair.h"

#define PINF INT_MAX

using namespace std;

// const int t = 600; // Time Limit

int main(int argc, char** argv) {
// int main() {
    if (argc != 3) {
        cout << "Usage:./FP Input.txt Output.txt" << endl;
        return 88;
    }

    chrono::steady_clock::time_point start = chrono::steady_clock::now();
    chrono::steady_clock::time_point here, last_update;
    chrono::steady_clock::time_point p1, p2;
    chrono::duration<double> duration, t_cool;

    string inputFile = argv[1];
    string outputFile = argv[2];
    // string inputFile = "PA2_testcases/floorplan_100.txt";
    // string outputFile = "output.txt";
    cout << "Input file: " << inputFile << endl;
    cout << "Output file: " << outputFile << endl; 

    // Parse inputFile
    Parser parser;
    vector<Macro> Macros = parser.parse(inputFile);
    
    // Initialize SPs
    SP currSP(Macros);
    SP tempSP;
    SP solSP = currSP; // Solution SP
    last_update = chrono::steady_clock::now(); // random_P is called in the constructor

    // Floorplanning
    // size_t rand_cnt = 0;
    int cost;
    int minCost = PINF;
    
    currSP.calcCost(Macros);
    cost = currSP.getCost();
    minCost = cost;
    solSP = currSP;

    random_device rd; // Obtain a random seed
    mt19937 gen(rd()); // Seed the generator
    uniform_real_distribution<double> dis(0.0, 1.0);
    
    // Perturbation + Simulated Annealing
    vector<int> mode(2, 0);
    size_t run = 0, run_best;
    currSP = solSP; // Recover to the optimal
    tempSP = currSP; // Initialize tempSP
    p1 = chrono::steady_clock::now(); // Starting time
    cout << "\n================   Perturbation  ================" << endl;
    do {
        int m = currSP.perturb();
        currSP.calcCost(Macros);
        cost = currSP.getCost();
        // cout << "Run" << setw(3) << run << ", Cost: " << cost << endl;

        // Output result (.txt file)
        // string tmpOutputFile = "Test_random_P/out_" + to_string(run) + ".txt";
        // parser.Output(tmpOutputFile, Macros, currSP.max_w, currSP.max_h);

        // // Visualize the floorplanning result
        // string gpFile = tmpOutputFile;
        // gpFile = tmpOutputFile.substr(0, tmpOutputFile.length() - 4); // Delete .txt
        // gpFile += ".gp";
        // parser.genPlot(tmpOutputFile, gpFile);

        int delta = cost - minCost;
        if (delta < 0) {
            run_best = run;
            mode[m - 1] += 1;
            cout << "\n---------------- Better Solution ----------------" << endl;
            cout << "---------- Found at Run" << setw(8) << run << " Mode: " << m << " ---------" << endl;
            cout << "Cost: " << minCost << " => " << cost << endl;
            minCost = cost;
            solSP = currSP;
            tempSP = solSP;
            last_update = chrono::steady_clock::now();
        } else { // if no improvement (delta >= 0)
            if (dis(gen) < exp(-delta / t_cool.count())) {
                tempSP = currSP;
                // cout << "\nAn Uphill move" << endl;
            }
            currSP = tempSP; // Recover to the last state
        }

        here = chrono::steady_clock::now();
        duration = chrono::duration_cast<chrono::duration<double>>(here - p1);
        t_cool = chrono::duration_cast<chrono::duration<double>>(here - last_update);
        run++;
        // cout << t_cool.count() << " " << duration.count() << " " << cost << endl;
    } while (t_cool.count() < 60 && duration.count() <= 120);

    // Random Rotation
    int rot_cnt = 0;
    currSP = solSP; // Recover to the optimal
    vector<Macro> Best_M = Macros; // Default
    p2 = chrono::steady_clock::now(); // Starting time
    cout << "\n================ Random Rotation ================" << endl;
    do {
        rand_rot(Macros);
        currSP.calcCost(Macros);
        cost = currSP.getCost();
        if (cost < minCost) {
            cout << "\n---------------- Better Solution ----------------" << endl;
            cout << "Cost: " << minCost << " => " << cost << endl;
            minCost = cost;
            Best_M = Macros;
            rot_cnt++;
        } else { // No improvement
            Macros = Best_M; // Recover to the optimal
        }
        here = chrono::steady_clock::now();
        duration = chrono::duration_cast<chrono::duration<double>>(here - p2);
    } while (duration.count() <= 30);
    Macros = Best_M; // Recover to the optimal

    // Output result (.txt file)
    solSP.calcCoords(Macros);
    parser.Output(outputFile, Macros, solSP.max_w, solSP.max_h);

    // Visualize the floorplanning result
    string gpFile = outputFile;
    gpFile = outputFile.substr(0, outputFile.length() - 4); // Delete .txt
    gpFile += ".gp";
    parser.genPlot(outputFile, gpFile);
    
    // cout << "\nRandom states generated: " << rand_cnt << endl;

    cout << "\nImprovements - Pertubation: " << endl;
    for (int i = 0; i < mode.size(); ++i) {
        cout << "Mode" << i + 1 << ": " << mode[i] << "\t";
    }
    cout << "\nMinimum Cost: " << minCost << ". Found at run " << run_best << endl;

    cout << "\nImprovements - Rotation: " << rot_cnt << endl;

    here = chrono::steady_clock::now();
    duration = chrono::duration_cast<chrono::duration<double>>(here - start);
    cout << "\nTotal time elapsed = " << duration.count() << " (" << run << " runs)" << endl;

    cout << "\nTerminating..." << endl;

    return 0;
}