#include "useful.h"
#include<list>
#include "minCheckCommon.h"

/**
 * Check if cycle set is minimal and add a clause if this is not the case.
 * Throws std::vector<signed_edge_t> if current partially defined graph is not minimal
 * Throws LimitReachedException if cutoff limit is reached
 */

class MinCheck_V1 : public MinCheckCommon
{
public:
    MinCheck_V1(cycle_set_t cycset, vector<vector<vector<lit_t>>> cycset_lits);
    MinCheck_V1();
    MinCheck_V1(vector<int> diag, vector<vector<vector<lit_t>>> cycset_lits);
    void checkMinimality(vector<int> &perm, vector<vector<int>> &fixingPerms, int r, int c, int res, int d);
    void MinCheck(cycle_set_t cycset);

private:
    int getBreakingOrFixingSymms(vector<vector<int>> &fixingPerms, vector<int> &perm, int i, int j);
    int unknownIndexCase(vector<vector<int>> &fixingPerms, vector<int> &perm, int i, int j); 
    void possibleMatrixEntryPermutations(vector<int> &perm, vector<vector<int>> &pos, int i, int j);
    bool diagTest(vector<int> &perm, int i, int j);
    int knownIndexCase(vector<vector<int>> &fixingPerms, vector<int> &perm, int i, int j);
    int fixOrBreakPerm(vector<vector<int>> &fixingPerms, vector<int> &perm, int permVal, int ogVal, int i, int j);
    int getPermsOriginalKnown(vector<int> &perm, vector<vector<int>> &fixingPerms, int invVal, int ogVal);
    vector<int> extendPerm(cycle_set_t &cycset, vector<int> &perm, int d);
    bool extendPermCheck(vector<int> &perm);
    int knownInvCase(vector<vector<int>> &fixingPerms, vector<int> &perm, int ogVal, int invVal, int i, int j);
};