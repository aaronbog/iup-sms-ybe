#ifndef CADICAL_SOLVER_INTERFACE_H
#define CADICAL_SOLVER_INTERFACE_H

#include "global.h"
#include "solveGeneral.hpp"
#include "cadical.hpp"
#include <bits/stdc++.h>
#include <algorithm>

class CadicalSolver : public CommonInterface, public CaDiCaL::ExternalPropagator
{
private:
    CaDiCaL::Solver *solver;
    const std::vector<int> *model;
    bool redundant;
    bool changeInTrail = true; // checks whether the trail has changed since the last propagation step

    int highestEdgeVariable;
    vector<vector<int>> lit2entry; // get for positive literal the corresponding edge
    vector<vector<int>> clauses;
    int highestVariable;
    int checkMode = false; // if true solver has finished and clauses are added by the normal "incremental interface"
    cycle_set_t currentCycleSet = cycle_set_t(problem_size,cycset_lits);
    vector<vector<vector<bool>>> fixedCycleSet; // true if entry in currentMatrix is fixed.
    deque<vector<int>> current_trail; // for each decision lvl store the assigned literals (only positive version)

    vector<vector<int>> literal2clausePos; // for each edge variable store clause which was used the last time.
    vector<vector<int>> literal2clauseNeg; // for each negation of an edge variable
    void fixDiag(vector<int> diag);
public:
    CadicalSolver(cnf_t &cnf, int highestVariable);
    CadicalSolver(cnf_t &cnf, int highestVariable, vector<int> diag, vector<vector<vector<lit_t>>> cycset_lits, vector<vector<vector<lit_t>>> ord_lits, statistics stats);
    ~CadicalSolver() { solver->disconnect_external_propagator(); }

protected: // virtual classes from common interface
    void solve(vector<int> assumptions);
    bool solve(vector<int> assumptions, int timeout);

    cycle_set_t getCycleSet()
    {
        if (!checkMode)
        {
            return currentCycleSet;
        }
        else
        {
            cycle_set_t cycset(problem_size,currentCycleSet.cycset_lits);
            //cycset.assignments=vector<vector<vector<truth_vals>>>(problem_size, vector<vector<truth_vals>>(problem_size, vector<truth_vals>(problem_size, Unknown_t)));
            //cycset.matrix=vector<vector<int>>(problem_size, vector<int>(problem_size, -1));
            for (int i = 0; i < problem_size; i++)
                for (int j = 0; j < problem_size; j++)
                    for (int k = 0; k < problem_size; k++)
                    {
                        if(cycset_lits[i][j][k]!=0){
                            if (solver->val(cycset_lits[i][j][k])>0)
                            {
                                cycset.matrix[i][j]=k;
                                cycset.assignments[i][j][k]=True_t;
                                cycset.domains[i][j].dom=vector<int>{k};
                                for(int l=0; l<problem_size; l++){
                                    if(j==l)
                                        continue;
                                    else
                                        cycset.domains[i][l].delete_value(k);
                                }
                            } 
                            if (solver->val(cycset_lits[i][j][k])<0)
                            {
                                cycset.assignments[i][j][k]=False_t;
                                cycset.domains[i][j].delete_value(k);
                            }
                        } else if (diagPart && i==j){
                            cycset.matrix[i][j]=currentCycleSet.matrix[i][j];
                            cycset.assignments[i][j][k]=True_t;
                            cycset.domains[i][j].dom=vector<int>{k};
                            for(int l=0; l<problem_size; l++){
                                if(j==l)
                                    continue;
                                else
                                    cycset.domains[i][l].delete_value(k);
                            }
                        }
                    } 
            //cycset.cycset_lits=currentCycleSet.cycset_lits;
            //cycset.ordered_lits=currentCycleSet.ordered_lits;
            return cycset;
        }
    }

    bool is_decision(lit_t lit){
        return solver->is_decision(lit);
    }

    bool is_true(lit_t lit){
        return solver->val(lit);
    }

