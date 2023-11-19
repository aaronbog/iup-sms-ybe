#include "minimalityCheck_V2.h"
#include "global.h"
#include<tuple>
#include<algorithm>
#include<set>
#include<list>
#include<queue>
#include<iterator>

void checkMinimality_v2(cycle_set_t &cycset)
{

    /* printf("MINCHECK CALL\n");
    printPartiallyDefinedCycleSet(cycset); */

    vector<int> perm=vector<int>(problem_size,-1);
    list<vector<int>> fixingPerms={};
    int res = smallerOrEqualSymms(cycset,fixingPerms,perm,0,0);
    minimalityCheck(cycset,fixingPerms,perm,0,0,res);
}

int getPermsOriginalKnown(vector<int> &perm, vector<int> &testPerm, list<vector<int>> &fixingPerms, int invVal, int ogVal, int permVal){
    if(invVal==ogVal){
        if(permVal!=-1)
            {vector<int> copyPerm;
            copy(testPerm.begin(), testPerm.end(),back_inserter(copyPerm));
            copyPerm[invVal]=permVal;
            fixingPerms.emplace_back(copyPerm);}
        else
            fixingPerms.emplace_back(testPerm);
        return 0;
    } else if (invVal<ogVal){
        if(permVal!=-1)
            testPerm[invVal]=permVal;
        if(perm!=testPerm){
            for(int i = 0; i<problem_size; i++)
                perm[i]=testPerm[i];
        }
        return 1;
    } else
        return -1;
}

int getPermsOriginalUnknown(vector<int> &perm, vector<int> &testPerm, list<vector<int>> &fixingPerms, int permVal){
    int min = problem_size;
    for(int l=0; l<problem_size; l++)
        if(testPerm[l]==-1 && l<min)
            min=l;
    if(min<problem_size){
        testPerm[min]=permVal;
        if(testPerm!=perm)
            {for(int i = 0; i<problem_size; i++)
                perm[i]=testPerm[i];}
        return 1;
    } else {
        return -1;
    }
}

