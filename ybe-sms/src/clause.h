#include "useful.h"
#include "global.h"

void encodeEntries(cnf_t *cnf);
void YBEClauses(cnf_t *cnf);
void atLeastOne(cnf_t *cnf, vector<int> alo);
void atMostOne(cnf_t *cnf, vector<int> alo);
void exactlyOne(cnf_t *cnf, vector<int> eo);
pair<int,cnf_t> commanderEncoding(vector<int> amo);