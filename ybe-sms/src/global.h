#ifndef GLOBAL_H
#define GLOBAL_H

#include "useful.h"

extern int nextFreeVariable;
extern bool allModels;
extern bool diagPart;
extern int problem_size;
extern int checkFreq;
extern int maxDepth;
extern bool doFinalCheck;
extern bool minCheckOld;


//extern vector<vector<vector<lit_t>>> cycset_lits;
//extern vector<vector<lit_t>> ybe_left_lits;
//extern vector<vector<lit_t>> ybe_right_lits;
//extern vector<vector<lit_t>> ybe_lits;
#define COUNT_ASSIGNED 0
#define ESTIMATE_SPACE 1

typedef struct
{
    chrono::_V2::steady_clock::time_point start;
    double timePropagator;
    double timeMinimalityCheck;
    double timeMinCheckSwap;
    double timeCheckFullGraphs;
    long long callsPropagator=0;
    long long callsCheck=0;
    long long nSymBreakClauses=0; // number of symmetry breaking clauses
    long long hyperclauses=0;
} statistics;

extern const char **optionsClingo;
extern int nOptionsClingo;
extern bool propagateLiteralsCadical;
extern bool checkSolutionInProp;
extern bool smallerEncoding;
extern bool propagateMincheck;
extern bool oldBreakingClauses;
extern string solOutput;

extern string proofFile;

extern FILE *addedClauses;
#endif
