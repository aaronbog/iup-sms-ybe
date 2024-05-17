#include "useful.h"
#include "domains.h"
#include <iostream>
#include <fstream>

int problem_size = 3;

int main(int argc, char const **argv)
{
    // argument parsing
    for (int i = 1; i < argc; i++)
    {
        if (strcmp("--size", argv[i]) == 0 || strcmp("-s", argv[i]) == 0)
        {
            i++;
            problem_size = atoi(argv[i]);
            continue;
        }


        /*printf("ERROR: invalid argument %s\n", argv[i]);
        EXIT_UNWANTED_STATE */
    }

    // ASSIGN DEFAULTS
    int t=0;
    for(int i=0; i<problem_size; i++)
        t+=i;
    t*=problem_size;

    vector<int> toPart;
    vector<vector<int>> parts;
    for(int i=0; i<problem_size; i++)
        toPart.push_back(i);
    part(problem_size, toPart, 0, parts);
    vector<vector<int>> diags;
    vector<int> d;
    for(int i=0; i<problem_size; i++)
        d.push_back(i);
    diags.push_back(d);
    d.clear();
    makeDiagonals(parts, diags);

    ofstream outFile;
    outFile.open ("diagonals.txt");
    for(auto d : diags){
        for(int i : d){
            outFile << i << ",";
        }
        outFile << "\n";
    }
    outFile.close();
    return 0;
}
