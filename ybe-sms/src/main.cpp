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
int checkFreq = 40;
clock_t startOfSolving;
bool allModels = false;
bool diagPart = false;
bool parallel = false;
bool propagateMincheck = false;
bool oldBreakingClauses = false;
bool propagateLiteralsCadical = false;
bool checkSolutionInProp = false;
int maxDepth = INT_MAX;
bool doFinalCheck=false;
bool smallerEncoding=false;
bool minCheckOld = false;
string solOutput = "";
vector<int> diagonal=vector<int>();

int main(int argc, char const **argv)
{

    srand((unsigned) time(NULL));
    
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

        if (strcmp("--checkFreq", argv[i]) == 0)
            {
                i++;
                checkFreq = atoi(argv[i]);
                continue;
            }

        if (strcmp("--smallerEncoding", argv[i]) == 0)
            {
                smallerEncoding = true;
                continue;
            }

        if (strcmp("--propagate", argv[i]) == 0)
            {
                propagateMincheck = true;
                continue;
            }

        if (strcmp("--oldBreak", argv[i]) == 0)
            {
                oldBreakingClauses = true;
                continue;
            }

        if (strcmp("--oldMinCheck", argv[i]) == 0)
            {
                minCheckOld = true;
                continue;
            }


        if(strcmp("--diag", argv[i])==0)
            {
                i++;
                int element;
                stringstream ss;
                ss<<argv[i];
                while (ss >> element)
                {
                    diagonal.push_back(element);

                    if (ss.peek() == ',')
                        ss.ignore();
                }
                if(diagonal.size()!=problem_size){
                    printf("ERROR: invalid argument, diagonal has length %lu different from problem size %d.", diagonal.size(),problem_size);
                    EXIT_UNWANTED_STATE;
                }
            }

        if (strcmp("--maxDepth", argv[i]) == 0)
            {
                i++;
                if(atoi(argv[i])>0){
                    maxDepth = atoi(argv[i]);
                    continue;
                } else {
                    printf("ERROR: invalid argument, maxDepth needs to be a positive number.");
                    EXIT_UNWANTED_STATE;
                }
            }

        if(strcmp("--out", argv[i])==0)
            {
                i++;
                stringstream ss;
                solOutput=argv[i];
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
        nextFreeVariable = 1;
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

        solver = new CadicalSolver(cnf, highestVariable, vector<int>(), cycset_lits, vector<vector<vector<int>>>(), stats);
        solver->solve();

        printf("Total time: %f\n", (duration_cast<nanoseconds>(steady_clock::now()-stats.start).count()) / 1000000000.0);

        return 0;
    } else if (diagonal.size()==0){
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

        vector<int> numSols=vector<int>(diags.size(),0);

        size_t i;
        
        #pragma omp parallel for shared(numSols,i,t,diags) schedule(dynamic, 1) if(diags.size()>=20) 
        for(i=0; i<diags.size(); i++)
        {
            auto d = diags[i];
            statistics stats;
            stats.start=steady_clock::now();
            
            cnf_t cnf;
            int nextFree = 1;

            vector<vector<vector<lit_t>>> cycset_lits = vector<vector<vector<lit_t>>>(problem_size, vector<vector<lit_t>>(problem_size, vector<lit_t>(problem_size, 0)));
            vector<vector<vector<lit_t>>> cycset_lits_ord = vector<vector<vector<lit_t>>>(problem_size, vector<vector<lit_t>>(problem_size, vector<lit_t>(problem_size, 0)));            
            vector<vector<lit_t>> ybe_left_lits = vector<vector<lit_t>>(t, vector<lit_t>(problem_size*problem_size, 0));
            vector<vector<lit_t>> ybe_right_lits = vector<vector<lit_t>>(t, vector<lit_t>(problem_size*problem_size, 0));
            vector<vector<lit_t>> ybe_lits = vector<vector<lit_t>>(t, vector<lit_t>(problem_size, 0));

            encodeEntries(&cnf, d, nextFree, cycset_lits);

            //encodeOrder(&cnf, d, nextFree, cycset_lits_ord, cycset_lits);
            
            YBEClauses(&cnf, d, nextFree, cycset_lits,ybe_left_lits,ybe_right_lits,ybe_lits);
            
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

            solver = new CadicalSolver(cnf, highestVariable,d, cycset_lits, cycset_lits_ord, stats);
            solver->solve();
            numSols[i]=solver->nModels;

            nextFreeVariable=max(nextFreeVariable,highestVariable);

            printf("Diagonal: ");
            for(auto i :d)
                printf("%d-",i);
            printf("\n");
            printf("Total time: %f\n", (duration_cast<nanoseconds>(steady_clock::now()-stats.start).count()) / 1000000000.0);
            printf("---------------------------------------------------------\n");
        }
        printf("Total time: %f\n", (duration_cast<nanoseconds>(steady_clock::now()-start).count()) / 1000000000.0);
        printf("Total models found: %d\n", accumulate(numSols.begin(),numSols.end(),0));
        return 0;
    }
    else {
        using namespace std::chrono;
        auto start = steady_clock::now();

        int totalModels=0;
        
        statistics stats;
        stats.start=steady_clock::now();
        
        cnf_t cnf;
        nextFreeVariable = 1;

        vector<vector<vector<lit_t>>> cycset_lits = vector<vector<vector<lit_t>>>(problem_size, vector<vector<lit_t>>(problem_size, vector<lit_t>(problem_size, 0)));
        vector<vector<vector<lit_t>>> cycset_lits_ord = vector<vector<vector<lit_t>>>(problem_size, vector<vector<lit_t>>(problem_size, vector<lit_t>(problem_size, 0)));   
        vector<vector<lit_t>> ybe_left_lits = vector<vector<lit_t>>(t, vector<lit_t>(problem_size*problem_size, 0));
        vector<vector<lit_t>> ybe_right_lits = vector<vector<lit_t>>(t, vector<lit_t>(problem_size*problem_size, 0));
        vector<vector<lit_t>> ybe_lits = vector<vector<lit_t>>(t, vector<lit_t>(problem_size, 0));

        encodeEntries(&cnf, diagonal, nextFreeVariable, cycset_lits);
        
        YBEClauses(&cnf, diagonal, nextFreeVariable, cycset_lits,ybe_left_lits,ybe_right_lits,ybe_lits);

        //encodeOrder(&cnf, diagonal, nextFreeVariable, cycset_lits_ord, cycset_lits);

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

        solver = new CadicalSolver(cnf, highestVariable, diagonal, cycset_lits, cycset_lits_ord, stats);
        solver->solve();
        totalModels=solver->nModels;

        printf("Diagonal: ");
        for(auto i :diagonal)
            printf("%d-",i);
        printf("\n");
        printf("Total time: %f\n", (duration_cast<nanoseconds>(steady_clock::now()-stats.start).count()) / 1000000000.0);
        printf("---------------------------------------------------------\n");

        printf("Total time: %f\n", (duration_cast<nanoseconds>(steady_clock::now()-start).count()) / 1000000000.0);
        printf("Total models found: %d\n", totalModels);
        return 0;
    }
}
