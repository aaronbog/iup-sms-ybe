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

    printf("MINCHECK CALL\n");
    printPartiallyDefinedCycleSet(cycset);

    vector<int> perm=vector<int>(problem_size,-1);
    list<vector<int>> fixingPerms={};
    int res = smallerOrEqualSymms(cycset,fixingPerms,perm,0,0);
    minimalityCheck(cycset,fixingPerms,perm,0,0,res);
}

int smallerOrEqualSymms(cycle_set_t &cycset, list<vector<int>> &fixingPerms, vector<int> &perm, int i, int j){
    int ogVal=cycset.matrix[i][j];
    int pi=perm[i];
    int pj=perm[j];
    list<tuple<int,int,int>> posMs = {};
    
    for(int l=0; l<problem_size; l++){
        
        if(pi!=-1 && pj!=-1){
            if(ogVal!=-1){
                if(perm[l]==-1 && l<=ogVal)
                    posMs.emplace_back(make_tuple(pi,pj,l));
            }
            else{
                if(perm[l]==-1)
                    posMs.emplace_back(make_tuple(pi,pj,l));
            }
        }
        else{
            if(count(perm.begin(),perm.end(),l)==0){
                if(pj!=-1 && pi==-1 && cycset.matrix[l][pj]!=-1)
                    posMs.emplace_back(make_tuple(l,pj,cycset.matrix[l][pj]));
                else if(pj==-1 && pi!=-1 && cycset.matrix[pi][l]!=-1)
                    posMs.emplace_back(make_tuple(pi,l,cycset.matrix[pi][l]));
                else if(pi==-1 && pj==-1 && cycset.matrix[l][l]!=-1)
                    posMs.emplace_back(make_tuple(l,l,cycset.matrix[l][l]));
            }
        }
    }
    if(pi==-1 || pj==-1){
        for(tuple<int,int,int> pos : posMs){
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
                            if(l==ogVal)
                            {
                                vector<int> pp;
                                copy(p.begin(), p.end(),back_inserter(pp));
                                pp[l]=get<2>(pos);
                                fixingPerms.emplace_back(pp);
                            }
                            else if(l<ogVal){
                                p[l]=get<2>(pos);
                                for(int i = 0; i<problem_size; i++)
                                    perm[i]=p[i];
                                return 1;
                            }
                        }
                    }
                } else {
                    int min = problem_size;
                    for(int l=0; l<problem_size; l++)
                        if(p[l]==-1 && l<min)
                            min=l;
                    if(min<problem_size){
                        p[min]=get<2>(pos);
                        for(int i = 0; i<problem_size; i++)
                            perm[i]=p[i];
                        return 1;
                    } else {
                        return -1;
                    }
                } 
            } else {
                int invVal = -1;
                for(int z=0;z<problem_size;z++){
                    if(p.begin()+z==it){
                        invVal=z;
                    }
                }
                if(invVal==ogVal)
                    fixingPerms.emplace_back(p);
                else if (invVal<ogVal)
                    {
                    for(int i = 0; i<problem_size; i++)
                            perm[i]=p[i];
                    return 1;}
                else
                    return -1;
            }
        }
        return 0;
    } else {
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
                    if(invPermVal<ogVal)
                        {return 1;}
                    else if(invPermVal==ogVal)
                        {fixingPerms.emplace_back(perm);
                        return 0;}
                    else
                        return -1;
                } else {
                    return 1;
                }
            } else {
                if(ogVal!=-1){
                    for(tuple<int,int,int> t : posMs){
                        if(get<2>(t)==ogVal){
                            vector<int> p;
                            copy(perm.begin(), perm.end(),back_inserter(p));
                            p[get<2>(t)]=permVal;
                            fixingPerms.emplace_back(p);
                        } else if(get<2>(t)<ogVal){
                            perm[get<2>(t)]=permVal;
                            return 1;
                        }
                    }
                } else {
                    int min = problem_size;
                    for(tuple<int,int,int> t : posMs){
                        if(get<2>(t)<min)
                            min=get<2>(t);
                    }
                    if(min<problem_size){
                        perm[min]=permVal;
                        return 1;
                    } else {
                        return -1;
                    }
                }
                for(tuple<int,int,int> t : posMs){
                    if(ogVal!=-1){
                        if(get<2>(t)==ogVal){
                            vector<int> p;
                            copy(perm.begin(), perm.end(),back_inserter(p));
                            p[get<2>(t)]=permVal;
                            fixingPerms.emplace_back(p);
                        }
                        else if(get<2>(t)<ogVal){
                            perm[get<2>(t)]=permVal;
                            return 1;
                        }
                    } else {
                        int min = problem_size;
                        for(int l=0; l<problem_size; l++)
                            if(perm[l]!=-1 && l<min)
                                min=l;
                        if(min<problem_size){
                            perm[min]=get<2>(t);
                            return 1;
                        } else {
                            return -1;
                        }
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

    for(int ri=0;ri<=r;ri++){
        for(int ci=0;ci<=problem_size-1;ci++){
            if(ri==r && ci==c)
                break;
            int ogVal=cycset.matrix[ri][ci];
            int permVal=cycset.matrix[perm[ri]][perm[ci]];
            for(int i=problem_size-1; i>=0; i--){
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

    int index=problem_size-1;
    int ogVal=cycset.matrix[r][c];
    int permVal=cycset.matrix[perm[r]][perm[c]];
    if(ogVal!=-1)
        index=ogVal;
    for(int i=problem_size-1; i>index; i--){
        truth_vals og_asg = cycset.assignments[r][c][i];
        truth_vals perm_asg=cycset.assignments[perm[r]][perm[c]][perm[i]];
        if(r!=perm[r]||c!=perm[c]||i!=perm[i])
        {
            if(og_asg==True_t)
                {toAdd.push_back(-cycset_lits[r][c][i]);}
            if(perm_asg==False_t)
                {toAdd.push_back(cycset_lits[perm[r]][perm[c]][perm[i]]);}
        }
    }
    toAdd.push_back(cycset_lits[perm[r]][perm[c]][perm[index]]);
    toAdd.push_back(-cycset_lits[r][c][index]);

    throw toAdd;
}