#include "minimalityCheck_V2.h"
#include "global.h"
#include<tuple>
#include<algorithm>
#include<set>
#include<list>
#include<queue>
#include<iterator>

bool diagTest(cycle_set_t &cycset, vector<int> &perm, int i){
    //printf("diagTest\n");
    //invperm[Mpipi]=Mii
    //Mpipi=perm[Mii]
    if(!diagPart)
        return true;
    int val = cycset.matrix[i][i];
    int pval = cycset.matrix[perm[i]][perm[i]];
    if(count(perm.begin(),perm.end(),pval)!=0){
        return perm[val]==pval;
    } else {
        if(perm[val]==-1)
            {perm[val]=pval;
            if(diagTest(cycset,perm,val))
                return 1;
            else
                return 0;}
        else {
            return perm[val]==pval;
        }
    }
}

void checkMinimality_v2(cycle_set_t &cycset, vector<vector<vector<lit_t>>> &cycset_lits)
{

    //printf("MINCHECK CALL\n");
    //printPartiallyDefinedCycleSet(cycset);

    vector<int> perm=vector<int>(problem_size,-1);
    list<vector<int>> fixingPerms={};
    int res = getBreakingOrFixingSymms(cycset,fixingPerms,perm,0,0);
    minimalityCheck(cycset,fixingPerms,perm,0,0,res, cycset_lits);
}

int getBreakingOrFixingSymms(cycle_set_t &cycset, list<vector<int>> &fixingPerms, vector<int> &perm, int i, int j){
    //printf("getBreakingOrFixingSymms\n");
    if(perm[i]==-1 || perm[j]==-1){
        //check the given possibilities given that at least one of the indices is unknown
        return unknownIndexCase(cycset,fixingPerms,perm,i,j);
    } else {
        //check the given possibilities given that both indices are known
        return knownIndexCase(cycset,fixingPerms,perm,i,j);
    }
}

int minimalityCheck(cycle_set_t &cycset, list<vector<int>> &fixingPerms, vector<int> &perm, int r, int c, int cont, vector<vector<vector<lit_t>>> &cycset_lits){
    if(cont==-1){
        //no fixing permutations
        return -1;
    }
    if(cont==-1){
        //no fixing permutations
        return -1;
    }
    else if(cont==0){
        //permutations that fix in &fixingPerms
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
            return -1;
        
        queue<list<vector<int>>> toCheck;
        queue<vector<int>> permsToCheck;

        for(auto p : fixingPerms){
            list<vector<int>> fp={};
            int res = getBreakingOrFixingSymms(cycset,fp,p,nextr,nextc);
            if(res==1){
                addClauses(cycset,p,nextr,nextc, cycset_lits);
                return 1;
            } 
            else if(res==0){
                toCheck.push(fp);
                permsToCheck.push(p);
            }
        }
        while(!toCheck.empty()){
            int res = minimalityCheck(cycset,toCheck.front(),permsToCheck.front(),nextr,nextc,0, cycset_lits);
            if(res==1){
                addClauses(cycset,permsToCheck.front(),nextr,nextc, cycset_lits);
                return 1;
            }
            permsToCheck.pop();
            toCheck.pop();
        }
        return -1;
    } else {
        //cont==1 => breaking perm in &perm;
        addClauses(cycset,perm,r,c, cycset_lits);
        return 1;
    }
}

void possiblePermsMatrixEntry(cycle_set_t &cycset, vector<int> &perm, list<tuple<int,int,int>> &pos, int i, int j){
    //printf("possiblePermsMatrixEntry\n");
    int ogVal=cycset.matrix[i][j]; //1
    int pi=perm[i]; //0
    int pj=perm[j]; //1
    
    //enumerate possible permutations of given matrix entry
    for(int l=0; l<problem_size; l++){
        //if both indices known possibilities are l whose inverse perm is smaller than or equal to ogVal
        if(pi!=-1 && pj!=-1){
            if(ogVal!=-1){
                if(perm[l]==-1 && l<=ogVal)
                    pos.emplace_back(make_tuple(pi,pj,l));
            }
            else{
                if(perm[l]==-1)
                    pos.emplace_back(make_tuple(pi,pj,l));
            }
        }
        else{
            //if indice(s) unknown possibilities are l whose s.t. M[l,pj] or M[pi,l] or M[l,l] not unassigned
            if(count(perm.begin(),perm.end(),l)==0){
                if(pj!=-1 && pi==-1 && cycset.matrix[l][pj]!=-1)
                    pos.emplace_back(make_tuple(l,pj,cycset.matrix[l][pj]));
                else if(pj==-1 && pi!=-1 && cycset.matrix[pi][l]!=-1)
                    pos.emplace_back(make_tuple(pi,l,cycset.matrix[pi][l]));
                else if(pi==-1 && pj==-1 && cycset.matrix[l][l]!=-1)
                    pos.emplace_back(make_tuple(l,l,cycset.matrix[l][l]));
            }
        }
    }
}

