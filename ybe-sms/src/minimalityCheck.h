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

bool permSmaller(vector<int> perm, vector<tuple<int,int,int>> &vars, cycle_set_t &cycset);

void makePerms(vector<int> perm, vector<int> toPermute, vector<tuple<int,int,int>> &vars, cycle_set_t &cycset);

void addClauses(vector<int> perm, vector<tuple<int,int,int>> &vars, vector<tuple<int,int,int>> &permedVars, cycle_set_t &cycset);