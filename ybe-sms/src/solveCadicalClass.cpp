#include "useful.h"
#include "global.h"
#include "solveCadicalClass.hpp"
#include "cadical.hpp"
#include "minCheck_V2.h"
#include "minCheck_V3.h"

// add formula and register propagator
CadicalSolver::CadicalSolver(cnf_t &cnf, int highestVariable, vector<int> diag, vector<vector<vector<lit_t>>> lits, vector<vector<vector<lit_t>>> ord_lits, statistics stats)
{
    this->highestVariable = highestVariable;
    this->cycset_lits=lits;
    this->stats=stats;
    this->diag=diag;
    currentCycleSet = cycle_set_t(problem_size,lits);
    fixedCycleSet = vector<vector<vector<bool>>>(problem_size, vector<vector<bool>>(problem_size, vector<bool>(problem_size, false)));
    // The root-level of the trail is always there
    current_trail.push_back(std::vector<int>());

    string outputFilePath;
    outputFilePath.append(solOutput);
    outputFilePath.append("sols_");
    outputFilePath.append(to_string(problem_size));
    outputFilePath.append("_");
    for(auto d : diag)
        outputFilePath.append(to_string(d));
    outputFilePath.append(".txt");

    FILE *fp;
    fp = fopen(outputFilePath.c_str(),"w");
    this->output=fp;

    // only_propagating = false;
    solver = new CaDiCaL::Solver();
    
    //solver->configure("plain");
    if (!solver->configure("unsat"))
        EXIT_UNWANTED_STATE

    //solver->set("shuffle", 0);
    //solver->set("shufflequeue", 0);

    solver->set("lucky", 0);
    solver->set("walk", 0);
    solver->set("elim", 0);
    // solver->set("log", 1);
    // solver->set("debug", 1);

    // register propagator first
    solver->connect_external_propagator(this);

    lit2entry.push_back(vector<int>{-1,-1,-1}); // dummy pair for index 0
     
    highestEdgeVariable = 0;
    for (int i = 0; i < problem_size; i++)
        for (int j = 0; j < problem_size; j++)
            for (int k = 0; k < problem_size; k++)
                if(!diagPart || (i!=j && (!smallerEncoding||k!=diag[i])))
                {
                    lit2entry.push_back(vector<int>{i,j,k});
                    highestEdgeVariable++;
                }

    // add clauses to solver
    for (auto clause : cnf)
    {
        if (clause.size() == 0)
            EXIT_UNWANTED_STATE

        for (auto lit : clause)
        {
            if (lit == 0)
                EXIT_UNWANTED_STATE
            solver->add(lit);
        }
        solver->add(0);
    }

    

    //solver->write_dimacs("ybe.cnf");

    for (int i = 0; i < problem_size; i++)
        for (int j = 0; j < problem_size; j++)
            for (int k = 0; k < problem_size; k++)
                if(!diagPart || (i!=j&&(!smallerEncoding||k!=diag[i])))
                    solver->add_observed_var(cycset_lits[i][j][k]);

    literal2clausePos = vector<vector<int>>(highestEdgeVariable + 1);
    literal2clauseNeg = vector<vector<int>>(highestEdgeVariable + 1);

    if(diagPart)
        fixDiag(diag);

    /* printCycleSet(currentCycleSet);
    printDomains(currentCycleSet); */

    if(minCheckOld)
        mincheck = new MinCheck_V3(diag,cycset_lits);
    else
        mincheck = new MinCheck_V2(diag,cycset_lits);

    //mincheck = MinimalityChecker(diag,cycset_lits);
}
void CadicalSolver::fixDiag(vector<int> diag)
{
    for(int i=0; i<problem_size; i++){
        for(int k=0; k<problem_size; k++){
            if(k!=diag[i]){
                currentCycleSet.assignments[i][i][k]=False_t;
            } else {
                currentCycleSet.assignments[i][i][k]=True_t;
            }
            if(k!=i){
                currentCycleSet.bitdomains[i][k].reset(diag[i]);
            } else {
                currentCycleSet.bitdomains[i][k].reset();
                currentCycleSet.bitdomains[i][k].set(diag[i]);
            }
            fixedCycleSet[i][i][k]=true;
        }       
    currentCycleSet.matrix[i][i]=diag[i];
    }
}


void CadicalSolver::solve(vector<int> assumptions)
{
    do
    {
        for (auto lit : assumptions)
            solver->assume(lit);
        //solver->statistics();
        //solver->resources();
    } while (solver->solve() == 10);
    fclose(output);
}

bool CadicalSolver::solve(vector<int>, int)
{
    printf("Not implemented yet\n");
    EXIT_UNWANTED_STATE
}
