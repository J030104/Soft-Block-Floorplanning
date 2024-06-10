#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace std;
// #define PINF INT_MAX

class Macro {
public:
    string macroName;
    int w, h;
    int x1, y1, x2, y2;
    // int x2 = PINF, y2 = PINF;
    int cx, cy;

    void findCenter();
    void swapwh();
    void findx2y2();
};

extern unsigned NumBlocks;
extern double minAR; // MinAspectRatio
extern double maxAR; // MaxAspectRatio

class Parser {
    int calcTics(int n);
    int num;
public:
    Parser();
    static vector<Macro> parse(string inputFile);
    void genPlot(string inputFile, string outputFile);
    void Output(const string& outputFile, const vector<Macro>& M, int max_w, int max_h);
};

#endif