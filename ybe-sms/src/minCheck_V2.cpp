#include "minCheck_V2.h"
#include "global.h"
#include<queue>
#include<iterator>

MinCheck_V2::MinCheck_V2(cycle_set_t cycset, vector<vector<vector<lit_t>>> cycset_lits){
    this->cycset=cycle_set_t(problem_size,cycset_lits);
    this->cycset=cycset;
    this->cycset_lits=cycset_lits;
    if(diagPart){
        diagIsId=true;
        for(int i=0; i<problem_size; i++){
            if(cycset.matrix[i][i]!=i)
                diagIsId=false;
        }
    } else
        diagIsId=true;
}

MinCheck_V2::MinCheck_V2(){
    return;
}

MinCheck_V2::MinCheck_V2(vector<int> diag, vector<vector<vector<lit_t>>> cycset_lits){
    this->cycset_lits=cycset_lits;
    this->diag=cyclePerm_t(diag);

    if(diagPart){
        diagIsId=true;
        for(int i=0; i<problem_size; i++){
            if(this->diag.permOf(i)!=i)
                diagIsId=false;
        }
    } else
        diagIsId=true;

    initialPart = partialPerm_t(diag);
}


void MinCheck_V2::MinCheck(cycle_set_t cycset){
    this->cycset=cycset;
    checkMinimality(initialPart, 0);
}

//Backtracking algorithm
void MinCheck_V2::checkMinimality(partialPerm_t &perm, int r){

    vector<partialPerm_t> propagated_options = vector<partialPerm_t>();
    //queue<int> rows = queue<int>();

    auto options = perm.options(r);

    //filterOptions(perm, options, r, propagated_options, rows);
    filterOptions(perm, options, r, propagated_options);

    /* while(!propagated_options.empty()){
        partialPerm_t option = propagated_options.front();
        int row = rows.front();
        propagated_options.pop();
        rows.pop();

        if(option.fullDefined()){
            permFullyDefinedCheck(option.element,0,row);
            continue;
        }
        
        if(row<problem_size-1){
            //checkMinimality(option,r+1);

            auto new_options = option.options(row+1);

            filterOptions(option, new_options, row+1, propagated_options, rows);
        }
    } */

    //depth first
    for(auto option : propagated_options){
        if(option.fullDefined()){
            permFullyDefinedCheck(option.element,0,r);
            continue;
        }
        
        if(r<problem_size-1){
            checkMinimality(option,r+1);
        }
    }
}

bool MinCheck_V2::propagateDecision(partialPerm_t &perm, int r){
    int dec = perm.element[r];
    auto cycle_og = diag.cycle(r);

    //ensure cycles match == ensure diag is fixed

    if(dec==r){
        for(int i=1; i<cycle_og.size(); i++){
            bool fixed = perm.fix(cycle_og[i],cycle_og[i]);
            if(!fixed)
                return false;
        }
        return true;
    } else if(find(cycle_og.begin(),cycle_og.end(),dec)!=cycle_og.end()){
        
        int dist = find(cycle_og.begin(),cycle_og.end(),dec)-cycle_og.begin();

        int size = cycle_og.size();
        for(int i=1; i<size; i++){
            bool fixed = perm.fix(cycle_og[i],cycle_og[(i+dist)%size]);
            if(!fixed)
                return false;
        }
        return true;
    } else { 
        auto cycle_perm = diag.cycle(dec);
        if(cycle_perm.size()!=cycle_og.size())
            return false;
        for(int i=1; i<cycle_og.size();i++){
            bool fixed=perm.fix(cycle_og[i],cycle_perm[i]);
            if(!fixed)
                return false;
        }
        return true;
    }
}

bool MinCheck_V2::fixAndPropagate(partialPerm_t &perm, int i, int j){
        bool fixed = perm.fix(i,j);

        if(!diagIsId && fixed)
            fixed=fixed&&propagateDecision(perm,i);
        
        return fixed;
}

void MinCheck_V2::filterOptions(partialPerm_t &perm, vector<int> &options, int r, vector<partialPerm_t> &options_prop){
    for(int i=0; i<options.size();i++){
        partialPerm_t copyPerm = perm.copyPerm();

        bool fixed = fixAndPropagate(copyPerm,r,options[i]);
        
        if(!fixed)
            continue;

        int ogVal = cycset.matrix[0][r];
        int permVal = cycset.matrix[copyPerm.permOf(0)][copyPerm.permOf(r)];
        int invVal = permVal==-1 ? -1 : copyPerm.invPermOf(permVal);

        //M[i,j] unknown
        if(ogVal==-1){
            int minog = *min_element(cycset.domains[0][r].dom.begin(),cycset.domains[0][r].dom.end());
            //p^-1 M[p(i),p(j)] known
            //If less than min og, breaking perm found, else skip.
            if(invVal!=-1){
                if(invVal<=minog){
                    addClauses(copyPerm.element,0,r);
                } else {
                    continue;
                }
            //p^-1 M[p(i),p(j)] unknown
            // if M[p(i),p(j)] known, extend perm s.t. p^-1 M[p(i),p(j)] <= min M[i,j]
            // else skip.
            } else {
                if(permVal!=-1){
                    partialPerm_t invFixed = inverseUnkown(copyPerm,minog,permVal);
                    if(invFixed.invPermOf(permVal)!=-1)
                        addClauses(invFixed.element, 0,r);
                }
            }
        
        //M[i,j] known
        } else {
            //p^-1 M[p(i),p(j)] known
            // check
            if(invVal!=-1){
                if(invVal<ogVal)
                    addClauses(copyPerm.element,0,r); 
                else if (invVal==ogVal)
                    options_prop.push_back(copyPerm);
                    /* {options_prop.push(copyPerm);
                    rows.push(r);} */
            //p^-1 M[p(i),p(j)] unknown
            // if M[p(i),p(j)] known, extend perm s.t. p^-1 M[p(i),p(j)] <= M[i,j]
            // else skip.
            } else {
                if(permVal!=-1){
                    partialPerm_t invFixed = inverseUnkown(copyPerm,ogVal,permVal);
                    int inv = invFixed.invPermOf(permVal);
                    if(inv==ogVal)
                        options_prop.push_back(invFixed);
                        /* {options_prop.push(invFixed);
                        rows.push(r);} */
                    else if(inv!=-1 && inv<ogVal)
                        addClauses(invFixed.element,0,r);
                } else {
                    if(ogVal==problem_size-1){
                        options_prop.push_back(copyPerm);
                        /* {options_prop.push(copyPerm);
                        rows.push(r);} */
                    }
                }
            }
        }
    }
}

partialPerm_t MinCheck_V2::inverseUnkown(partialPerm_t &perm, int ogVal, int permVal){
    for(int j=0;j<=ogVal;j++){
        if(perm.fixed(j))
            continue;
        auto options = perm.options(j);
        if(find(options.begin(),options.end(),permVal)!=options.end()){
            partialPerm_t copyPerm = perm.copyPerm();

            bool fixed = fixAndPropagate(copyPerm, j, permVal);

            if(fixed)
                return copyPerm;
        }
    }
    return perm;
}