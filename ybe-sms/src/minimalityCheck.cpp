#include "minimalityCheck.h"
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
    
    /* printf("---------------------\n");
    printPartiallyDefinedCycleSet(cycset);
    printAssignments(cycset); */
}

//Backtracking algorithm
//Creates search tree over possible permutations taking the cycle set into consideration
//starts with an empty permutation and refines it by iterating over the cycle set
//  If there's a breaking permutation based on this square in the matrix: return it
//  If there are several permutations that fix this square, try them out
//  If all permutations are bigger: no symmetries to be found here

void MinimalityChecker::checkMinimality(vector<int> &perm, vector<vector<int>> &fixingPerms, int r, int c, int res, int d){
    if(res==1)
        addClauses(cycset,perm,r,c,cycset_lits);
    
    if(res==0){
        int nextr=r;
        int nextc=c;
        if(nextc<problem_size-1){
            nextc+=1;
        } 
        else if(nextr<problem_size-1){
            nextr+=1;
            nextc=0;
        }
        else
            return;

        if(fixingPerms.size()>0){
            queue<vector<vector<int>>> toCheck;
            queue<vector<int>> permsToCheck;

            for(auto p : fixingPerms){
                if(permIsId(p)){
                    continue;
                }

                vector<vector<int>> fp;
                int found = getBreakingOrFixingSymms(fp,p,nextr,nextc);
                if(found==1){
                    addClauses(cycset,p,nextr,nextc, cycset_lits);
                } 
                else if(found==0){
                    toCheck.push(fp);
                    permsToCheck.push(p);
                }
            }
            while(!toCheck.empty()){
                checkMinimality(permsToCheck.front(),toCheck.front(),nextr,nextc,0,d+1);
                permsToCheck.pop();
                toCheck.pop();
            }
            return;
        } else
            //checkMinimality(perm, fixingPerms, nextr, nextc,0);
            return;
    }
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

int MinimalityChecker::getBreakingOrFixingSymms(vector<vector<int>> &fixingPerms, vector<int> &perm, int r, int c){
    if(perm[r]==-1 || perm[c]==-1){
        //check the given possibilities given that at least one of the indices is unknown
        return unknownIndexCase(fixingPerms,perm,r,c);
    } else {
        //check the given possibilities given that both indices are known
        return knownIndexCase(fixingPerms,perm,r,c);
    }
}

void MinimalityChecker::possibleMatrixEntryPermutations(vector<int> &perm, vector<vector<int>> &pos, int i, int j){
    //Enumerates possible permutations at this point in time taking into account:
        //the partially defined permutation
        //the partially defined cycleset
        //the current cycleset entry (all previous entries are fixed by the symmetry)

    int ogVal=cycset.matrix[i][j]; //1
    int pi=perm[i]; //0
    int pj=perm[j]; //1
    
    //enumerate possible permutations of given matrix entry
    for(int l=0; l<problem_size; l++){
        //if both indices known possibilities are l whose inverse perm is smaller than or equal to ogVal
        if(pi!=-1 && pj!=-1){
            if(ogVal!=-1){
                if(perm[l]==-1 && l<=ogVal)
                    pos.push_back(vector<int>{pi,pj,l});
            }
            else{
                if(perm[l]==-1)
                    pos.push_back(vector<int>{pi,pj,l});
            }
        }
        else{
            //if indice(s) are unknown, possibilities are l whose s.t. M[l,pj] or M[pi,l] or M[l,l] is not unassigned
            if(count(perm.begin(),perm.end(),l)==0){
                if(pj!=-1 && pi==-1 && cycset.matrix[l][pj]!=-1)
                    pos.push_back(vector<int>{l,pj,cycset.matrix[l][pj]});
                else if(pj==-1 && pi!=-1 && cycset.matrix[pi][l]!=-1)
                    pos.push_back(vector<int>{pi,l,cycset.matrix[pi][l]});
                else if(pi==-1 && pj==-1 && cycset.matrix[l][l]!=-1)
                    pos.push_back(vector<int>{l,l,cycset.matrix[l][l]});
            }
        }
    }
}

int MinimalityChecker::unknownIndexCase(vector<vector<int>> &fixingPerms, vector<int> &perm, int r, int c){

    int pr=perm[r];
    int pc=perm[c];

    vector<vector<int>> pos;
    possibleMatrixEntryPermutations(perm,pos,r,c);
    
    if(pos.size()==0)
        return -1;

    //pos contains tuples {pi,pj,l}
    for(auto t : pos){
        vector<int> p;
        copy(perm.begin(), perm.end(),back_inserter(p));
        if(pr==-1){
            if(!diagIsId && !diagTest(p,r,t[0]))
                continue;
            p[r]=t[0];
        }
        else if(pc==-1){
            if(!diagIsId && !diagTest(p,c,t[1]))
                continue;
            p[c]=t[1];
        }

        //check if we can make invperm(l)<=k
        //inperm(l)==get<2>(t)
        //k==ogVal
        vector<vector<int>> fp;
        int res = knownIndexCase(fp,p,r,c);
        if(res==1){
            perm.clear();
            copy(p.begin(),p.end(),back_inserter(perm));
            return 1;
        }
        if(res==0)
        {
            for(auto fixperm : fp){
                fixingPerms.push_back(fixperm);
            }
        }
    }

    if(fixingPerms.size()!=0)
       return 0;

    else
        return -1;
}

int MinimalityChecker::permFullyDefinedCheck(vector<int> &perm, int i, int j){
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
                if(permval<*min_element(cycset.domains[r][c].dom.begin(),cycset.domains[r][c].dom.end())){
                    addClauses(cycset,perm,r,c,cycset_lits);
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
                        addClauses(cycset,perm,r,c,cycset_lits);
                        break;
                    }
                } else {
                    fixes = -1;
                    break;
                }
            }
        }
        if(fixes!=0)
            break;
    }
    return -1;
}


