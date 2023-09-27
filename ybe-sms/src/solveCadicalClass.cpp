#include "useful.h"
#include "global.h"
#include "solveCadicalClass.hpp"
#include "cadical.hpp"

// add formula and register propagator
CadicalSolver::CadicalSolver(cnf_t &cnf, int highestVariable)
{
    this->highestVariable = highestVariable;
    currentCycleSet = cycle_set_t();
    currentCycleSet.assignments=vector<vector<vector<truth_vals>>>(size, vector<vector<truth_vals>>(size, vector<truth_vals>(size, Unknown_t)));
    currentCycleSet.matrix=vector<vector<int>>(size, vector<int>(size, -1));
    fixedCycleSet = vector<vector<vector<bool>>>(size, vector<vector<bool>>(size, vector<bool>(size, false)));
    // The root-level of the trail is always there
    current_trail.push_back(std::vector<int>());

    // only_propagating = false;
    solver = new CaDiCaL::Solver();
    
    
    // solver->configure("plain");
    if (!solver->configure("unsat"))
        EXIT_UNWANTED_STATE

    // solver->set("lucky", 0);
    // solver->set("log", 1);
    // solver->set("debug", 1);

    // register propagator first
    solver->connect_external_propagator(this);

    lit2entry.push_back(make_tuple(-1, -1, -1)); // dummy pair for index 0
     
    highestEdgeVariable = (size*size*size);
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            for (int k = 0; k < size; k++)
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

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            for (int k = 0; k < size; k++)
                solver->add_observed_var(cycset_lits[i][j][k]);

    literal2clausePos = vector<vector<int>>(highestEdgeVariable + 1);
    literal2clauseNeg = vector<vector<int>>(highestEdgeVariable + 1);
}

void CadicalSolver::solve(vector<int> assumptions)
{
    do
    {
        for (auto lit : assumptions)
            solver->assume(lit);
    } while (solver->solve() == 10 && !check_solution());
}

bool CadicalSolver::solve(vector<int>, int)
{
    printf("Not implemented yet\n");
    EXIT_UNWANTED_STATE
}