    void addClause(const vector<lit_t> &clause, bool redundant)
    {
        //printf("size clause: %d\n",clause.size());
        if (!checkMode)
            this->clauses.push_back(clause);
        else
        {
            // use incremental interface
            for (auto l : clause)
                solver->add(l);
            solver->add(0);
        }
        //solver->resources();
    }

public:
    void notify_assignment(int lit, bool is_fixed)
    {
        changeInTrail = true;
        int absLit = abs(lit);
        if (!is_fixed) // push back literal to undo if current decission literal is changed
            {current_trail.back().push_back(absLit);}
        auto entry = lit2entry[absLit];
        //printf("Assigned %d %d = %d to %d\n",entry[0],entry[1],entry[2],lit>0?1:0);
        if (lit > 0)
        {
            currentCycleSet.assignments[entry[0]][entry[1]][entry[2]] = True_t;
            currentCycleSet.matrix[entry[0]][entry[1]] = entry[2];
            currentCycleSet.domains[entry[0]][entry[1]].dom=vector<int>{entry[2]};
            for(int l=0; l<problem_size; l++){
                if(entry[1]==l)
                    continue;
                else
                    currentCycleSet.domains[entry[0]][l].delete_value(entry[2]);
            }
            
        }
        else
        {
            currentCycleSet.assignments[entry[0]][entry[1]][entry[2]] = False_t;
            currentCycleSet.domains[entry[0]][entry[1]].delete_value(entry[2]);
        }
        if (is_fixed)
            fixedCycleSet[entry[0]][entry[1]][entry[2]] = true;
    }

    void notify_new_decision_level()
    {
        current_trail.push_back(vector<int>());
    }

    void notify_backtrack(size_t new_level)
    {
        /* printf("BACKTRACK\n");
        printPartiallyDefinedCycleSet(currentCycleSet);
        printDomains(currentCycleSet);
        printf("----------\n"); */
        while (current_trail.size() > new_level + 1)
        {
            auto last = current_trail.back();
            for (int l : last)
            {
                auto entry = lit2entry[l];
                //printf("Entry = %d %d = %d\n",entry[0],entry[1],entry[2]);
                if (fixedCycleSet[entry[0]][entry[1]][entry[2]])
                    continue;

                if(currentCycleSet.assignments[entry[0]][entry[1]][entry[2]]==True_t){
                    //printf("Reset %d %d = %d was 1\n",entry[0],entry[1],entry[2]);
                    currentCycleSet.matrix[entry[0]][entry[1]]=-1;
                    currentCycleSet.domains[entry[0]][entry[1]]=domain_t(problem_size);
                    
                    for(int j=0;j<problem_size;j++){
                        if(j!=entry[1]){
                            if(currentCycleSet.matrix[entry[0]][j]!=-1)
                                currentCycleSet.domains[entry[0]][entry[1]].delete_value(currentCycleSet.matrix[entry[0]][j]);
                            else if (currentCycleSet.assignments[entry[0]][j][entry[2]]!=False_t)
                                currentCycleSet.domains[entry[0]][j].add_value(entry[2]);
                        }

                        if(currentCycleSet.assignments[entry[0]][entry[1]][j]==False_t){
                            currentCycleSet.domains[entry[0]][entry[1]].delete_value(j);
                        }
                    }
                    /* printDomains(currentCycleSet);
                    printf("----\n"); */
                }

                if(currentCycleSet.assignments[entry[0]][entry[1]][entry[2]]==False_t){
                    //printf("Reset %d %d = %d was 0\n",entry[0],entry[1],entry[2]);
                    currentCycleSet.domains[entry[0]][entry[1]].add_value(entry[2]);
                    //printDomains(currentCycleSet);
                    //printf("----\n");
                }

                currentCycleSet.assignments[entry[0]][entry[1]][entry[2]] = Unknown_t;
            }
            current_trail.pop_back();
        }
        //printDomains(currentCycleSet);
        /* for(int i=0;i<problem_size;i++){
            for(int j=0; j<problem_size;j++){
                if(i==j)
                    continue;
                for(int k=0;k<problem_size;k++){
                    if(k!=currentCycleSet.matrix[i][i])
                        printf("%d, ", i,j,k,currentCycleSet.assignments[i][j][k]);
                }
            }
        } */
        //printf("------------------------------------------\n");
    }

    // currently not checked in propagator but with the normal incremental interface to allow adding other literals or even new once.
    bool cb_check_found_model(const std::vector<int> &model)
    {
        //printf("cb_check_found_model\n");
        if (!clauses.empty())
            return false; // EXIT_UNWANTED_STATE only do check if there isn't another clause to add before
        // this->current_trail = &model;
        if (checkSolutionInProp)
        {
            return check();
        }
        return true;
    }

