#include "minimalityCheck.h"
#include "global.h"
#include<tuple>
#include<vector>

void checkMinimality(cycle_set_t &cycset)
{
    printf("MINCHECK CALL\n");
    printPartiallyDefinedCycleSet(cycset);
    vector<tuple<int,int,int>> ordered_vars;
    for(int i=0; i<size;i++)
        for(int j=0; j<size; j++)
            for(int l=size-1; l>=0; l--)
                ordered_vars.push_back(make_tuple(i,j,l));

    
    
    vector<int> perm;
    vector<int> toPerm;
    for(int i=0; i<size; i++)
        toPerm.push_back(i);
    makePerms(perm,toPerm,ordered_vars,cycset);
}

void makePerms(vector<int> perm, vector<int> toPermute, vector<tuple<int,int,int>> vars, cycle_set_t &cycset)
{
    if(toPermute.size()!=0)
    {
        for(int i = toPermute.size()-1; i>=0; i--)
        {
            vector<int> p = vector<int>(perm.begin(),perm.end());
            vector<int> toPerm = vector<int>(toPermute.begin(),toPermute.end());
            p.push_back(toPerm[i]);
            toPerm.erase(toPerm.begin()+i);
            if(permSmaller(p, vars, cycset))
            {
                printf("FOUND A PERMUTATION TO USE\n");
                return;
            }
            else
            {
                makePerms(p,toPerm,vars,cycset);
            }
        }
    }
}

bool permSmaller(vector<int> perm, vector<tuple<int,int,int>> vars, cycle_set_t &cycset)
{
    vector<int> undefined;
    vector<int> invperm(size, -1);
    for(size_t i=perm.size();i<size;i++)
        undefined.push_back(i);
    for(size_t i=0,max=perm.size();i<max;i++)
        invperm[perm[i]]=i;
    vector<tuple<int,int,int>> permed_vars;
    if(undefined.size()!=0)
    {
        for(auto t : vars)
            if(invperm[get<0>(t)] != -1 && invperm[get<1>(t)] != -1 && invperm[get<2>(t)] != -1)
                permed_vars.push_back(make_tuple(invperm[get<0>(t)],invperm[get<1>(t)],invperm[get<2>(t)]));
            else
                permed_vars.push_back(make_tuple(-1,-1,-1));
    }
    else
    {
        for(auto t : vars)
            permed_vars.push_back(make_tuple(invperm[get<0>(t)],invperm[get<1>(t)],invperm[get<2>(t)]));
    }
    
    for(size_t i=0, max=permed_vars.size(); i<max; i++)
    {
        truth_vals og_asg = cycset.assignments[get<0>(vars[i])][get<1>(vars[i])][get<2>(vars[i])];
        truth_vals perm_asg;
        //printf("START COMPARING\n");
        if(get<0>(permed_vars[i])!=-1)
            perm_asg = cycset.assignments[get<0>(permed_vars[i])][get<1>(permed_vars[i])][get<2>(permed_vars[i])];
        else
            perm_asg = Unknown_t;
        if((og_asg == True_t && perm_asg==False_t) || (og_asg == True_t && perm_asg==Unknown_t) || (og_asg == Unknown_t && perm_asg==False_t))
            {
                printf("PERM SMALLER\n");
                return 1;
            }
        if((og_asg == False_t && perm_asg==True_t))
        {
            //printf("OG SMALLER\n");
            return 0;
        }
        if((vars[i]==permed_vars[i] && og_asg!=Unknown_t) || (perm_asg==False_t) || (og_asg==True_t))
        {
            //printf("CHECK NEXT\n");
            continue;
        }    
        //printf("is niks\n");
        return 0;  
    }
    return 0;
}