int MinimalityChecker::knownIndexCase(vector<vector<int>> &fixingPerms, vector<int> &perm, int i, int j){
    //p[i], p[j] are defined, what about M[p[i],p[j]]?
    //if we can find l s.t. perm[l]=M[p[i],p[j]] && l<=M[i,j] we have can continue.

    if(find(perm.begin(),perm.end(),-1)==perm.end()){
        return permFullyDefinedCheck(perm,i,j);
    }
    
    int ogVal=cycset.matrix[i][j];
    int pi=perm[i];
    int pj=perm[j];
    int permVal=cycset.matrix[pi][pj];

    //M[p[i],p[j]] is defined
    //try to find extend perm s.t. perm[l]=M[p[i],p[j]] && l<=M[i,j]
    if(permVal!=-1)
        return fixOrBreakPerm(fixingPerms,perm,permVal,ogVal, i , j);

    //M[p[i],p[j]] is undefinded, we can only continue if M[i,j] = problem_size-1.
    else {
        if(!propagateMincheck && ogVal==problem_size-1){
            fixingPerms.push_back(perm);
            return 0;
        } else 
            return -1;
    }
}

int MinimalityChecker::fixOrBreakPerm(vector<vector<int>> &fixingPerms, vector<int> &perm, int permVal, int ogVal, int i, int j){
    //GOAL: can we extend testperm to testPerm[l]=permVal s.t. l<=ogVal

    vector<int>::iterator it = find(perm.begin(), perm.end(), permVal);

    //testPerm[l]=permVal defined
    if(it!=perm.end()){
        int invPermVal = it-perm.begin();

        return knownInvCase(fixingPerms,perm,ogVal,invPermVal, i, j);
    } 
    
    //testPerm[l]=permVal undefined
    //can we find l smaller than ogVal 
    else {
        vector<vector<int>> pos;
        possibleMatrixEntryPermutations(perm,pos,i,j);
        if(propagateMincheck && ogVal==-1){
            int min = problem_size;
            for(auto t: pos){
                if(t[2]<min && (diagIsId || diagTest(perm,t[2], permVal)))
                    min=t[2];
            }
            perm[min]=permVal;
            return knownInvCase(fixingPerms,perm,ogVal,min,i,j);
        } else {
            for(auto t : pos){
            vector<int> permCopy; 
            copy(perm.begin(), perm.end(),back_inserter(permCopy));
            if(diagIsId || diagTest(permCopy,t[2], permVal)){
                permCopy[t[2]]=permVal;
            } else {
                continue;
            }

            vector<vector<int>> fp;
            int res=knownInvCase(fp,permCopy,ogVal,t[2],i,j);
            if(res==1){
                perm.clear();
                copy(permCopy.begin(),permCopy.end(),back_inserter(perm));
                return 1;
            }
            if(res==0)
            {
                for(auto fixperm : fp){
                    fixingPerms.push_back(fixperm);
                }
            }
        }
        if(fixingPerms.size()>0)
            return 0;
        else
            return -1;
        }
    }
}