int unknownIndexCase(cycle_set_t &cycset, list<vector<int>> &fixingPerms, vector<int> &perm, int i, int j){
    //printf("unknownIndexCase\n");
    int ogVal=cycset.matrix[i][j]; //1
    int pi=perm[i]; //-1
    int pj=perm[j]; //-1

    //given a tuple M[i,j]=k, fill posMs with permutations M[pi,pj]=l that could lead to breaking or fixing permutation
    //if both indices known
    //  posMs contains tuples {pi,pj,invperm(l)} (check if invperm(l)<=k)
    //if indice unknown
    //  posMs contains tuples {pi,pj,l} (check if we can make invperm(l)<=k)
    list<tuple<int,int,int>> pos;
    possiblePermsMatrixEntry(cycset,perm,pos,i,j);
    
    if(pos.size()==0)
        return -1;
    
    //pos contains tuples {pi,pj,l}
    for(tuple<int,int,int> t : pos){
        vector<int> p;
        copy(perm.begin(), perm.end(),back_inserter(p));
        
        if(pi==-1){
            p[i]=get<0>(t);
            if(diagPart && !diagTest(cycset,p,i))
                continue;
        }
        else if(pj==-1){
            p[j]=get<1>(t);
            if(diagPart && !diagTest(cycset,p,j))
                continue;
        }

        //check if we can make invperm(l)<=k
        //inperm(l)==get<2>(t)
        //k==ogVal
        list<tuple<int,int,int>> posc;
        possiblePermsMatrixEntry(cycset,p,posc,i,j);
        int res = fixOrBreakPerm(cycset, fixingPerms,posc,perm,p,get<2>(t),ogVal);
        if(res==1)
            return res;
    }
    if(fixingPerms.size()!=0)
       return 0;
    else
        return -1;
}

int knownIndexCase(cycle_set_t &cycset, list<vector<int>> &fixingPerms, vector<int> &perm, int i, int j){
    //printf("knownIndexCase\n");
    int ogVal=cycset.matrix[i][j]; //5
    int pi=perm[i]; //0
    int pj=perm[j]; //1

    int permVal=cycset.matrix[pi][pj]; //5
    if(permVal!=-1){
        list<tuple<int,int,int>> pos;
        possiblePermsMatrixEntry(cycset,perm,pos,i,j);
        return fixOrBreakPerm(cycset, fixingPerms,pos,perm,perm,permVal,ogVal);
    } else {
        if(ogVal==problem_size-1){
            //any possible perm is safe to continue
            fixingPerms.emplace_back(perm);
            return 0;
        } else if(ogVal==-1){
            //try to find perm so that invperm will not be problem_size-1 (unknown - 0 is breaking perm)
            vector<int>::iterator it = find(perm.begin(), perm.end(), problem_size-1);
            if(it!=perm.end()){
                //find out if some other el in row is sent to problem_size-1, this el can't be sent to 4.
                for(int z=0;z<=problem_size;z++){
                    if(perm.begin()+z==it){
                        if(count(cycset.matrix[pi].begin(), cycset.matrix[pi].end(), z)!=0)
                            {vector<int> extendedPerm;
                            if(diagPart){
                                extendedPerm=extendPerm(cycset,perm,0);
                                if(extendedPerm[0]==-1)
                                    return -1;
                                else
                                    for(int i = 0; i<problem_size; i++)
                                        perm[i]=extendedPerm[i];}
                            return 1;}
                        else
                            return -1;
                    }
                }
                return -1;
            } else {
                for(int z = 0; z<problem_size;z++){
                    //send other el in row to problem_size-1, this el can't be sent to 4.
                    if(perm[z]==-1 && count(cycset.matrix[pi].begin(), cycset.matrix[pi].end(), z)!=0){
                        if(!diagPart){
                            perm[z]=problem_size-1;
                            return 1;
                        } else {
                            vector<int> copyPerm;
                            copy(perm.begin(), perm.end(),back_inserter(copyPerm));
                            copyPerm[z]=problem_size-1;
                            if(diagTest(cycset,copyPerm,z)) {
                                vector<int> extendedPerm=extendPerm(cycset,perm,0);
                                if(extendedPerm[0]==-1)
                                    return -1;
                                else {
                                    for(int i = 0; i<problem_size; i++)
                                            perm[i]=extendedPerm[i];
                                    return 1;
                                }
                            } else
                                return -1;
                        }
                    }
                }
                return -1;
            }
            return -1;
        } else {
            return -1;
        }
    }
}

