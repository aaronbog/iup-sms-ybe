#include "minCheckCommon.h"
#include "global.h"
#include<tuple>
#include<algorithm>
#include<set>
#include<list>
#include<iterator>

bool MinCheckCommon::preCheck(cycle_set_t &cycset, vector<vector<vector<lit_t>>> &cycset_lits){
    bool isID = true;
    for(int i = 0; i<problem_size; i++){
        for(int j = 0; j<problem_size; j++){
            if(count(cycset.matrix[j].begin(), cycset.matrix[j].end(), i)>1){
                return true;
            }
            if(cycset.matrix[i][j]==-1 ||  cycset.matrix[i][j]!=j)
                isID=false;
        }
    }
    return isID;
}


bool MinCheckCommon::permIsId(vector<int> &perm){
    for(int i=0; i<problem_size;i++){
        if(i==problem_size-1)
            return perm[i]==-1 || perm[i]==i;
        else if(perm[i]!=i)
            return false;
    }
}

int MinCheckCommon::permFullyDefinedCheck(vector<int> &perm, int i, int j){
    bool permId=true;

    if(permIsId(perm))
        return -1;

    vector<int> invperm = vector<int>(problem_size,-1);
    for(int r=0; r<problem_size; r++)
        invperm[perm[r]]=r;

    int fixes=0;
    for(int r = 0; r<problem_size;r++){
        for(int c=0; c<problem_size; c++){
            if(r<i || (r==i && c<j))
                continue;
            
            int minog = cycset.bitdomains[r][c].firstel;
            bool minOgFixed=cycset.matrix[r][c]!=-1;
            vector<int> permVal = cycset.bitdomains[perm[r]][perm[c]].options();
            int pv=-1;
            int inv=-1;
            if(permVal.size()==1){
                pv=permVal[0];
                inv=invperm[pv];
            }

            if(permVal.size()==1){
                if(inv<minog){
                    addClauses(perm,r,c);
                } else if (inv==minog){
                    continue;
                } else {
                    fixes=-1;
                    break;
                }
            } else {
                vector<int> invpermvals=vector<int>();
                for(auto p : permVal){
                    invpermvals.push_back(invperm[p]);
                }
                if(*max_element(invpermvals.begin(),invpermvals.end())<=minog){
                    addClauses(perm,r,c);
                } else {
                    fixes=-1;
                    break;
                }
            }
        }
        if(fixes!=0)
            break;
    }
    return -1;
}

void MinCheckCommon::addClauses(vector<int> &perm, int r, int c)
{
    if(logging>0){
        printf("ADDING BREAKING CLAUSES %d %d\n",r,c);
        for(int i=0;i<problem_size;i++){
            printf("%d -> %d\n",i,perm[i]);
        }
    }

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
                            if(logging>1)
                                printf("-M_%d_%d_%d ",ri,ci,i); 
                        }
                            
                        if(perm_asg==False_t){
                            toAdd.push_back(cycset_lits[perm[ri]][perm[ci]][perm[i]]);
                            if(logging>1)
                                printf("M_%d_%d_%d ",perm[ri],perm[ci],perm[i]); 
                        }
                            
                    }
                }
            }
        }

        endloopOld:
            if(diagPart && index!=cycset.matrix[r][r]){
                toAdd.push_back(cycset_lits[perm[r]][perm[c]][perm[index]]);
                toAdd.push_back(-cycset_lits[r][c][index]);
                if(logging>1){
                    printf("M_%d_%d_%d ",perm[r],perm[c],perm[index]); 
                    printf("-M_%d_%d_%d \n",r,c,index);
                }
                throw toAdd;
            } else {
                toAdd.push_back(cycset_lits[perm[r]][perm[c]][perm[problem_size-2]]);
                toAdd.push_back(-cycset_lits[r][c][problem_size-2]);
                if(logging>1){
                    printf("M_%d_%d_%d ",perm[r],perm[c],perm[problem_size-2]); 
                    printf("-M_%d_%d_%d \n",r,c,problem_size-2);
                }
                throw toAdd;
            }

    } else {
        vector<int> toExclude;
        if(cycset.matrix[r][c]==-1){
            for(auto i : cycset.bitdomains[r][c].options()){
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

