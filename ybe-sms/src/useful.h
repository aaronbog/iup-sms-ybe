#ifndef USEFUL_H
#define USEFUL_H

#include <algorithm>
#include <utility>
#include <vector>
#include <iostream>
#include <numeric>
#include <string>
#include <string.h>
#include <cassert>
#include <fstream>
#include <sstream>
#include <unordered_set>

using namespace std;

typedef int lit_t;
typedef vector<lit_t> clause_t;
typedef vector<clause_t> cnf_t;

void printCnf(cnf_t *cnf);

#define PRINT_CURRENT_LINE                            \
    printf("Line %d, file %s\n", __LINE__, __FILE__); \
    fflush(stdout);

#define EXIT_UNWANTED_STATE                                                          \
    {                                                                                \
        printf("Error: unexpected state at line %d, file %s\n", __LINE__, __FILE__); \
        exit(EXIT_FAILURE);                                                          \
    }

typedef enum {
    True_t=1,
    False_t=0,
    Unknown_t=-1
} truth_vals;

typedef struct {
    std::vector<vector<int>> matrix;
    std::vector<vector<vector<truth_vals>>> assignments;
} cycle_set_t;

void printCycleSet(const cycle_set_t &cycset);
void printPartiallyDefinedCycleSet(const cycle_set_t &cycset);
void part(int n, vector<int>& v, int level, vector<vector<int>>& parts);
void makeDiagonals(vector<vector<int>>& parts, vector<vector<int>>& permutations);

#endif
