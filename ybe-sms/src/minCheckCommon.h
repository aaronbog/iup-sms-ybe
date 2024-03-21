#ifndef MIN_CHECK_COMMON_H
#define MIN_CHECK_COMMON_H

#include "useful.h"
#include<list>

class MinCheckCommon
{
public:
    bool preCheck(cycle_set_t &cycset, vector<vector<vector<lit_t>>> &cycset_lits);
    virtual void MinCheck(cycle_set_t cycset) { EXIT_UNWANTED_STATE };
    virtual void checkMinimality(partialPerm_t &perm, int r) { EXIT_UNWANTED_STATE };

protected:
    cycle_set_t cycset;
    vector<vector<vector<lit_t>>> cycset_lits;
    cyclePerm_t diag;
    partialPerm_t initialPart;
    bool diagIsId;
    bool permIsId(vector<int> &perm);
    int permFullyDefinedCheck(vector<int> &perm, int i, int j);
    void addClauses(vector<int> &perm, int r, int c);
    
};

#endif