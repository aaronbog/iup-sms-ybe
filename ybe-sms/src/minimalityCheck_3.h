#include "useful.h"
#include<list>

class LimitReachedException
{
};

/**
 * Check if cycle set is minimal and add a clause if this is not the case.
 * Throws std::vector<signed_edge_t> if current partially defined graph is not minimal
 * Throws LimitReachedException if cutoff limit is reached
 */

class MinimalityChecker
{
public:
    MinimalityChecker(cycle_set_t cycset, vector<vector<vector<lit_t>>> cycset_lits);
    MinimalityChecker();
    MinimalityChecker(vector<int> diag, vector<vector<vector<lit_t>>> cycset_lits);
    void checkMinimality(partialPerm_t &perm, int r);
    void MinCheck(cycle_set_t cycset);


private:
    cycle_set_t cycset;
    vector<vector<vector<lit_t>>> cycset_lits;
    cyclePerm_t diag;
    partialPerm_t initialPart;
    bool diagIsId;
    void filterOptions(partialPerm_t &perm, vector<int> &options, int r, vector<partialPerm_t> &options_prop);
    bool propagateDecision(partialPerm_t &perm, int r);
    bool permIsId(vector<int> &perm);
    int permFullyDefinedCheck(vector<int> &perm, int i, int j);
    bool fixAndPropagate(partialPerm_t &perm, int i, int j);
    partialPerm_t inverseUnkown(partialPerm_t &perm, int i, int j);
    void addClauses(vector<int> &perm, int r, int c);
};

bool preCheck(cycle_set_t &cycset, vector<vector<vector<lit_t>>> &cycset_lits);