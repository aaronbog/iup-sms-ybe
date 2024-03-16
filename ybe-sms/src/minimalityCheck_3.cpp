#include "minimalityCheck_3.h"
#include "global.h"
#include<tuple>
#include<algorithm>
#include<set>
#include<list>
#include<queue>
#include<iterator>

bool preCheck(cycle_set_t &cycset, vector<vector<vector<lit_t>>> &cycset_lits){
    for(int i = 0; i<=problem_size-1; i++){
        for(int j = 0; j<=problem_size-1; j++){
            if(count(cycset.matrix[j].begin(), cycset.matrix[j].end(), i)>1){
                return true;
            }
        }
    }
    return false;
}

MinimalityChecker::MinimalityChecker(cycle_set_t cycset, vector<vector<vector<lit_t>>> cycset_lits){
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

MinimalityChecker::MinimalityChecker(){
    return;
}

MinimalityChecker::MinimalityChecker(vector<int> diag, vector<vector<vector<lit_t>>> cycset_lits){
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

void MinimalityChecker::MinCheck(cycle_set_t cycset){
    this->cycset=cycset;
    checkMinimality(initialPart, 0);
}

//Backtracking algorithm
void MinimalityChecker::checkMinimality(partialPerm_t &perm, int r){

    auto options = perm.options(r);

    vector<partialPerm_t> propagated_options;

    filterOptions(perm, options, r, propagated_options);

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

bool MinimalityChecker::propagateDecision(partialPerm_t &perm, int r){
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

bool MinimalityChecker::fixAndPropagate(partialPerm_t &perm, int i, int j){
        bool fixed = perm.fix(i,j);

        if(!diagIsId && fixed)
            fixed=fixed&&propagateDecision(perm,i);
        
        return fixed;
}

void MinimalityChecker::filterOptions(partialPerm_t &perm, vector<int> &options, int r, vector<partialPerm_t> &options_prop){
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
            //p^-1 M[p(i),p(j)] unknown
            // if M[p(i),p(j)] known, extend perm s.t. p^-1 M[p(i),p(j)] <= M[i,j]
            // else skip.
            } else {
                if(permVal!=-1){
                    partialPerm_t invFixed = inverseUnkown(copyPerm,ogVal,permVal);
                    int inv = invFixed.invPermOf(permVal);
                    if(inv==ogVal)
                        options_prop.push_back(invFixed);
                    else if(inv!=-1 && inv<ogVal)
                        addClauses(invFixed.element,0,r);
                } else {
                    if(ogVal==problem_size-1){
                        options_prop.push_back(copyPerm);
                    }
                }
            }
        }
    }
}

partialPerm_t MinimalityChecker::inverseUnkown(partialPerm_t &perm, int ogVal, int permVal){
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

bool MinimalityChecker::permIsId(vector<int> &perm){
    vector<int> id = vector<int>(problem_size,-1);
    iota(id.begin(),id.end(),0);
    if(perm==id)
        return true;
    id[problem_size-1]=-1;
    if(perm==id)
        return true;
    else
        return false;
}

int MinimalityChecker::permFullyDefinedCheck(vector<int> &perm, int i, int j){
    bool permId=true;

    if(permIsId(perm))
        return -1;

    vector<int> invperm = vector<int>(problem_size,-1);
    for(int r=0; r<problem_size; r++)
        invperm[perm[r]]=r;

    vector<vector<int>> cyc = permToCyclePerm(perm);
    vector<vector<int>> permMat;
    copy(cycset.matrix.begin(), cycset.matrix.end(),back_inserter(permMat));
    apply_perm(permMat, cyc, invperm);

    int fixes=0;
    for(int r = 0; r<problem_size;r++){
        for(int c=0; c<problem_size; c++){
            if(r<i || (r==i && c<j))
                continue;
            int ogval = cycset.matrix[r][c];
            int permval = permMat[r][c];
            if(ogval==-1){
                int minog = *min_element(cycset.domains[r][c].dom.begin(),cycset.domains[r][c].dom.end());
                if(permval!=-1&&permval<=minog){
                    addClauses(perm,r,c);
                    break;
                } else {
                    fixes = -1;
                    break;
                }
            } else {
                if(permval!=-1){
                    if(permval>ogval){
                        fixes=-1;
                        break;
                    }
                    
                    if(permval<ogval){
                        addClauses(perm,r,c);
                        break;
                    }
                } else {
                    if(ogval!=problem_size-1){
                        fixes = -1;
                        break;
                    }
                }
            }
        }
        if(fixes!=0)
            break;
    }
    return -1;
}

void MinimalityChecker::addClauses(vector<int> &perm, int r, int c)
{
    /* printf("ADD BREAKING CLAUSE %d %d", r,c);
    printPartiallyDefinedCycleSet(cycset);
    printf("Breaking perm:");
    for(int i : perm){
        printf("%d, ",i);
    }
    printf("\n"); */

    vector<int> toAdd;
    vector<int> invperm=vector<int>(problem_size,-1);
        

    for(int i=0; i<problem_size; i++){
        if(perm[i]!=-1){
            invperm[perm[i]]=i;
        }
    }


    if(oldBreakingClauses){

        int index=problem_size-1;
        for(int ri=0;ri<=r;ri++){
            for(int ci=0;ci<=problem_size-1;ci++){
                if(diagPart && ri==ci)
                    continue;
                int ogVal=cycset.matrix[ri][ci];
                
                if(ri==r && ci==c && ogVal!=-1){
                    index=ogVal;
                }
                
                for(int i=problem_size-1; i>=0; i--){
                    if(ri==r && ci==c && i==index)
                        goto endloopOld;
                    if(diagPart && i==cycset.matrix[ri][ri])
                        continue;
                    truth_vals og_asg = cycset.assignments[ri][ci][i];
                    truth_vals perm_asg=cycset.assignments[perm[ri]][perm[ci]][perm[i]];
                    if(ri!=perm[ri]||ci!=perm[ci]||i!=perm[i])
                    {
                        if(og_asg==True_t){
                            toAdd.push_back(-cycset_lits[ri][ci][i]);
                            //printf("-M_%d_%d_%d ",ri,ci,i); 
                        }
                            
                        if(perm_asg==False_t){
                            toAdd.push_back(cycset_lits[perm[ri]][perm[ci]][perm[i]]);
                            //printf("M_%d_%d_%d ",perm[ri],perm[ci],perm[i]); 
                        }
                            
                    }
                }
            }
        }

        endloopOld:
            if(diagPart && index!=cycset.matrix[r][r]){
                toAdd.push_back(cycset_lits[perm[r]][perm[c]][perm[index]]);
                toAdd.push_back(-cycset_lits[r][c][index]);
                //printf("M_%d_%d_%d ",perm[r],perm[c],perm[index]); 
                //printf("-M_%d_%d_%d \n",r,c,index); 
                throw toAdd;
            } else {
                toAdd.push_back(cycset_lits[perm[r]][perm[c]][perm[problem_size-2]]);
                toAdd.push_back(-cycset_lits[r][c][problem_size-2]);
                throw toAdd;
            }

    } else {
        vector<int> toExclude;
        if(cycset.matrix[r][c]==-1){
            for(auto i : cycset.domains[r][c].dom){
                if(i>invperm[cycset.matrix[perm[r]][perm[c]]])
                    toExclude.push_back(i);
            }
        }

        for(int ri=0;ri<=r;ri++){
            for(int ci=0;ci<=problem_size-1;ci++){
                if(diagPart && ri==ci)
                    continue;

                if(ri==r && ci==c && cycset.matrix[r][c]==-1)
                    goto endloopNew;
                
                int og_val = cycset.matrix[ri][ci];
                int perm_val=cycset.matrix[perm[ri]][perm[ci]];
                int invPermval=-1;
                if(perm_val!=-1){
                    invPermval=invperm[perm_val];
                }

                if(ri==perm[ri]&&ci==perm[ci]&&og_val==perm_val){
                    toAdd.push_back(-cycset_lits[ri][ci][og_val]);
                    //printf("-M_%d_%d_%d, ",ri,ci,og_val);
                } else if(og_val==-1){
                    toAdd.push_back(-cycset_lits[perm[ri]][perm[ci]][perm_val]);
                    //printf("-M_%d_%d_%d, ",perm[ri],perm[ci],problem_size-1);
                } else if (invPermval==-1) {
                    toAdd.push_back(-cycset_lits[ri][ci][problem_size-1]);
                    //printf("-M_%d_%d_%d, ",ri,ci,0);
                } else {
                    toAdd.push_back(-cycset_lits[ri][ci][og_val]);
                    //printf("-M_%d_%d_%d, ",ri,ci,og_val);
                    toAdd.push_back(-cycset_lits[perm[ri]][perm[ci]][perm_val]);
                    //printf("-M_%d_%d_%d, ",perm[ri],perm[ci],perm_val);
                }

                if(ri==r && ci==c)
                    goto endloopNew;
            }
        }

        endloopNew:
            if(cycset.matrix[r][c]==-1){
                /* printf("\n");
                printf("Excluding: \n"); */
                vector<vector<int>> clauses;
                for(auto i : toExclude){
                    vector<int> cl;
                    copy(toAdd.begin(),toAdd.end(),back_inserter(cl));
                    cl.push_back(-cycset_lits[perm[r]][perm[c]][cycset.matrix[perm[r]][perm[c]]]);
                    //printf("-M_%d_%d_%d, ",perm[r],perm[c],cycset.matrix[perm[r]][perm[c]]);
                    cl.push_back(-cycset_lits[r][c][i]);
                    /* printf("-M_%d_%d_%d, ",r,c,i);
                    printf("\n"); */
                    clauses.push_back(cl);
                }
                throw clauses;
            } else
                {//printf("\n");
                throw toAdd;}

    }
}

