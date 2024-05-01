#ifndef MIN_CHECK_COMMON_H
#define MIN_CHECK_COMMON_H

#include "useful.h"
#include<list>

class LimitReachedException
{
};

class MinCheckCommon
{
public:
    bool preCheck(cycle_set_t &cycset, vector<vector<vector<lit_t>>> &cycset_lits);
    virtual void MinCheck(cycle_set_t cycset) { EXIT_UNWANTED_STATE };
    bool final;

protected:
    cycle_set_t cycset;
    vector<vector<vector<lit_t>>> cycset_lits;
    cyclePerm_t diag;
    shared_ptr<pperm_common> initialPart;
    bool diagIsId;
    int depth;
    bool permIsId(vector<int> &perm);
    int permFullyDefinedCheck(vector<int> &perm, int i, int j);
    void addClauses(vector<int> &perm, int r, int c);
    
};

#endif