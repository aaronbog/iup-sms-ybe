#include "minimalityCheck.h"
#include "global.h"
#include<tuple>

void checkMinimality(cycle_set_t &cycset)
{
    printf("MINCHECK CALL\n");
    printPartiallyDefinedCycleSet(cycset);
    vector<tuple<int,int,int>> ordered_vars;
    for(int i=0; i<problem_size;i++)
        for(int j=0; j<problem_size; j++)
            for(int l=problem_size-1; l>=0; l--)
                ordered_vars.push_back(make_tuple(i,j,l));

    
    
    vector<int> perm;
    vector<int> toPerm;
    
    for(int i=problem_size-1, j=0; i>j; i--, j++)
            {toPerm.push_back(j);
            toPerm.push_back(i);}
    
    if(problem_size%2!=0)
        toPerm.push_back(int(problem_size/2));
        
    printf("To Permute:");
    for(size_t i=0; i<toPerm.size();i++)
        printf("%d,",toPerm[i]);
    printf("\n");

    makePerms(perm,toPerm,ordered_vars,cycset);
}

void makePerms(vector<int> perm, vector<int> toPermute, vector<tuple<int,int,int>> &vars, cycle_set_t &cycset)
{
    if(toPermute.size()!=0)
    {
        for(size_t i = 0; i<toPermute.size(); i++)
        {
            vector<int> p = vector<int>(perm.begin(),perm.end());
            vector<int> toPerm = vector<int>(toPermute.begin(),toPermute.end());
            p.insert(p.begin(),toPerm[i]);
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

bool permSmaller(vector<int> perm, vector<tuple<int,int,int>> &vars, cycle_set_t &cycset)
{
    unordered_set<int> unused=unordered_set<int>();
    for(int i=0;i<problem_size;i++)
        unused.insert(i);
    for(size_t i=0;i<perm.size();i++)
        unused.erase(perm[i]);

    while(perm.size()<size_t(problem_size))
        perm.insert(perm.begin(), -1);

    vector<int> invperm(problem_size, -1);
    for(int i=0; i<problem_size; i++)
        if(perm[i]!=-1)
            invperm[perm[i]]=i;
    
    vector<tuple<int,int,int>> permed_vars;
    printf("Perm:");
    for(size_t i=0; i<perm.size();i++){
        printf("%d,",perm[i]);
    }
    printf("\n");
    if(unused.size()!=0)
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
    printf("OG\n");
    for(auto c : vars)
        if(cycset.assignments[get<0>(c)][get<1>(c)][get<2>(c)]!=-1)
            printf("%d ", cycset.assignments[get<0>(c)][get<1>(c)][get<2>(c)]);
        else
            printf("* ");
    printf("\n");
    printf("SYMM\n");
    for(auto c : permed_vars)
        if(get<0>(c)!=-1 && get<1>(c)!=-1 && get<2>(c)!=-1 && cycset.assignments[get<0>(c)][get<1>(c)][get<2>(c)]!=-1)
            printf("%d ", cycset.assignments[get<0>(c)][get<1>(c)][get<2>(c)]);
        else
            printf("* ");
    printf("\n");
    printf("OG\n");
    for(auto c : vars)
        printf("M_%d_%d_%d ", get<0>(c),get<1>(c),get<2>(c));
    printf("\n");
    printf("SYMM\n");
    for(auto c : permed_vars)
        if(get<0>(c)!=-1 && get<1>(c)!=-1 && get<2>(c)!=-1)
            printf("M_%d_%d_%d ", get<0>(c),get<1>(c),get<2>(c));
        else
            printf("M_*_*_* ");
    printf("\n");
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
                if(perm.size()<size_t(problem_size))
                    {printf("EXTENDING PERM");
                    for(auto itr = unused.begin(); itr != unused.end(); itr++)
                        perm.push_back(*itr);}
                addClauses(perm, vars, permed_vars, cycset);
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

void addClauses(vector<int> perm, vector<tuple<int,int,int>> &vars, vector<tuple<int,int,int>> &permedVars, cycle_set_t &cycset)
{
    return;
}
