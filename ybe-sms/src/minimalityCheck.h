#include "useful.h"

/**
 * Check if cycle set is minimal and add a clause if this is not the case.
 * Throws std::vector<signed_edge_t> if current partially defined graph is not minimal
 * Throws LimitReachedException if cutoff limit is reached
 */

void checkMinimality(cycle_set_t &cycset);

int permSmaller(vector<int> &invperm, int d, cycle_set_t &cycset);

int makePerms(vector<int> &perm, vector<int> toPermute_vars, vector<int> toPermute, cycle_set_t &cycset, int d);

void addClauses(cycle_set_t &cycset);