int fixOrBreakPerm(cycle_set_t &cycset, list<vector<int>> &fixingPerms, list<tuple<int,int,int>> &pos, vector<int> &perm, vector<int> &testPerm, int permVal, int ogVal){
    //printf("fixOrBreakPerm\n");
    //if perm==testperm and pos!={}, we're dealing with the known indices case
    //if perm!=testperm and pos=={}, we're dealing with the unknown indices case
    
    //GOAL: can we extend testperm to testPerm[l]=permVal s.t. l<=ogVal
    vector<int>::iterator it = find(testPerm.begin(), testPerm.end(), permVal);
    if(it!=testPerm.end()){
        //testPerm[l]=permVal already defined
        int invPermVal = -1;
        for(int z=0;z<problem_size;z++){
            if(testPerm.begin()+z==it){
                invPermVal=z;
            }
        }
        //if ogVal unknown we have found a breaking perm
        //else:
        // if invPermVal<ogVal, copy testPerm to perm and return 1;
        // if invPermVal==ogVal, add testPerm to fixingPerms and return 0;
        // else return -1;
        if(ogVal!=-1){
            int res = getPermsOriginalKnown(cycset, perm,testPerm,fixingPerms,invPermVal,ogVal,-1);
            return res;
        } else {
            vector<int> extendedPerm;
            if(diagPart){
                extendedPerm=extendPerm(cycset,testPerm,0);
                if(extendedPerm[0]==-1)
                    return -1;
                else
                    for(int i = 0; i<problem_size; i++)
                        testPerm[i]=extendedPerm[i];
            }
            if(testPerm!=perm){
                for(int i = 0; i<problem_size; i++)
                    perm[i]=testPerm[i];
            }
            return 1;
        }
    } else {
        //testPerm[l]=permVal undefined
        //can we find l smaller than ogVal 
        for(tuple<int,int,int> t : pos){
            if(ogVal!=-1){
                int res = getPermsOriginalKnown(cycset, perm,testPerm,fixingPerms,get<2>(t),ogVal,permVal);
                if(res==1)
                    return res;
            } else {
                if(perm==testPerm)
                    return getPermsOriginalUnknown(cycset, perm,perm,get<2>(t));
                else
                    return getPermsOriginalUnknown(cycset, perm,testPerm,permVal);
                
            }
        }
        return 0;
    }
}