int MinimalityChecker::knownInvCase(vector<vector<int>> &fixingPerms, vector<int> &perm, int ogVal, int invVal, int i, int j){
    if(ogVal!=-1){
           if(invVal==ogVal){
                {fixingPerms.push_back(perm);
                    return 0;}
            } else if (invVal<ogVal){
                    if(!diagIsId)
                        return extendPermCheck(perm);
                    else
                        return 1;
            } else
                return -1;
        } else {
            if(propagateMincheck){
                int ogMax = *max_element(cycset.domains[i][j].dom.begin(),cycset.domains[i][j].dom.end());
                if(invVal<ogMax)
                    return 1;
            } else {
                if(invVal==0){
                    fixingPerms.push_back(perm);
                    return 0;
                }
            }
            
            return -1;
        }
}

bool MinimalityChecker::extendPermCheck(vector<int> &perm){
    vector<int> extendedPerm;
    extendedPerm=extendPerm(cycset,perm,0);
    if(extendedPerm[0]==-1)
        return -1;
    else
        {for(int i = 0; i<problem_size; i++)
            perm[i]=extendedPerm[i];
        return 1;}
}

int MinimalityChecker::getPermsOriginalKnown(vector<int> &perm, vector<vector<int>> &fixingPerms, int invVal, int ogVal){
    if(invVal==ogVal){
           {fixingPerms.push_back(perm);
            return 0;}
    } else if (invVal<ogVal){
            if(!diagIsId)
                return extendPermCheck(perm);
            else
                return 1;
    } else
        return -1;
}

bool MinimalityChecker::diagTest(vector<int> &perm, int i, int v){
    //printf("diagTest\n");
    //invperm[Mpipi]=Mii
    //Mpipi=perm[Mii]
    if(diagIsId)
        return true;
    bool res=false;
    perm[i]=v;
    int val = cycset.matrix[i][i];
    int pval = cycset.matrix[perm[i]][perm[i]];
    if(count(perm.begin(),perm.end(),pval)!=0){
        res = perm[val]==pval;
    } else {
        if(perm[val]==-1)
            {if(diagTest(perm,val,pval)){
                res=1;
            }
            else
                res=0;}
        else {
            res=perm[val]==pval;
        }
    }
    perm[i]=-1;
    return res;
}

vector<int> MinimalityChecker::extendPerm(cycle_set_t &cycset, vector<int> &perm, int d){
    if(d<problem_size){
        vector<int> toPerm=vector<int>(0);
        for(int i=0; i<problem_size; i++){
            if(count(perm.begin(),perm.end(),i)==0)
                toPerm.push_back(i);
        }
        if(perm[d]==-1){
            for(size_t i = 0; i<toPerm.size(); i++){
                vector<int> res;
                vector<int>tpCopy;
                copy(perm.begin(), perm.end(),back_inserter(res));
                copy(toPerm.begin(), toPerm.end(),back_inserter(tpCopy));
                if(diagIsId || diagTest(res,d,tpCopy[i]))
                {
                    res[d]=tpCopy[i];
                    tpCopy.erase(tpCopy.begin()+i);
                    res=extendPerm(cycset,res,d+1);
                    if(res[0]!=-1)
                        return res;
                }
            }
            return vector<int>(problem_size,-1);
        } else {
            if(d==problem_size-1)
                return perm;
            else
                return extendPerm(cycset,perm,d+1);
        }
    } else
        return perm;
}

