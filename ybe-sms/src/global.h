#ifndef GLOBAL_H
#define GLOBAL_H

#include "useful.h"

extern int nextFreeVariable;
extern bool allModels;
extern int problem_size;

extern vector<vector<vector<lit_t>>> cycset_lits;
extern vector<vector<lit_t>> ybe_left_lits;
extern vector<vector<lit_t>> ybe_right_lits;
extern vector<vector<lit_t>> ybe_lits;
#define COUNT_ASSIGNED 0
#define ESTIMATE_SPACE 1

typedef struct
{
    clock_t start;
    clock_t timePropagator;
    clock_t timeMinimalityCheck;
    clock_t timeMinCheckSwap;
    clock_t timeCheckFullGraphs;
    long long callsPropagator;
    long long callsCheck;
    long long nSymBreakClauses; // number of symmetry breaking clauses
    long long hyperclauses;
} statistics;

extern statistics stats;

extern const char **optionsClingo;
extern int nOptionsClingo;

extern string proofFile;

extern FILE *addedClauses;
#endif