int getPermsOriginalKnown(cycle_set_t &cycset, vector<int> &perm, vector<int> &testPerm, list<vector<int>> &fixingPerms, int invVal, int ogVal, int permVal){
    //printf("getPermsOriginalKnown\n");
    //testperm=[0,1,,-1,-1,-1]

    if(invVal==ogVal){
        if(permVal!=-1)
            {vector<int> copyPerm;
            copy(testPerm.begin(), testPerm.end(),back_inserter(copyPerm));
            copyPerm[invVal]=permVal;
            if(!diagPart || diagTest(cycset,copyPerm,invVal)){
                fixingPerms.emplace_back(copyPerm);
                return 0;}
            else
                return -1;}
        else
           {fixingPerms.emplace_back(testPerm);
            return 0;}
    } else if (invVal<ogVal){

        if(permVal!=-1){
            if(!diagPart){
                testPerm[invVal]=permVal;
                if(testPerm!=perm){
                    for(int i = 0; i<problem_size; i++)
                        perm[i]=testPerm[i];
                }
                return 1;
            } else {
                vector<int> copyPerm;
                copy(testPerm.begin(), testPerm.end(),back_inserter(copyPerm));
                copyPerm[invVal]=permVal;
                if(diagTest(cycset,copyPerm,invVal)){
                    vector<int> extendedPerm=extendPerm(cycset,copyPerm,0);
                    if(extendedPerm[0]==-1)
                        return -1;
                    for(int i = 0; i<problem_size; i++)
                        testPerm[i]=extendedPerm[i];
                    if(testPerm!=perm){
                        {for(int i = 0; i<problem_size; i++)
                            perm[i]=testPerm[i];}
                    }
                    return 1;
            } else
                return -1;
            }
        } else {
            vector<int> extendedPerm;
            if(diagPart){
                extendedPerm=extendPerm(cycset,testPerm,0);
                if(extendedPerm[0]==-1)
                    return -1;
                else
                    for(int i = 0; i<problem_size; i++)
                        testPerm[i]=extendedPerm[i];
            }
            if(perm!=testPerm){
                for(int i = 0; i<problem_size; i++)
                    perm[i]=testPerm[i];
            }
            return 1;
        }
    } else
        return -1;
}

int getPermsOriginalUnknown(cycle_set_t &cycset, vector<int> &perm, vector<int> &testPerm, int permVal){
    //printf("getPermsOriginalUnknown\n");
    int min = problem_size;
    for(int l=0; l<problem_size; l++)
        if(testPerm[l]==-1 && l<min && count(testPerm.begin(), testPerm.end(),l)==0)
            min=l;
    if(min<problem_size){
        testPerm[min]=permVal;
        vector<int> extendedPerm;
        if(!diagPart || diagTest(cycset,testPerm,min)){
            if(diagPart){
                extendedPerm=extendPerm(cycset,testPerm,0);
                if(extendedPerm[0]==-1)
                    return -1;
                else
                    for(int i = 0; i<problem_size; i++)
                        testPerm[i]=extendedPerm[i];
            }
            if(testPerm!=perm){
                for(int i = 0; i<problem_size; i++)
                    perm[i]=testPerm[i];
            }
            return 1;}
        else
            return -1;
    } else {
        return -1;
    }
}

vector<int> extendPerm(cycle_set_t &cycset, vector<int> &perm, int d){
    if(d<problem_size){
        vector<int> toPerm=vector<int>(0);
        for(int i=0; i<problem_size; i++){
            if(count(perm.begin(),perm.end(),i)==0)
                toPerm.emplace_back(i);
        }
        if(perm[d]==-1){
            for(int i = 0; i<toPerm.size(); i++){
                vector<int> res;
                vector<int>tpCopy;
                copy(perm.begin(), perm.end(),back_inserter(res));
                copy(toPerm.begin(), toPerm.end(),back_inserter(tpCopy));
                res[d]=tpCopy[i];
                if(!diagPart || diagTest(cycset,res,d))
                    {tpCopy.erase(tpCopy.begin()+i);
                    res=extendPerm(cycset,res,d+1);
                    if(res[0]!=-1)
                        return res;}
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

void addClauses(cycle_set_t &cycset, vector<int> &perm, int r, int c, vector<vector<vector<lit_t>>> &cycset_lits)
{
    //printf("addClauses\n");
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
                    goto endloop;
                truth_vals og_asg = cycset.assignments[ri][ci][i];
                truth_vals perm_asg=cycset.assignments[extendedPerm[ri]][extendedPerm[ci]][extendedPerm[i]];
                if(ri!=extendedPerm[ri]||ci!=extendedPerm[ci]||i!=extendedPerm[i])
                {
                    if(og_asg==True_t)
                        {   toAdd.push_back(-cycset_lits[ri][ci][i]);}
                    if(perm_asg==False_t)
                        {   toAdd.push_back(cycset_lits[extendedPerm[ri]][extendedPerm[ci]][extendedPerm[i]]);}
                }
            }
        }
    }

    endloop:
        toAdd.push_back(cycset_lits[extendedPerm[r]][extendedPerm[c]][extendedPerm[index]]);
        toAdd.push_back(-cycset_lits[r][c][index]);
        

        throw toAdd;
}