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
    void checkMinimality(vector<int> &perm, vector<vector<int>> &fixingPerms, int r, int c, int res);
    int getBreakingOrFixingSymms(vector<vector<int>> &fixingPerms, vector<int> &perm, int i, int j);


private:
    cycle_set_t cycset;
    vector<vector<vector<lit_t>>> cycset_lits;
    int unknownIndexCase(vector<vector<int>> &fixingPerms, vector<int> &perm, int i, int j); 
    void possibleMatrixEntryPermutations(vector<int> &perm, vector<vector<int>> &pos, int i, int j);
    bool diagTest(vector<int> &perm, int i, int j);
    int knownIndexCase(vector<vector<int>> &fixingPerms, vector<int> &perm, int i, int j);
    int fixOrBreakPerm(vector<vector<int>> &fixingPerms, vector<vector<int>> &pos, vector<int> &perm, int permVal, int ogVal);
    int fixOrBreakPerm2(vector<vector<int>> &fixingPerms, vector<int> &perm, int permVal, int ogVal, int i, int j);
    int getPermsOriginalUnknown(vector<int> &perm, int permVal);
    int getPermsOriginalKnown(vector<int> &perm, vector<vector<int>> &fixingPerms, int invVal, int ogVal);
    vector<int> extendPerm(cycle_set_t &cycset, vector<int> &perm, int d);
    void addClauses(cycle_set_t &cycset, vector<int> &perm, int r, int c, vector<vector<vector<lit_t>>> &cycset_lits);
};

bool preCheck(cycle_set_t &cycset, vector<vector<vector<lit_t>>> &cycset_lits);