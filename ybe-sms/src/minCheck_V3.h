#include "useful.h"
#include<queue>
#include "minCheckCommon.h"

/**
 * Check if cycle set is minimal and add a clause if this is not the case.
 * Throws std::vector<signed_edge_t> if current partially defined graph is not minimal
 * Throws LimitReachedException if cutoff limit is reached
 */

class MinCheck_V3 : public MinCheckCommon
{
public:
    MinCheck_V3(cycle_set_t cycset, vector<vector<vector<lit_t>>> cycset_lits);
    MinCheck_V3();
    MinCheck_V3(vector<int> diag, vector<vector<vector<lit_t>>> cycset_lits);
    void checkMinimality(pperm_bit &perm, int r, int c);
    void MinCheck(cycle_set_t cycset);


private:
    pperm_bit bitInitialPart;
    void extendPerm(pperm_bit &perm);
    void filterOptions(pperm_bit &perm, int r, int c, queue<tuple<pperm_bit,int,int>> &options_prop);
    bool propagateDecision(pperm_bit &perm, int r);
    bool fixAndPropagate(pperm_bit &perm, int i, int j);
    pperm_bit inverseUnkown(pperm_bit &perm, int i, int j);
    bool checkPerm(pperm_bit &perm,int r, int c);
};

bool preCheck(cycle_set_t &cycset, vector<vector<vector<lit_t>>> &cycset_lits);