void MinimalityChecker::addClauses(cycle_set_t &cycset, vector<int> &perm, int r, int c, vector<vector<vector<lit_t>>> &cycset_lits)
{
    /* printf("ADD CLAUSES %d,%d\n",r,c);
    printPartiallyDefinedCycleSet(cycset); */

    vector<int> toAdd;
    vector<int> invperm=vector<int>(problem_size,-1);

    vector<int> extendedPerm=extendPerm(cycset,perm,0);

    if(extendedPerm[0]==-1)
    {
        return;
    }
        

    for(int i=0; i<problem_size; i++){
        if(extendedPerm[i]!=-1){
            invperm[extendedPerm[i]]=i;
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
                    truth_vals perm_asg=cycset.assignments[extendedPerm[ri]][extendedPerm[ci]][extendedPerm[i]];
                    if(ri!=extendedPerm[ri]||ci!=extendedPerm[ci]||i!=extendedPerm[i])
                    {
                        if(og_asg==True_t){
                            toAdd.push_back(-cycset_lits[ri][ci][i]);
                            //printf("-M_%d_%d_%d ",ri,ci,i); 
                        }
                            
                        if(perm_asg==False_t){
                            toAdd.push_back(cycset_lits[extendedPerm[ri]][extendedPerm[ci]][extendedPerm[i]]);
                            //printf("M_%d_%d_%d ",extendedPerm[ri],extendedPerm[ci],extendedPerm[i]); 
                        }
                            
                    }
                }
            }
        }

        endloopOld:
            if(diagPart && index!=cycset.matrix[r][r]){
                toAdd.push_back(cycset_lits[extendedPerm[r]][extendedPerm[c]][extendedPerm[index]]);
                toAdd.push_back(-cycset_lits[r][c][index]);
                //printf("M_%d_%d_%d ",extendedPerm[r],extendedPerm[c],extendedPerm[index]); 
                //printf("-M_%d_%d_%d \n",r,c,index); 
                throw toAdd;
            }

    } else {
        vector<int> toExclude;
        if(cycset.matrix[r][c]==-1){
            for(auto i : cycset.domains[r][c].dom){
                if(i>invperm[cycset.matrix[extendedPerm[r]][extendedPerm[c]]])
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
                int perm_val=cycset.matrix[extendedPerm[ri]][extendedPerm[ci]];
                int invPermval=-1;
                if(perm_val!=-1){
                    invPermval=invperm[perm_val];
                }

                if(ri==extendedPerm[ri]&&ci==extendedPerm[ci]&&og_val==perm_val){
                    toAdd.push_back(-cycset_lits[ri][ci][og_val]);
                    printf("-M_%d_%d_%d, ",ri,ci,og_val);
                } else if(og_val==-1){
                    toAdd.push_back(-cycset_lits[extendedPerm[ri]][extendedPerm[ci]][perm_val]);
                    printf("-M_%d_%d_%d, ",extendedPerm[ri],extendedPerm[ci],problem_size-1);
                } else if (invPermval==-1) {
                    toAdd.push_back(-cycset_lits[ri][ci][problem_size-1]);
                    printf("-M_%d_%d_%d, ",ri,ci,0);
                } else {
                    toAdd.push_back(-cycset_lits[ri][ci][og_val]);
                    printf("-M_%d_%d_%d, ",ri,ci,og_val);
                    toAdd.push_back(-cycset_lits[extendedPerm[ri]][extendedPerm[ci]][perm_val]);
                    printf("-M_%d_%d_%d, ",extendedPerm[ri],extendedPerm[ci],perm_val);
                }

                if(ri==r && ci==c)
                    goto endloopNew;
            }
        }

        endloopNew:
            if(cycset.matrix[r][c]==-1){
                printf("\n");
                printf("Excluding: \n");
                vector<vector<int>> clauses;
                for(auto i : toExclude){
                    vector<int> cl;
                    copy(toAdd.begin(),toAdd.end(),back_inserter(cl));
                    cl.push_back(-cycset_lits[extendedPerm[r]][extendedPerm[c]][cycset.matrix[extendedPerm[r]][extendedPerm[c]]]);
                    printf("-M_%d_%d_%d, ",extendedPerm[r],extendedPerm[c],cycset.matrix[extendedPerm[r]][extendedPerm[c]]);
                    cl.push_back(-cycset_lits[r][c][i]);
                    printf("-M_%d_%d_%d, ",r,c,i);
                    printf("\n");
                    clauses.push_back(cl);
                }
                throw clauses;
            } else
                {printf("\n");
                throw toAdd;}

    }
}

