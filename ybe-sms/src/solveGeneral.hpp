#ifndef SOLVE_GENERAL_H
#define SOLVE_GENERAL_H

#include "useful.h"

class CommonInterface
{
public:
  void solve(); // TODO suitable return value;
  virtual ~CommonInterface() {}
  int nModels = 0;
  
  // functions which must be implemented for the concrete solver
protected:
  virtual void solve(vector<int> assumptions) { EXIT_UNWANTED_STATE }              // solve the formula under the assumption
  virtual bool solve(vector<int> assumptions, int timeout) { EXIT_UNWANTED_STATE } // solve with a given timeout; return false if timeout was reached

  virtual void addClause(const vector<lit_t> &clause, bool redundant) { EXIT_UNWANTED_STATE }
  virtual cycle_set_t getCycleSet() { EXIT_UNWANTED_STATE }

  cycle_set_t currentCycleSet; // current partially defined graph 

  // functions which are the same for all solvers, which use the previous funcitons
private:
  bool checkMin(); // checks if adjacency matrix is minimal
  //bool cutoffFunction();                        // If certain number of edge variables is assigned, a cube will be generated

  //bool checkFullyDefinedCycleSet(const cycle_set_t &cycset); // check the property of the fully defined graph
  void printStatistics();
public:
  bool propagate(); // Check state of partial assignment and add clauses if necessary; returns true if no clause was added otherwise false
  bool check();     // prints and excludes graph from search space; returns true if no clause was added otherwise false
};

#endif