void getPossiblePermutations(cycle_set_t &cycset, vector<int> &perm, list<tuple<int,int,int>> &pos, int i, int j){
    int ogVal=cycset.matrix[i][j];
    int pi=perm[i];
    int pj=perm[j];

    for(int l=0; l<problem_size; l++){
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

int unknownIndexCase(cycle_set_t &cycset, list<vector<int>> &fixingPerms, list<tuple<int,int,int>> &pos, vector<int> &perm, int i, int j){
    int ogVal=cycset.matrix[i][j];
    int pi=perm[i];
    int pj=perm[j];

    for(tuple<int,int,int> pos : pos){
        vector<int> p;
        copy(perm.begin(), perm.end(),back_inserter(p));
        if(pi==-1)
            p[i]=get<0>(pos);
        else if(pj==-1)
            p[j]=get<1>(pos);
        
        vector<int>::iterator it = find(p.begin(), p.end(), get<2>(pos));
        if(it==p.end()){
            if(ogVal!=-1){
                for(int l=0; l<problem_size; l++){
                    if(p[l]==-1 && l<=ogVal){
                        int res = getPermsOriginalKnown(perm,p,fixingPerms,l,ogVal,get<2>(pos));
                        if(res==1)
                            return res;
                    }
                }
            } else {
                return getPermsOriginalUnknown(perm,p,fixingPerms,get<2>(pos));
            } 
        } else {
            int invVal = -1;
            for(int z=0;z<problem_size;z++){
                if(p.begin()+z==it){
                    invVal=z;
                }
            }
            if(ogVal!=-1){
                int res = getPermsOriginalKnown(perm,p,fixingPerms,invVal,ogVal,-1);
                if(res!=0)
                    return res;
            } else {
                return 1;
            }
        }
    }
    return 0;
}

int knownIndexCase(cycle_set_t &cycset, list<vector<int>> &fixingPerms, list<tuple<int,int,int>> &pos, vector<int> &perm, int i, int j){
    int ogVal=cycset.matrix[i][j];
    int pi=perm[i];
    int pj=perm[j];

    int permVal=cycset.matrix[pi][pj];
    if(permVal!=-1){
        vector<int>::iterator it = find(perm.begin(), perm.end(), permVal);
        if(it!=perm.end()){
            int invPermVal = -1;
            for(int z=0;z<problem_size;z++){
                if(perm.begin()+z==it){
                    invPermVal=z;
                }
            }
            if(ogVal!=-1){
                int res = getPermsOriginalKnown(perm,perm,fixingPerms,invPermVal,ogVal,-1);
                return res;
            } else {
                return 1;
            }
        } else {
            for(tuple<int,int,int> t : pos){
                if(ogVal!=-1){
                    int res = getPermsOriginalKnown(perm,perm,fixingPerms,get<2>(t),ogVal,permVal);
                    if(res==1)
                        return res;
                } else {
                    return getPermsOriginalUnknown(perm,perm,fixingPerms,get<2>(t));
                }
            }
            return 0;
        }
    } else {
        if(ogVal==problem_size-1){
            fixingPerms.emplace_back(perm);
            return 0;
        } else if(ogVal==-1){
            vector<int>::iterator it = find(perm.begin(), perm.end(), problem_size-1);
            if(it!=perm.end()){
                for(int z=0;z<=problem_size;z++){
                    if(perm.begin()+z==it)
                        {perm[z]=problem_size-1;
                        return 1;}
                    }
            } else {
                for(int z = 0; z<problem_size;z++){
                    if(perm[z]==-1 && count(cycset.matrix[pi].begin(), cycset.matrix[pi].end(), z)!=0){
                        perm[z]=problem_size-1;
                        return 1;
                    }
                }
                return -1;
            }
        } else {
            return -1;
        }
    }
}

int smallerOrEqualSymms(cycle_set_t &cycset, list<vector<int>> &fixingPerms, vector<int> &perm, int i, int j){
    
    list<tuple<int,int,int>> posMs = {};

    getPossiblePermutations(cycset,perm,posMs,i,j);
    
    if(perm[i]==-1 || perm[j]==-1){
        return unknownIndexCase(cycset,fixingPerms,posMs,perm,i,j);
    } else {
        return knownIndexCase(cycset,fixingPerms,posMs,perm,i,j);
    }
}

int minimalityCheck(cycle_set_t &cycset, list<vector<int>> &fixingPerms, vector<int> &perm, int r, int c, int cont){
    if(cont==-1){
        //no fixing permutations
        return -1;
    }
    else if(cont==0){
        //permutations that fix in fixingPerms
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
            int res = smallerOrEqualSymms(cycset,fp,p,nextr,nextc);
            if(res==1){
                addClauses(cycset,p,nextr,nextc);
                return 1;
            } 
            else if(res==0){
                toCheck.push(fp);
                permsToCheck.push(p);
            }
        }
        while(!toCheck.empty()){
            int res = minimalityCheck(cycset,toCheck.front(),permsToCheck.front(),nextr,nextc,0);
            if(res==1){
                addClauses(cycset,permsToCheck.front(),nextr,nextc);
                return 1;
            }
            permsToCheck.pop();
            toCheck.pop();
        }
        return -1;
    } else {
        //cont==1 => breaking perm in &perm;
        addClauses(cycset,perm,r,c);
        return 1;
    }
}

void addClauses(cycle_set_t &cycset, vector<int> &perm, int r, int c)
{

    vector<int> toAdd;
    vector<int> invperm=vector<int>(problem_size,-1);
    
    vector<int> toPerm=vector<int>(0);
    for(int i=0; i<problem_size; i++){
        if(count(perm.begin(),perm.end(),i)==0)
            toPerm.emplace_back(i);
    }
    for(int i=0,j=0; i<problem_size; i++){
        if(perm[i]==-1)
            perm[i]=toPerm[j++];
    }
    for(int i=0; i<problem_size; i++){
        if(perm[i]!=-1){
            invperm[perm[i]]=i;
        }
    }

    int index=problem_size-1;
    for(int ri=0;ri<=r;ri++){
        for(int ci=0;ci<=problem_size-1;ci++){
            int ogVal=cycset.matrix[ri][ci];
            int permVal=cycset.matrix[perm[ri]][perm[ci]];
            
            if(ri==r && ci==c && ogVal!=-1){
                index=ogVal;
            }
            
            for(int i=problem_size-1; i>=0; i--){
                if(ri==r && ci==c && i==index)
                    goto endloop;
                truth_vals og_asg = cycset.assignments[ri][ci][i];
                truth_vals perm_asg=cycset.assignments[perm[ri]][perm[ci]][perm[i]];
                if(ri!=perm[ri]||ci!=perm[ci]||i!=perm[i])
                {
                    if(og_asg==True_t)
                        {toAdd.push_back(-cycset_lits[ri][ci][i]);}
                    if(perm_asg==False_t)
                        {toAdd.push_back(cycset_lits[perm[ri]][perm[ci]][perm[i]]);}
                }
            }
        }
    }

    endloop:
        toAdd.push_back(cycset_lits[perm[r]][perm[c]][perm[index]]);
        toAdd.push_back(-cycset_lits[r][c][index]);

        throw toAdd;
}