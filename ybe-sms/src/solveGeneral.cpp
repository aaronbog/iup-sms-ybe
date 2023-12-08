/**
 * Implementation of common parts amongst all solvers
 */
#include "useful.h"
#include "global.h"
#include "solve.h"
#include "solveGeneral.hpp"
#include "cadical.hpp"
#include "minimalityCheck_V2.h"
#include "minimalityCheck.h"

typedef int lit_t;

bool CommonInterface::propagate()
{

  stats.callsPropagator++;
  auto start = clock();

  bool res;

  if ((stats.callsPropagator % checkFreq == 0) && checkSolutionInProp)
    res=checkMin();
  else
    res=true;
  
  stats.timePropagator += clock() - start;
  return res;
}

bool CommonInterface::checkMin()
{
  clock_t start = clock();
  bool res = true;
  cycle_set_t cycset = getCycleSet();

  try
  {
    if(v2)
      checkMinimality_v2(cycset,cycset_lits);
    else
      checkMinimality(cycset, cycset_lits);
  }
  catch (LimitReachedException e)
  {
    printf("Limit reached\n");
  }
  catch (clause_t c)
  {
    stats.nSymBreakClauses++;
    addClause(c,true);
    res=false;
  }
  catch (vector<clause_t> cs)
  {
    for(auto c : cs){
      stats.nSymBreakClauses++;
      addClause(c,true);
    }
    res=false;
  } 
  // printf("Time %f\n", ((double) clock() - start) / CLOCKS_PER_SEC);
  stats.timeMinimalityCheck += clock() - start;
  return res;
}

bool CommonInterface::check()
{
  clock_t start = clock();
  
  bool res=checkMin();
  
  stats.timePropagator += clock() - start;
  stats.callsCheck++;

  if(!res)
    return false;


  
  cycle_set_t cycset = getCycleSet();

  nModels++;
  printf("Solution %d\n", nModels);
  printCycleSet(cycset);

  if (allModels)
  {
    // exclude current graph
    cycle_set_t &cs = cycset;
    vector<lit_t> clause;
    for (int i = 0; i < problem_size; i++)
      for (int j = 0; j < problem_size; j++)
        for (int k = 0; k < problem_size; k++)
        {
          if(i==j)
            continue;
          if (cs.assignments[i][j][k] == True_t){
            clause.push_back(-cycset_lits[i][j][k]);
          }
          if (cs.assignments[i][j][k] == False_t){
            clause.push_back(cycset_lits[i][j][k]);
          }
        }
    addClause(clause, false);
    return false;
  }
  return true;
}

void CommonInterface::printStatistics()
{
  printf("Time in propagator: %f\n", ((double)stats.timePropagator) / CLOCKS_PER_SEC);
  printf("Time in minimality check: %f\n", ((double)stats.timeMinimalityCheck) / CLOCKS_PER_SEC);
  printf("Time in check full graphs: %f\n", ((double)stats.timeCheckFullGraphs) / CLOCKS_PER_SEC);
  printf("Calls of check: %lld\n", stats.callsCheck);
  printf("Calls propagator: %lld\n", stats.callsPropagator);
  printf("Number of hypercolorings: %lld\n", stats.hyperclauses);
  printf("Number of symmetry breaking constraints: %lld\n", stats.nSymBreakClauses);
  if (allModels)
    printf("Number of models: %d\n", nModels);
}

void CommonInterface::solve()
{
  // solve
  printf("Start solving\n");
  fflush(stdout);

  // get a solve handle
  int cubeCounter = 0;
  solve(vector<int>());

  printf("Searched finished\n");
  printStatistics();
}