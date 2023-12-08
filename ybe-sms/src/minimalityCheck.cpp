#include "minimalityCheck.h"
#include "global.h"
#include<tuple>
#include<algorithm>
#include<set>

vector<array<int,3>> ordered_vars;
vector<array<int,3>> pvars;


void checkMinimality(cycle_set_t &cycset, vector<vector<vector<lit_t>>> &cycset_lits)
{
    /* printf("MINCHECK CALL\n");
    printPartiallyDefinedCycleSet(cycset); */
    
    ordered_vars=vector<array<int,3>>();
    pvars=vector<array<int,3>>();

    for(int i=0; i<problem_size;i++)
        for(int j=0; j<problem_size; j++)
            for(int l=problem_size-1; l>=0; l--)
                {ordered_vars.push_back({i,j,l});}

    vector<int> perm=vector<int>(problem_size,-1);
    vector<int> toPerm(problem_size);
    iota(toPerm.begin(),toPerm.end(),0);

    makePerms(perm,toPerm, toPerm,cycset,0, cycset_lits);
}

int makePerms(vector<int> &perm, vector<int> toPermute_vars, vector<int> toPermute, cycle_set_t &cycset, int d, vector<vector<vector<lit_t>>> &cycset_lits)
{
    if(d<ordered_vars.size())
    {
        vector<int>::iterator it;
        vector<int> tp=vector<int>(ordered_vars[d].begin(),ordered_vars[d].end());
        tp.erase(unique(tp.begin(),tp.end()), tp.end()); //tp=[0,3]
        
        vector<int>vars=vector<int>(tp.begin(),tp.end());
        it=set_intersection(tp.begin(),tp.end(),toPermute_vars.begin(),toPermute_vars.end(),tp.begin());
        tp.resize(it-tp.begin()); //tp=[0,3]

        vector<bool> vals=vector<bool>(toPermute.size(),false);
        fill((vals.end() - tp.size()), vals.end(), true); //tp=[0,0, 1, 1]

        vector<int> pickedVals(tp.size()); 

        vector<int> rest(problem_size);
        it=set_difference(toPermute_vars.begin(),toPermute_vars.end(),tp.begin(),tp.end(),rest.begin());
        rest.resize(it-rest.begin()); //rest=[1,2]
        
        do
        {
            vector<int> restVals;

            for(int i=0,j=0; i<toPermute.size();i++){
                if(vals[i]){
                    pickedVals[j++]=toPermute[i];
                }
            }

            restVals=vector<int>(problem_size);
            vector<int>sel=vector<int>(pickedVals.begin(),pickedVals.end());
            sort(sel.begin(),sel.end());
            it=set_difference(toPermute.begin(),toPermute.end(),sel.begin(),sel.end(),restVals.begin());
            restVals.resize(it-restVals.begin());

            do
            {
                vector<int> ext_perm=vector<int>(perm.begin(),perm.end());

                for(int i=0,j=0; i<pickedVals.size();i++){
                    ext_perm[tp[j++]]=pickedVals[i];
                }

                if(rest.size()==1)
                    {ext_perm[rest[0]]=restVals[0];
                    restVals.clear();}
                
                int toCont=permSmaller(ext_perm, d, cycset,cycset_lits);
                if(toCont==0)
                {
                    int i;
                    if(rest.size()!=1){
                        i=makePerms(ext_perm, rest, restVals, cycset,d+1,cycset_lits);
                    }
                    else
                        i= makePerms(ext_perm, vector<int>(0),  vector<int>(0), cycset,d+1,cycset_lits);
                    if(i<=0)
                        {pvars=vector<array<int,3>>(pvars.begin(),pvars.begin()+d);}
                }
                if(toCont<0)
                    {pvars=vector<array<int,3>>(pvars.begin(),pvars.begin()+d);}
                } while (next_permutation(pickedVals.begin(),pickedVals.end()));
            
        } while (next_permutation(vals.begin(), vals.end()));
        return 0;
    }
    return 0;
}

int permSmaller(vector<int> &invperm, int d, cycle_set_t &cycset, vector<vector<vector<lit_t>>> &cycset_lits)
{
    array<int,3> permed_var;
    permed_var={invperm[ordered_vars[d][0]],invperm[ordered_vars[d][1]],invperm[ordered_vars[d][2]]};

    pvars.push_back(permed_var);

    truth_vals og_asg = cycset.assignments[ordered_vars[d][0]][ordered_vars[d][1]][ordered_vars[d][2]];
    truth_vals perm_asg;

    if(permed_var[0]!=-1)
        perm_asg = cycset.assignments[permed_var[0]][permed_var[1]][permed_var[2]];
    else
        perm_asg = Unknown_t;

    if((og_asg == True_t && perm_asg==False_t) || (og_asg == True_t && perm_asg==Unknown_t) || (og_asg == Unknown_t && perm_asg==False_t))
        {   
            addClauses(cycset,cycset_lits);
            return 1; //If perm < OG -> we have found a permutation to use for breaking!
        }
    if((og_asg == False_t && perm_asg==True_t))
        return -1; //If OG < perm -> backtrack and try other permutation
    if((ordered_vars[d]==permed_var && og_asg!=Unknown_t) || (perm_asg==False_t) || (og_asg==True_t))
        return 0; //If one of these cases, check the next variables.
    return -1; //If not one of the cases above, backtrack and try other permutation.
}

void addClauses(cycle_set_t &cycset, vector<vector<vector<lit_t>>> &cycset_lits)
{

    vector<int> toAdd;
    if(pvars.size()==1)
    {
        toAdd.push_back(cycset_lits[pvars[0][0]][pvars[0][1]][pvars[0][2]]);
        toAdd.push_back(-cycset_lits[ordered_vars[0][0]][ordered_vars[0][1]][ordered_vars[0][2]]);
    }
    else
    {
        for(int i=0; i<pvars.size()-1; i++)
        {
            truth_vals og_asg = cycset.assignments[ordered_vars[i][0]][ordered_vars[i][1]][ordered_vars[i][2]];
            truth_vals perm_asg=cycset.assignments[pvars[i][0]][pvars[i][1]][pvars[i][2]];
            if(ordered_vars[i]!=pvars[i])
            {
                if(og_asg==True_t)
                    {toAdd.push_back(-cycset_lits[ordered_vars[i][0]][ordered_vars[i][1]][ordered_vars[i][2]]);}
                if(perm_asg==False_t)
                    {toAdd.push_back(cycset_lits[pvars[i][0]][pvars[i][1]][pvars[i][2]]);}
            }
        }
        toAdd.push_back(cycset_lits[pvars.back()[0]][pvars.back()[1]][pvars.back()[2]]);
        toAdd.push_back(-cycset_lits[ordered_vars[pvars.size()-1][0]][ordered_vars[pvars.size()-1][1]][ordered_vars[pvars.size()-1][2]]);
    }
    throw toAdd;
}