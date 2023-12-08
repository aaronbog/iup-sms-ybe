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

vector<int> extendPerm(cycle_set_t &cycset, vector<int> &perm, int d);

bool diagTest(cycle_set_t &cycset, vector<int> &perm, int i);

int fixOrBreakPerm(cycle_set_t &cycset, list<vector<int>> &fixingPerms, list<tuple<int,int,int>> &pos, vector<int> &perm, vector<int> &testPerm, int permVal, int ogVal);

int getPermsOriginalUnknown(cycle_set_t &cycset, vector<int> &perm, vector<int> &testPerm, int permVal);

int getPermsOriginalKnown(cycle_set_t &cycset, vector<int> &perm, vector<int> &testPerm, list<vector<int>> &fixingPerms, int invVal, int ogVal, int permVal);

void possiblePermsMatrixEntry(cycle_set_t &cycset, vector<int> &perm, list<tuple<int,int,int>> &pos, int i, int j);

void checkMinimality_v2(cycle_set_t &cycset, vector<vector<vector<lit_t>>> &cycset_lits);

int getBreakingOrFixingSymms(cycle_set_t &cycset, list<vector<int>> &fixingPerms, vector<int> &perm, int i, int j);

int minimalityCheck(cycle_set_t &cycset, list<vector<int>> &fixingPerms, vector<int> &perm, int r, int c, int res, vector<vector<vector<lit_t>>> &cycset_lits);

void addClauses(cycle_set_t &cycset, vector<int> &perm, int r, int c, vector<vector<vector<lit_t>>> &cycset_lits);

int unknownIndexCase(cycle_set_t &cycset, list<vector<int>> &fixingPerms, vector<int> &perm, int i, int j);

int knownIndexCase(cycle_set_t &cycset, list<vector<int>> &fixingPerms, vector<int> &perm, int i, int j);