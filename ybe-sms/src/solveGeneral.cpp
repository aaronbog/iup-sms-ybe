/**
 * Implementation of common parts amongst all solvers
 */
#include "useful.h"
#include "global.h"
#include "solve.h"
#include "solveGeneral.hpp"
#include "cadical.hpp"
#include "minimalityCheck.h"

typedef int lit_t;

bool CommonInterface::propagate()
{

  stats.callsPropagator+=1LL;
  auto start = steady_clock::now();

  bool res;

  if ((stats.callsPropagator % checkFreq*1LL == 0) && checkSolutionInProp)
  {
    res=checkMin();
  }
  else
    res=true;
  
  stats.timePropagator += ((duration_cast<nanoseconds>(steady_clock::now()-start).count()) / 1000000000.0);;
  return res;
}

bool CommonInterface::checkMin()
{
  auto start = steady_clock::now();
  bool res = true;
  cycle_set_t cycset = getCycleSet();
  try
  {
    checkMinimality(cycset,cycset_lits);
  }
  catch (LimitReachedException e)
  {
    printf("Limit reached\n");
  }
  catch (clause_t c)
  {
    stats.nSymBreakClauses+=1LL;
    addClause(c,true);
    res=false;
  }
  catch (vector<clause_t> cs)
  {
    for(auto c : cs){
      stats.nSymBreakClauses+=1LL;
      addClause(c,true);
    }
    res=false;
  } 
  stats.timeMinimalityCheck += ((duration_cast<nanoseconds>(steady_clock::now()-start).count()) / 1000000000.0);
  return res;
}

bool CommonInterface::check()
{
  auto start = steady_clock::now();
  
  bool res=checkMin();

  stats.timePropagator += ((duration_cast<nanoseconds>(steady_clock::now()-start).count()) / 1000000000.0);
  stats.callsCheck+=1LL;

  if(!res)
    return false;


  cycle_set_t cycset = getCycleSet();

  nModels++;
  fprintf(output,"Solution %d\n", nModels);
  fprintCycleSet(output, cycset);

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
          /* if(!is_decision(cycset_lits[i][j][k]))
            continue; */
          if (cs.assignments[i][j][k] == True_t){
            clause.push_back(-cycset_lits[i][j][k]);
          }
          //We can only add the positive lits, the negation are implied.
          //This makes the added clauses smaller.
          //Only adding the decision lits does not work somehow...
          /* if (cs.assignments[i][j][k] == False_t){
            clause.push_back(cycset_lits[i][j][k]);
          } */
        }
    //printf("EXCLUDED SOL\n");
    addClause(clause, false);
    return false;
  }
  return true;
}

void CommonInterface::printStatistics()
{
  printf("Time in propagator: %f\n", (stats.timePropagator));
  printf("Time in minimality check: %f\n", (stats.timeMinimalityCheck));
  printf("Time in check full graphs: %f\n", (stats.timeCheckFullGraphs));
  printf("Calls of check: %lld\n", stats.callsCheck);
  printf("Calls propagator: %lld\n", stats.callsPropagator);
  printf("Number of symmetry breaking constraints: %lld\n", stats.nSymBreakClauses);
  if (allModels)
    printf("Number of models: %d\n", nModels);
}

void CommonInterface::solve()
{
  // solve
  printf("** Start solving\n");
  fflush(stdout);

  // get a solve handle
  int cubeCounter = 0;
  solve(vector<int>());

  printf("** Search finished\n");
  printStatistics();
}