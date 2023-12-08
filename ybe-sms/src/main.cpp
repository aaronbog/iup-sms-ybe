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
bool parallel = false;
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
    int t=0;
    for(int i=0; i<problem_size; i++)
        t+=i;
    t*=problem_size;

    if(!diagPart){

        statistics stats;
        stats.start = steady_clock::now();

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

        solver = new CadicalSolver(cnf, highestVariable, vector<int>(), cycset_lits, stats);
        solver->solve();

        printf("Total time: %f\n", (duration_cast<nanoseconds>(steady_clock::now()-stats.start).count()) / 1000000000.0);

        return 0;
    } else {
        using namespace std::chrono;
        auto start = steady_clock::now();

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

        vector<int> numSols=vector<int>(diags.size(),0);

        int i;
        
        #pragma omp parallel for shared(numSols,i,t,diags) schedule(dynamic, 3) if(diags.size()>=20) 
        for(i=0; i<diags.size(); i++)
        {
            auto d = diags[i];
            statistics stats;
            stats.start=steady_clock::now();
            
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

            int highestVariable = 0;
            for (auto clause : cnf)
            {
                for (auto lit : clause)
                    highestVariable = max(highestVariable, abs(lit));
            }

            solver = new CadicalSolver(cnf, highestVariable,d, cycset_lits, stats);
            solver->solve();
            numSols[i]=solver->nModels;

            printf("Diagonal: ");
            for(auto i :d)
                printf("%d-",i);
            printf("\n");
            printf("Num solutions: %d\n",solver->nModels);
            printf("Total time: %f\n", (duration_cast<nanoseconds>(steady_clock::now()-stats.start).count()) / 1000000000.0);
            printf("---------------------------------------------------------\n");
        }
        printf("Total time: %f\n", (duration_cast<nanoseconds>(steady_clock::now()-start).count()) / 1000000000.0);
        printf("Total models found: %d\n", accumulate(numSols.begin(),numSols.end(),0));
        return 0;
    }
}
