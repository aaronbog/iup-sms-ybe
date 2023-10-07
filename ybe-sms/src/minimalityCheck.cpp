#include "minimalityCheck.h"
#include "global.h"
#include<tuple>
#include<algorithm>

void checkMinimality(cycle_set_t &cycset)
{
    printf("MINCHECK CALL\n");
    printPartiallyDefinedCycleSet(cycset);
    //vector<tuple<int,int,int>> ordered_vars;
    vector<array<int,3>> ordered_vars;
    for(int i=0; i<problem_size;i++)
        for(int j=0; j<problem_size; j++)
            for(int l=problem_size-1; l>=0; l--)
                ordered_vars.push_back({i,j,l});

    vector<int> perm=vector<int>(problem_size,-1);
    //vector<int> toPerm;
    unordered_set<int> toPerm;
    for(int i=0; i<problem_size; i++)
            toPerm.insert(i);

    vector<array<int,3>> pvars;

    makePerms(perm,toPerm,ordered_vars,pvars,cycset,0);
}

int makePerms(vector<int> perm, unordered_set<int> toPermute, vector<array<int,3>> &vars, vector<array<int,3>> &pvars, cycle_set_t &cycset, int d)
{
    if(d<vars.size())
    {
        vector<int> tp=vector<int>(toPermute.size(),-1);
        //tp=[-1,-1,-1,-1]
        //perm=[-1,-1,-1,-1]
        //toPermute=[0,1,2,3]
        //vars[0]=[0,0,3]
        int i=0;
        for(auto v : vars[d])
            if(toPermute.find(v) != toPermute.end())
                {tp[i++]=v;
                toPermute.erase(v);}
                
        //tp=[-1,-1,0,3]
        //toPermute=[1,2]
        sort(tp.begin(),tp.end());
        do
        {
            vector<int> p=vector<int>(perm.begin(),perm.end());
            
            for(int i=0, j=0; i<p.size(); i++)
                if(p[i]==-1)
                    {p[i]=tp[j];
                    j++;}

            int toCont=permSmaller(p, vars, pvars, d, cycset, toPermute);
            if(toCont>0)
            {
                return 1;
            }
            if(toCont==0)
            {
                int i = makePerms(p,toPermute,vars,pvars,cycset,d+1);
                if(i>0)
                    return i;
                else
                    pvars=vector<array<int,3>>(pvars.begin(),pvars.begin()+d);
            }
            else
                pvars=vector<array<int,3>>(pvars.begin(),pvars.begin()+d);

        } while (next_permutation(tp.begin(),tp.end()));
        return 0;
    }
    return 0;
}

int permSmaller(vector<int> perm, vector<array<int,3>> &vars, vector<array<int,3>> &pvars, int d, cycle_set_t &cycset, unordered_set<int> unused)
{
    vector<int> invperm=vector<int>(problem_size, -1);

    if(unused.size()==1)
        {
            for(int i=0, j=0; i<perm.size(); i++)
                if(perm[i]==-1)
                    {perm[i]=*unused.begin();
                    unused.erase(unused.begin());
                    j++;}
        }
    
    for(int i=0; i<problem_size; i++)
        if(perm[i]!=-1)
            invperm[perm[i]]=i;

    array<int,3> permed_var;
    if(unused.size()!=0)
    {
        if(invperm[vars[d][0]] != -1 && invperm[vars[d][1]] != -1 && invperm[vars[d][2]] != -1)
            permed_var={invperm[vars[d][0]],invperm[vars[d][1]],invperm[vars[d][2]]};
        else
            permed_var={-1,-1,-1};
    }
    else
    {
        permed_var={invperm[vars[d][0]],invperm[vars[d][1]],invperm[vars[d][2]]};
    }

    pvars.push_back(permed_var);

    truth_vals og_asg = cycset.assignments[vars[d][0]][vars[d][1]][vars[d][2]];
    truth_vals perm_asg;

    if(permed_var[0]!=-1)
        perm_asg = cycset.assignments[permed_var[0]][permed_var[1]][permed_var[2]];
    else
        perm_asg = Unknown_t;

    if((og_asg == True_t && perm_asg==False_t) || (og_asg == True_t && perm_asg==Unknown_t) || (og_asg == Unknown_t && perm_asg==False_t))
        {
            if(unused.size()!=0)
                {//printf("EXTENDING PERM");
                for(int i=0, j=0; i<perm.size(); i++)
                    if(perm[i]==-1)
                        {perm[i]=*unused.begin();
                        unused.erase(unused.begin());
                        j++;}}
            
            //printf("IS KLEINER\n");
            addClauses(perm, vars, pvars, cycset);
            return 1;
        }
    if((og_asg == False_t && perm_asg==True_t))
    {
        //printf("OG SMALLER\n");
        return -1;
    }
    if((vars[d]==permed_var && og_asg!=Unknown_t) || (perm_asg==False_t) || (og_asg==True_t))
    {
        //printf("CHECK NEXT\n");
        return 0;
    } 
    return -1;
}

void addClauses(vector<int> perm, vector<array<int,3>> &vars, vector<array<int,3>> &permedVars, cycle_set_t &cycset)
{
    vector<int> toAdd;
    if(permedVars.size()==1)
    {
        toAdd.push_back(cycset_lits[permedVars[0][0]][permedVars[0][1]][permedVars[0][2]]);
        toAdd.push_back(-cycset_lits[vars[0][0]][vars[0][1]][vars[0][2]]);
    }
    else
    {
        for(int i=0; i<permedVars.size()-1; i++)
        {
            truth_vals og_asg = cycset.assignments[vars[i][0]][vars[i][1]][vars[i][2]];
            truth_vals perm_asg=cycset.assignments[permedVars[i][0]][permedVars[i][1]][permedVars[i][2]];
            if(vars[i]!=permedVars[i])
            {
                if(og_asg==True_t)
                    {toAdd.push_back(-cycset_lits[vars[i][0]][vars[i][1]][vars[i][2]]);}
                if(perm_asg==False_t)
                    {toAdd.push_back(cycset_lits[permedVars[i][0]][permedVars[i][1]][permedVars[i][2]]);}
            }
        }
        toAdd.push_back(cycset_lits[permedVars.back()[0]][permedVars.back()[1]][permedVars.back()[2]]);
        toAdd.push_back(-cycset_lits[vars[permedVars.size()-1][0]][vars[permedVars.size()-1][1]][vars[permedVars.size()-1][2]]);
    }
    throw toAdd;
}