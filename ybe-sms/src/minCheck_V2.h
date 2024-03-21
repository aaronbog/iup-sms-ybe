#include "useful.h"
#include<queue>
#include "minCheckCommon.h"

/**
 * Check if cycle set is minimal and add a clause if this is not the case.
 * Throws std::vector<signed_edge_t> if current partially defined graph is not minimal
 * Throws LimitReachedException if cutoff limit is reached
 */

class MinCheck_V2 : public MinCheckCommon
{
public:
    MinCheck_V2(cycle_set_t cycset, vector<vector<vector<lit_t>>> cycset_lits);
    MinCheck_V2();
    MinCheck_V2(vector<int> diag, vector<vector<vector<lit_t>>> cycset_lits);
    void checkMinimality(partialPerm_t &perm, int r);
    void MinCheck(cycle_set_t cycset);


private:
    void filterOptions(partialPerm_t &perm, vector<int> &options, int r, vector<partialPerm_t> &options_prop);
    bool propagateDecision(partialPerm_t &perm, int r);
    bool fixAndPropagate(partialPerm_t &perm, int i, int j);
    partialPerm_t inverseUnkown(partialPerm_t &perm, int i, int j);
};

bool preCheck(cycle_set_t &cycset, vector<vector<vector<lit_t>>> &cycset_lits);