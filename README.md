## Soft-Block-Floorplanning
--- 
### Purpose
This program aims to minimize the total area occupied by a set of soft blocks (macros).
Floorplan Representation: Sequence Pair method paired with the Longest Common Sequence Algorithm
Optimization Technique: Simulated Annealing
---
### Overview
#### Programming Language: C++
#### Platform: Linux GNU g++
#### Prerequisite: Linux environment and gnuplot
#### Files:
- file_parser.cpp (and the corresponding .h header file):
    Reader, writers and the macro data structure. 
- seq_pair.cpp (and the corresponding .h header file):
    The sequence pair data structure and some associated methods.
- floorplanner.cpp:
    The main program source code which includes SA optimization.
- Makefile:
    A makefile file.
- report.docx:
    A brief report on the data structures and algorithms.
- README.md:
    This file.
---
### How to compile
#### Simply type "make" under the directory.
    > make
---
### Usage
#### After a successful compilation, there will be an executable named *floorplanner*.
#### Type the following to execute:
    > ./floorplanner [input file name] [output file name]

#### Checker outputs the area if the file formats are correct.
    > ./checker [input file name] [output file name]

#### For example,
    > ./floorplanner floorplan_100.txt output_100.txt
    > ./checker floorplan_10.txt output_10.txt
---
### Remarks:
The program generally takes 1 to 2 minutes, and
the corresponding .gp file is generated as well.

###### 112 2nd Semester - Software Development for Electronic Design Automation
