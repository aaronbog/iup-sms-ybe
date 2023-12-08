#include "useful.h"
#include "global.h"
#include "solveCadicalClass.hpp"
#include "cadical.hpp"

// add formula and register propagator
CadicalSolver::CadicalSolver(cnf_t &cnf, int highestVariable, vector<int> diag, vector<vector<vector<lit_t>>> lits, statistics stats)
{
    this->highestVariable = highestVariable;
    this->cycset_lits=lits;
    this->stats=stats;
    currentCycleSet = cycle_set_t();
    currentCycleSet.assignments=vector<vector<vector<truth_vals>>>(problem_size, vector<vector<truth_vals>>(problem_size, vector<truth_vals>(problem_size, Unknown_t)));
    currentCycleSet.matrix=vector<vector<int>>(problem_size, vector<int>(problem_size, -1));
    fixedCycleSet = vector<vector<vector<bool>>>(problem_size, vector<vector<bool>>(problem_size, vector<bool>(problem_size, false)));
    // The root-level of the trail is always there
    current_trail.push_back(std::vector<int>());

    string outputFilePath;
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
    
    // solver->configure("plain");
    if (!solver->configure("sat"))
        EXIT_UNWANTED_STATE

    // solver->set("lucky", 0);
    // solver->set("log", 1);
    // solver->set("debug", 1);

    // register propagator first
    solver->connect_external_propagator(this);

    lit2entry.push_back(make_tuple(-1, -1, -1)); // dummy pair for index 0
     
    highestEdgeVariable = (problem_size*problem_size*problem_size);
    for (int i = 0; i < problem_size; i++)
        for (int j = 0; j < problem_size; j++)
            for (int k = 0; k < problem_size; k++)
                if(!diagPart || i!=j)
                    lit2entry.push_back(make_tuple(i, j,k));

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

    for (int i = 0; i < problem_size; i++)
        for (int j = 0; j < problem_size; j++)
            for (int k = 0; k < problem_size; k++)
                if(!diagPart || i!=j)
                    solver->add_observed_var(cycset_lits[i][j][k]);

    literal2clausePos = vector<vector<int>>(highestEdgeVariable + 1);
    literal2clauseNeg = vector<vector<int>>(highestEdgeVariable + 1);

    if(diagPart)
        fixDiag(diag);
}
void CadicalSolver::fixDiag(vector<int> diag)
{
    for(int i=0; i<problem_size; i++){
        for(int k=0; k<problem_size; k++){
            if(k!=diag[i])
                currentCycleSet.assignments[i][i][k]=False_t;
            else
                currentCycleSet.assignments[i][i][k]=True_t;
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
    } while (solver->solve() == 10 && !check_solution());
    fclose(output);
}

bool CadicalSolver::solve(vector<int>, int)
{
    printf("Not implemented yet\n");
    EXIT_UNWANTED_STATE
}
