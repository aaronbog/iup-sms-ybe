#include "useful.h"
#include "global.h"
#include "clause.h"
#include "solveGeneral.hpp"
#include "solveCadicalClass.hpp"
#include <omp.h>
#include <fstream>
#include <sstream>
#include <iterator>


int nextFreeVariable;
int problem_size = 3;
int checkFreq = 20;
clock_t startOfSolving;
bool allModels = false;
bool diagPart = false;
statistics stats;
bool propagateLiteralsCadical = false;
bool checkSolutionInProp = false;
bool v2 = false;

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

        if (strcmp("--diagPart", argv[i]) == 0)
        {
            diagPart = true;
            continue;
        }
        

        if (strcmp("--size", argv[i]) == 0 || strcmp("-s", argv[i]) == 0)
        {
            i++;
            problem_size = atoi(argv[i]);
            continue;
        }

        if (strcmp("--propLits", argv[i]) == 0)
        {
            propagateLiteralsCadical = true;
            continue;
        }

    if (strcmp("--checkSols", argv[i]) == 0)
        {
            checkSolutionInProp = true;
            continue;
        }

    if (strcmp("--v2", argv[i]) == 0)
        {
            v2 = true;
            continue;
        }

    if (strcmp("--checkFreq", argv[i]) == 0)
        {
            i++;
            checkFreq = atoi(argv[i]);
            continue;
        }


        /*printf("ERROR: invalid argument %s\n", argv[i]);
        EXIT_UNWANTED_STATE */
    }

    // ASSIGN DEFAULTS
    //stats.start = clock();

    int t=0;
    for(int i=0; i<problem_size; i++)
        t+=i;
    t*=problem_size;

    if(!diagPart){
        cnf_t cnf;
        int nextFreeVariable = 1;
        vector<vector<vector<lit_t>>> cycset_lits = vector<vector<vector<lit_t>>>(problem_size, vector<vector<lit_t>>(problem_size, vector<lit_t>(problem_size, 0)));
        vector<vector<lit_t>> ybe_left_lits = vector<vector<lit_t>>(t, vector<lit_t>(problem_size*problem_size, 0));
        vector<vector<lit_t>> ybe_right_lits = vector<vector<lit_t>>(t, vector<lit_t>(problem_size*problem_size, 0));
        vector<vector<lit_t>> ybe_lits = vector<vector<lit_t>>(t, vector<lit_t>(problem_size, 0));

        // create new variables
            
        for (int i = 0; i < problem_size; i++)
            for (int j = 0; j < problem_size; j++)
                for (int k = 0; k < problem_size; k++)
                    cycset_lits[i][j][k] = nextFreeVariable++;
        
        for (int i = 0; i < t; i++)
            for (int j = 0; j < problem_size; j++)
                ybe_lits[i][j]=nextFreeVariable++;
        
        for (int i = 0; i < t; i++)
            for (int j = 0; j < problem_size*problem_size; j++)
            {
                ybe_left_lits[i][j]=nextFreeVariable++;
                ybe_right_lits[i][j]=nextFreeVariable++;
            }
        
        encodeEntries(&cnf,nextFreeVariable, cycset_lits);
        YBEClauses(&cnf,nextFreeVariable,cycset_lits,ybe_left_lits,ybe_right_lits,ybe_lits);        

        // check if zero literal
        for (auto clause : cnf)
        {
            for (auto lit : clause)
                if (lit == 0)
                    EXIT_UNWANTED_STATE
        }

        CommonInterface *solver;

        printf("SAT Solver: Cadical\n");

        int highestVariable = 0;
        for (auto clause : cnf)
        {
            for (auto lit : clause)
                highestVariable = max(highestVariable, abs(lit));
        }

        solver = new CadicalSolver(cnf, highestVariable, vector<int>(), cycset_lits);
        solver->solve();

        printf("Total time: %f\n", ((double)clock() - stats.start) / CLOCKS_PER_SEC);

        return 0;
    } else {
        
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

        int totalModels=0;
        
        #pragma omp parallel for
        for(auto d : diags)
        {
            cnf_t cnf;
            int nextFreeVariable = 1;

            vector<vector<vector<lit_t>>> cycset_lits = vector<vector<vector<lit_t>>>(problem_size, vector<vector<lit_t>>(problem_size, vector<lit_t>(problem_size, 0)));
            
            vector<vector<lit_t>> ybe_left_lits = vector<vector<lit_t>>(t, vector<lit_t>(problem_size*problem_size, 0));
            vector<vector<lit_t>> ybe_right_lits = vector<vector<lit_t>>(t, vector<lit_t>(problem_size*problem_size, 0));
            vector<vector<lit_t>> ybe_lits = vector<vector<lit_t>>(t, vector<lit_t>(problem_size, 0));

            encodeEntries(&cnf, d, nextFreeVariable, cycset_lits);
            
            YBEClauses(&cnf, d, nextFreeVariable, cycset_lits,ybe_left_lits,ybe_right_lits,ybe_lits);

           
            
            // check if zero literal
            for (auto clause : cnf)
            {
                for (auto lit : clause)
                    if (lit == 0)
                        EXIT_UNWANTED_STATE
            }

            CommonInterface *solver;
            
            printf("SAT Solver: Cadical\n");
            int highestVariable = 0;
            for (auto clause : cnf)
            {
                for (auto lit : clause)
                    highestVariable = max(highestVariable, abs(lit));
            }

            solver = new CadicalSolver(cnf, highestVariable,d, cycset_lits);
            for(auto i : solver->cycset_lits){
                printf("%d, ",i);
            }
            printf("\n");
            solver->solve();
            printf("SOLVING");

            printf("Total time: %f\n", ((double)clock() - stats.start) / CLOCKS_PER_SEC);
            //totalModels+=solver->nModels;
        }
        //printf("Total models found: %d\n", totalModels);
        return 0;
    }
}
