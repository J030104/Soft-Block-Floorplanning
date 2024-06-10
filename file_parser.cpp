#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include "file_parser.h"

using namespace std;

unsigned NumBlocks;
double minAR; // MinAspectRatio
double maxAR; // MaxAspectRatio

void Macro::findCenter() {
    cx = (x1 + x2) / 2;
    cy = (y1 + y2) / 2;
}

void Macro::swapwh() {
    if(w != h)
        w ^= h ^= w ^= h; // Bitwise swap
}

void Macro::findx2y2() {
    x2 = x1 + w;
    y2 = y1 + h;
}

/* Input File Format
* NumBlocks <# of blocks>
* MinAspectRatio <minimum 𝑏𝑙𝑜𝑐𝑘 ℎ𝑒𝑖𝑔ℎ𝑡 / 𝑏𝑙𝑜𝑐𝑘 𝑤𝑖𝑑𝑡ℎ>
* MaxAspectRatio <maximum 𝑏𝑙𝑜𝑐𝑘 ℎ𝑒𝑖𝑔ℎ𝑡 / 𝑏𝑙𝑜𝑐𝑘 𝑤𝑖𝑑𝑡ℎ>
* <macro name> <macro width> <macro height>
* … More macros
* 
* Example:
*
* NumBlocks 4
* MinAspectRatio 0.5
* MaxAspectRatio 2
* A 30 40
* B 40 60
* C 50 30
* D 40 50
*/

 Parser::Parser() {
    num = 0;
 }

vector<Macro> Parser::parse(string inputFile) {
    fstream fin; // floorplan_#.txt
    fin.open(inputFile, fstream::in);
    if (!fin.is_open()) {
        cout << "Input file not opened." << endl;
        exit(1);
    }
    
    // Head Part
    string tmp;
    fin >> tmp;
    if (tmp == "NumBlocks")
        fin >> NumBlocks;
    else exit(2);

    fin >> tmp;
    if (tmp == "MinAspectRatio")
        fin >> minAR;
    else exit(3);
    
    fin >> tmp;
    if (tmp == "MaxAspectRatio")
        fin >> maxAR;
    else exit(4);

    // Macro Part
    vector<Macro> m_in(NumBlocks);
    string line;
    unsigned n = 0;
    while (getline(fin, line) && n < NumBlocks) {
        if (!line.empty()) { // Skip empty lines
            istringstream iss(line);
            Macro temp;
            if (iss >> temp.macroName >> temp.w >> temp.h) {
                m_in[n] = temp; // Success
                n++;
            } else {
                cerr << "Error parsing line: " << line << endl;
                exit(5);
            }
        }
    }

    fin.close();
    
    // Print parsing result
    cout << "Number of Blocks: " << NumBlocks << endl;
    cout << "Minimum Aspect Ratio: " << minAR << endl;
    cout << "Maximum Aspect Ratio: " << maxAR << endl;
    // for (const auto& data : m_in) {
    //     cout << data.macroName << " " << data.w
    //               << " " << data.h << endl;
    // }

    return m_in;
}

int Parser::calcTics(int n) {
    n /= 8;
    if (n > 10) {
        n /= 10;
        n *= 10;
    }
    return n;
}

void Parser::genPlot(string inputFile, string outputFile) {
    string pngFile = outputFile;
    pngFile = pngFile.substr(0, pngFile.length() - 3);
    pngFile += ".png";

    fstream fin;
    fin.open(inputFile, fstream::in);
    if (!fin.is_open()) {
        cout << "Input file not opened." << endl;
        exit(-1);
    }
    
    int width, height, area;
    fin >> width >> height;
    area = width * height;
    
    int xtics, ytics;
    xtics = calcTics(width);
    ytics = calcTics(height);
    
    fstream fout;
    fout.open(outputFile, fstream::out);
    if(!fout.is_open()) {
        cout << "Output file not opened." << endl;
        exit(-2);
    }
    fout << "reset\n";
    fout << "set title \"Floorplan - Area(Cost): " << area << "\"\n";
    fout << "set xlabel \"X\"\n";
    fout << "set ylabel \"Y\"\n";
    
    string line;
    unsigned i = 1;
    while (getline(fin, line)) {
        if (!line.empty()) { // Skip empty lines
            istringstream iss(line);
            Macro m;
            int w_ign, h_ign;
            if (iss >> m.macroName >> w_ign >> h_ign >> m.x1 >> m.y1 >> m.x2 >> m.y2) {
                // Success
                m.findCenter();
                fout << "set object " << i << " rect from " << m.x1 << "," << m.y1
                    << " to " << m.x2 << "," << m.y2 << "\n";
                fout << "set label \"" << m.macroName << "\" at " << m.cx << "," << m.cy << " center\n";
                i++;
            } else {
                cerr << "Error parsing line: " << line << endl;
                exit(-3);
            }
        }
    }

    fin.close();

    fout << "set xtics " << xtics << "\n";
    fout << "set ytics " << ytics << "\n";
    fout << "plot [0:" << width << "][0:" << height << "]0\n";
    fout << "set terminal png size 1080, 1080\n"; // 1024,768
    fout << "set output \"" << pngFile << "\"\n";
    fout << "replot\n";
    
    fout.close();

    // Linux command
    // string command = "gnuplot < ";
    // command += outputFile;
    // int result = system(command.c_str());

    // if (result > 0) {
    //     cerr << "Gnuplot script execution may have errors (exit code: " << result << ")" << endl;
    // } else if (result == -1) {
    //     cerr << "Error executing gnuplot command" << endl;
    // }
}

/* Output File Format:
* <chip_width> <chip_height>
* <macro_name> <macro width> <macro height> <x1> <y1> <x2> <y2>
* 
* (x1, y1): lower-left corner, (x2, y2): upper-right corner
* 
* Example:
* 
* 80 90
* A 40 30 0 60 40 90
* B 40 60 0 0 40 60
* C 50 30 40 0 80 30
* D 50 40 40 30 80 70
*/

void Parser::Output(const string& outputFile, const vector<Macro>& M, int max_w, int max_h) {
    cout << "\nStart processing placing info..." << endl;
    
    // Output the file
    ofstream fout(outputFile);
    if(!fout.is_open()) {
        cout << "Output file not opened." << endl;
        exit(-8787);
    }
    
    fout << max_w << " " << max_h << endl;
    for (int i = 0; i < M.size(); ++i) {
        Macro currM = M[i];
        fout << currM.macroName << " " 
             << currM.w << " " << currM.h << " "
             << currM.x1 << " " << currM.y1 << " "
             << currM.x2 << " " << currM.y2 << endl;
    }

    fout.close();
}