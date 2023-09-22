#include "useful.h"
#include "global.h"
#include "clause.h"
#include "solveGeneral.hpp"
#include "solveCadicalClass.hpp"
#include <fstream>
#include <sstream>
#include <iterator>

int nextFreeVariable;
int size;
vector<vector<vector<lit_t>>> cycset_lits;
vector<vector<lit_t>> ybe_left_lits;
vector<vector<lit_t>> ybe_right_lits;
vector<vector<lit_t>> ybe_lits;
clock_t startOfSolving;
bool allModels;
statistics stats;

int main(int argc, char const **argv)
{
    // argument parsing
    for (int i = 1; i < argc; i++)
    {
        if (strcmp("--allModels", argv[i]) == 0)
        {
            allModels = true;
            continue;
        }
        

        if (strcmp("--size", argv[i]) == 0 || strcmp("-s", argv[i]) == 0)
        {
            i++;
            size = atoi(argv[i]);
            continue;
        }

        /*printf("ERROR: invalid argument %s\n", argv[i]);
        EXIT_UNWANTED_STATE */
    }

    // ASSIGN DEFAULTS
    //stats.start = clock();

    cnf_t cnf;
    nextFreeVariable = 1;

    // create new variables
    cycset_lits = vector<vector<vector<lit_t>>>(size, vector<vector<lit_t>>(size, vector<lit_t>(size, 0)));    
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            for (int k = 0; k < size; k++)
                cycset_lits[i][j][k] = nextFreeVariable++;

    int t=0;
    for(int i=0; i<size; i++)
        t+=i;
    t*=size;
    
    ybe_left_lits = vector<vector<lit_t>>(t, vector<lit_t>(size*size, 0));
    ybe_right_lits = vector<vector<lit_t>>(t, vector<lit_t>(size*size, 0));
    ybe_lits = vector<vector<lit_t>>(t, vector<lit_t>(size, 0));
    
    for (int i = 0; i < t; i++)
        for (int j = 0; j < size; j++)
            ybe_lits[i][j]=nextFreeVariable++;
    for (int i = 0; i < t; i++)
        for (int j = 0; j < size*size; j++)
        {
            ybe_left_lits[i][j]=nextFreeVariable++;
            ybe_right_lits[i][j]=nextFreeVariable++;
        }
    
    encodeEntries(&cnf);
    YBEClauses(&cnf);          

    // check if zero literal
    for (auto clause : cnf)
    {
        for (auto lit : clause)
            if (lit == 0)
                EXIT_UNWANTED_STATE
    }

    // solving part-------------------------

    printf("Clauses: %ld, Variables %d\n", cnf.size(), nextFreeVariable - 1);
    fflush(stdout);
    //printCnf(&cnf);

    CommonInterface *solver;

    printf("SAT Solver: Cadical\n");
    int highestVariable = 0;
    for (auto clause : cnf)
    {
        for (auto lit : clause)
            highestVariable = max(highestVariable, abs(lit));
    }

    vector<int> toPart;
    vector<vector<int>> parts;
    for(int i=0; i<size; i++)
        toPart.push_back(i);
    part(size, toPart, 0, parts);
    vector<vector<int>> diags;
    vector<int> d;
    for(int i=0; i<size; i++)
        d.push_back(i);
    diags.push_back(d);
    d.clear();
    makeDiagonals(parts, diags);
    
    // SOLVE PER DIAGONAL

    solver = new CadicalSolver(cnf, highestVariable);

    solver->solve();

    printf("Total time: %f\n", ((double)clock() - stats.start) / CLOCKS_PER_SEC);
    return 0;
}
