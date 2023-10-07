#include "useful.h"

class LimitReachedException
{
};

/**
 * Check if cycle set is minimal and add a clause if this is not the case.
 * Throws std::vector<signed_edge_t> if current partially defined graph is not minimal
 * Throws LimitReachedException if cutoff limit is reached
 */

void checkMinimality(cycle_set_t &cycset);

int permSmaller(vector<int> perm, vector<array<int,3>> &vars, vector<array<int,3>> &pvars, int d, cycle_set_t &cycset, unordered_set<int> unused);

int makePerms(vector<int> perm, unordered_set<int> toPermute, vector<array<int,3>> &vars, vector<array<int,3>> &pvars, cycle_set_t &cycset, int d);

void addClauses(vector<int> perm, vector<array<int,3>> &vars, vector<array<int,3>> &pvars, cycle_set_t &cycset);