    bool cb_has_external_clause()
    {
        // PRINT_CURRENT_LINE
        // if no clause, then check whether a clause could be added. If already a clause present then just return clause.
        // if propagation is done in other function then not compute clauses here
        if (clauses.empty() && changeInTrail && !propagateLiteralsCadical)
        {
            changeInTrail = false;
            propagate();
        }

        // printf("Check for external clause: %ld\n", sym_breaking_clause.size());
        return !clauses.empty();
    }

    int cb_add_external_clause_lit()
    {
        // PRINT_CURRENT_LINE
        // printf("Call: Add external clause\n");
        vector<int> &lastClause = clauses[clauses.size() - 1];
        if (lastClause.empty())
        {
            clauses.pop_back(); // delete last clause
            // printf(" end clause\n");
            return 0;
        }
        else
        {
            // printf("Add external clause\n");
            int lit = lastClause.back();
            lastClause.pop_back();
            // printf("%d ", lit);
            return lit;
        }
    }

    // functions need to be defined
    int cb_decide() {
        //cycle_set_t cyc = getCycleSet();
        for(int i=0; i<problem_size; i++){
            for(int j=0; j<problem_size; j++){
                if(i!=j && currentCycleSet.matrix[i][j]==-1){
                    int max = *min_element(currentCycleSet.domains[i][j].dom.begin(),currentCycleSet.domains[i][j].dom.end());
                    return cycset_lits[i][j][max];
                }
            }
        }
        return 0;
        /* vector<int> dom_sizes;
        for(int i=0; i<problem_size; i++){
            for(int j=0; j<problem_size; j++){
                dom_sizes.push_back(currentCycleSet.domains[i][j].dom.size());
            }
        }
        int max_index = distance(dom_sizes.begin(),max_element(dom_sizes.begin(), dom_sizes.end()));
        int row = max_index/problem_size;
        int col = max_index%problem_size;
        int max = *max_element(currentCycleSet.domains[row][col].dom.begin(),currentCycleSet.domains[row][col].dom.end());
        return cycset_lits[row][col][max]; */
    }

    int cb_propagate()
    {
        // PRINT_CURRENT_LINE
        if (!propagateLiteralsCadical)
            return 0;

        // PRINT_CURRENT_LINE
        
        if (!changeInTrail)
            return 0;

        // PRINT_CURRENT_LINE

        // TODO changeInTrail = false;
        propagate();

        if (clauses.empty())
            return 0;

        auto lastClause = clauses.back();
        assert(!lastClause.empty());
        // find unassigned literal otherwise take last one; first check if clause is unit
        int nUnknown = 0;
        int unassigned = 0;
        for (auto l : lastClause)
        {
            auto entry = lit2entry[abs(l)];
            if (currentCycleSet.assignments[entry[0]][entry[1]][entry[2]] == Unknown_t)
            {
                nUnknown++;
                unassigned = l;
            }
            else if (currentCycleSet.assignments[entry[0]][entry[1]][entry[2]] == True_t && l > 0)
                return 0; // already satisfied
            else if (currentCycleSet.assignments[entry[0]][entry[1]][entry[2]] == False_t && l < 0)
                return 0; // already satisfied
        }

        if (nUnknown == 1)
        {
            // notify_assignment(unassigned, false); // push back the clause to the current trail
            clauses.pop_back(); // delete last clause
            if (unassigned > 0)
                literal2clausePos[abs(unassigned)] = lastClause;
            else
                literal2clauseNeg[abs(unassigned)] = lastClause;
            // PRINT_CURRENT_LINE
            return unassigned;
        }
        return 0;
    }

    int cb_add_reason_clause_lit(int plit)
    {
        // PRINT_CURRENT_LINE
        if (plit > 0)
        {
            if (literal2clausePos[abs(plit)].empty())
                return 0;
            auto l = literal2clausePos[abs(plit)].back();
            literal2clausePos[abs(plit)].pop_back();
            return l;
        }
        else
        {
            if (literal2clauseNeg[abs(plit)].empty())
                return 0;
            auto l = literal2clauseNeg[abs(plit)].back();
            literal2clauseNeg[abs(plit)].pop_back();
            return l;
        }
    };
};

#endif
