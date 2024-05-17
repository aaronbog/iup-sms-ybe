/**
 * Implementation of common parts amongst all solvers
 */
#include "useful.h"
#include "global.h"
#include "solve.h"
#include "solveGeneral.hpp"
#include "cadical.hpp"

typedef int lit_t;

bool CommonInterface::propagate()
{
  stats.callsPropagator+=1LL;
  auto start = steady_clock::now();

  bool res;

  if ((rand() % checkFreq == 0) && checkSolutionInProp)
  {
    res=checkMin(false);
  }
  else
    res=true;
  
  stats.timePropagator += ((duration_cast<nanoseconds>(steady_clock::now()-start).count()) / 1000000000.0);;
  return res;
}

bool CommonInterface::checkMin(bool final)
{
  auto start = steady_clock::now();
  bool res = true;
  cycle_set_t cycset = getCycleSet();
  
  bool fullDefined = true;
  for(auto i = problem_size-1; i>=0; i--){
    for(auto j=problem_size-1; j>=0; j--){
        if(cycset.matrix[i][j]==-1){
          fullDefined = false;
          break;
        }
    }
    if(!fullDefined)
      break;
  }
  
  if(fullDefined)
    mincheck->final=true;
  else
    mincheck->final=final;

  bool failed=false;
  
  try
  {
    if(!mincheck->preCheck(cycset,cycset_lits)){
      mincheck->MinCheck(cycset);
      //checkMinimality(cycset,cycset_lits);
    }
  }
  catch (clause_t c)
  {
    stats.nSymBreakClauses+=1LL;
    addClause(c,true);
    res=false;
    failed=true;
  }
  catch (vector<clause_t> cs)
  {
    //for(auto c : cs){
      stats.nSymBreakClauses+=1LL;
      addClause(cs.front(),true);
    //}
    res=false;
    failed=true;
  } 
  catch (LimitReachedException)
  {
    bool failed = true;
  }

  if(fullDefined && !failed && allModels){
    nModels++;
    fprintf(output,"Solution %d\n", nModels);
    fprintCycleSet(output, cycset);
    vector<lit_t> clause;
    for (int i = 0; i < problem_size; i++)
      for (int j = 0; j < problem_size; j++)
      {
        if(diagPart && i==j)
              continue;
        for (int k = 0; k < problem_size; k++)
        {
          if (cycset.assignments[i][j][k] == True_t){
            clause.push_back(-cycset_lits[i][j][k]);
          }
        }
      }
    addClause(clause, false);
    res=false;
  }

  stats.timeMinimalityCheck += ((duration_cast<nanoseconds>(steady_clock::now()-start).count()) / 1000000000.0);
  return res;
}

bool CommonInterface::check()
{
  auto start = steady_clock::now();
  
  bool res=checkMin(true);

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
    vector<lit_t> clause;
    /* printf("------------------\n");
    printCycleSet(cycset); */
    for (int i = 0; i < problem_size; i++)
      for (int j = 0; j < problem_size; j++)
      {
        if(diagPart && i==j)
              continue;
        /* bool containsDec = false;
        int posLit = 0; */
        for (int k = 0; k < problem_size; k++)
        {
          //if(!containsDec && is_decision(cycset_lits[i][j][k]))
            //containsDec=true;
          if (cycset.assignments[i][j][k] == True_t){
            //posLit=cycset_lits[i][j][k];
            //printf("%d = M_%d,%d,%d\n",posLit, i,j,k);
            clause.push_back(-cycset_lits[i][j][k]);
          }
          //We can only add the positive lits, the negations are implied.
          //This makes the added clauses smaller.
          //Only adding the decision lits does not work somehow...
          /* if (cs.assignments[i][j][k] == False_t){
            clause.push_back(cycset_lits[i][j][k]);
          } */
        }
        /* if(containsDec){
          clause.push_back(-posLit);
        } */
      }
    //printf("EXCLUDED SOL\n");
    /* for(auto i:clause){
      printf("%d or ",i);
    }
    printf("\n"); */
    addClause(clause, false);
    return false;
  }
  return true;
}

void CommonInterface::printStatistics()
{
  printf("Time in propagator: %f\n", (stats.timePropagator));
  printf("Time in minimality check: %f\n", (stats.timeMinimalityCheck));
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