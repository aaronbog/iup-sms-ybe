#include "minCheck_V2.h"
#include "global.h"
#include<queue>
#include<iterator>

MinCheck_V2::MinCheck_V2(cycle_set_t cycset, vector<vector<vector<lit_t>>> cycset_lits){
    this->cycset=cycle_set_t(problem_size,cycset_lits);
    this->cycset=cycset;
    this->cycset_lits=cycset_lits;
    diagIsId=true;
    if(diagPart){
        for(int i=0; i<problem_size; i++){
            if(cycset.matrix[i][i]!=i)
                diagIsId=false;
        }
    }
}

MinCheck_V2::MinCheck_V2(){
    return;
}

MinCheck_V2::MinCheck_V2(vector<int> diag, vector<vector<vector<lit_t>>> cycset_lits){
    this->cycset_lits=cycset_lits;
    this->diag=cyclePerm_t(diag);
    
    diagIsId=true;
    if(diagPart){
        for(int i=0; i<problem_size; i++){
            if(this->diag.permOf(i)!=i)
                diagIsId=false;
        }
    }
    if(useBit)
        initialPart = make_shared<pperm_bit>(pperm_bit(diag));
    else 
        initialPart = make_shared<pperm_plain>(pperm_plain(diag));
}


void MinCheck_V2::MinCheck(cycle_set_t cycset){
    if(logging>0){
        printf("MINCHECK!!!!!!!\n");
        printPartiallyDefinedCycleSet(cycset);
        printDomains(cycset);
    }
    this->depth=0;
    this->cycset=cycset;
    checkMinimality(initialPart, 0);
}

//Backtracking algorithm
void MinCheck_V2::checkMinimality(shared_ptr<pperm_common> perm, int r){
    if(logging>0){
        printf("row %d\n",r);
        perm->print();
    }
    depth++;
    if(!final && depth>maxDepth){
        throw LimitReachedException();
    }
    vector<shared_ptr<pperm_common>> propagated_options = vector<shared_ptr<pperm_common>>();
    auto options = perm->options(r);
    filterOptions(perm, options, r, propagated_options);


    //breadth first slower
    /* while(!propagated_options.empty()){
        partialPerm_t option = propagated_options.front();
        int row = rows.front();
        propagated_options.pop();
        rows.pop();

        if(option.fullDefined()){
            permFullyDefinedCheck(option.element,0,row);
            continue;
        }
        
        if(row<problem_size-1){
            //checkMinimality(option,r+1);

            auto new_options = option.options(row+1);

            filterOptions(option, new_options, row+1, propagated_options, rows);
        }
    } */

    //depth first
    for(auto option : propagated_options){
        if(option->fullDefined()){
            vector<int> p = option->getPerm();
            permFullyDefinedCheck(p,0,r);
            continue;
        }
        
        if(r<problem_size-1){
            checkMinimality(option,r+1);
        }
    }
}

bool MinCheck_V2::propagateDecision(shared_ptr<pperm_common> perm, int r){
    int dec = perm->permOf(r);
    auto cycle_og = diag.cycle(r);

    //ensure cycles match == ensure diag is fixed

    if(dec==r){
        for(int i=1; i<cycle_og.size(); i++){
            bool fixed = perm->fix(cycle_og[i],cycle_og[i]);
            if(!fixed)
                return false;
        }
        return true;
    } else if(find(cycle_og.begin(),cycle_og.end(),dec)!=cycle_og.end()){
        
        int dist = find(cycle_og.begin(),cycle_og.end(),dec)-cycle_og.begin();

        int size = cycle_og.size();
        for(int i=1; i<size; i++){
            bool fixed = perm->fix(cycle_og[i],cycle_og[(i+dist)%size]);
            if(!fixed)
                return false;
        }
        return true;
    } else { 
        auto cycle_perm = diag.cycle(dec);
        int size = cycle_og.size();
        if(cycle_perm.size()!=size)
            return false;
        for(int i=1; i<size;i++){
            bool fixed=perm->fix(cycle_og[i],cycle_perm[i]);
            if(!fixed)
                return false;
        }
        return true;
    }
}

bool MinCheck_V2::fixAndPropagate(shared_ptr<pperm_common> perm, int i, int j){
        bool fixed = perm->fix(i,j);

        if(fixed && !diagIsId)
            fixed=fixed&&propagateDecision(perm,i);
        
        return fixed;
}

void MinCheck_V2::extendPerm(shared_ptr<pperm_common> perm){
    if(!useBit || perm->fullDefined())
        return;

    for(int i=0; i<problem_size; i++){
        if(perm->permOf(i)==-1)
            fixAndPropagate(perm,i,perm->options(i).front());
    }
}

void MinCheck_V2::filterOptions(shared_ptr<pperm_common> perm, vector<int> &options, int r, vector<shared_ptr<pperm_common>> &options_prop){
    for(int i=0; i<options.size();i++){
        shared_ptr<pperm_common> copyPerm(nullptr);
        if(!perm->fixed(r)){
            copyPerm = perm->copyPerm();
            bool fixed = fixAndPropagate(copyPerm,r,options[i]);
            if(!fixed)
                continue;
        }
        else
            copyPerm = perm;
        int minog = cycset.bitdomains[0][r].firstel;
        bool minOgFixed=cycset.matrix[0][r]!=-1;

        vector<int> permVal = cycset.bitdomains[copyPerm->permOf(0)][copyPerm->permOf(r)].options();
        int pv=-1;
        int inv=-1;
        if(permVal.size()==1){
            pv=permVal[0];
            inv=copyPerm->invPermOf(pv);
        }

        if(permVal.size()==1){
            if(inv!=-1){
                if(inv<minog){
                    extendPerm(copyPerm);
                    vector<int> p = copyPerm->getPerm();
                    addClauses(p,0,r);
                } else if (inv==minog){
                    options_prop.push_back(copyPerm);
                }
            } else {
                vector<int> opt = copyPerm->invOptions(pv);
                int m = *min_element(opt.begin(),opt.end());
                if((m<=minog && !minOgFixed) || (m<minog && minOgFixed)){
                    fixAndPropagate(copyPerm,m,pv);
                    extendPerm(copyPerm);
                    vector<int> p = copyPerm->getPerm();
                    addClauses(p,0,r);
                } else if (m==minog && minOgFixed){
                    fixAndPropagate(copyPerm,m,pv);
                    options_prop.push_back(copyPerm);
                }
            }
        } else if (minog==problem_size-1){
            options_prop.push_back(copyPerm);
        } else if (propagateMincheck){

            //make list of cycles = tuple(start cycle, end cycle), vector[options from cycle], tuple(smallest invperm option, biggest invperm option)
            vector<tuple<tuple<int,int>,vector<int>,tuple<int,int>>> cycles = vector<tuple<tuple<int,int>,vector<int>,tuple<int,int>>>();
            vector<int> alreadyDefined = vector<int>();
            for(auto pv : permVal){
                auto iopt = copyPerm->invOptions(pv);
                if(iopt.size()==1){
                    alreadyDefined.push_back(iopt[0]);
                    continue;
                } else {
                    bool present = false;
                    for(auto t : cycles){
                        if(get<0>(get<0>(t))<=pv && get<1>(get<0>(t))>=pv){
                            present=true;
                            get<1>(t).push_back(pv);
                            break;
                        }
                    }
                    if(!present){
                        auto c = diag.cycle(pv);
                        int min = *min_element(c.begin(),c.end());
                        int max = *max_element(c.begin(),c.end());
                        int minp = *min_element(iopt.begin(),iopt.end());
                        int maxp = *max_element(iopt.begin(),iopt.end());
                        cycles.push_back(tuple<tuple<int,int>,vector<int>,tuple<int,int>>(tuple<int,int>(min,max),vector<int>(1,pv),tuple<int,int>(minp,maxp)));
                    }
                }
            }

            if(cycles.size()==0){
                if(*max_element(alreadyDefined.begin(),alreadyDefined.end())<=minog){
                    extendPerm(copyPerm);
                    vector<int> p = copyPerm->getPerm();
                    addClauses(p,0,r);
                }
                break;
            }

            //partition list of cycles according to the range of their invperm (tuple(smallest invperm option, biggest invperm option))
            vector<int> pps = vector<int>();
            partition(cycles.begin(),cycles.end(),[cycles](tuple<tuple<int,int>,vector<int>,tuple<int,int>> c){return get<2>(c)==get<2>(cycles[0]);});
            auto pp = partition_point(cycles.begin(),cycles.end(),[cycles](tuple<tuple<int,int>,vector<int>,tuple<int,int>> c){return get<2>(c)==get<2>(cycles[0]);});
            while (pp != cycles.end()){
                pps.push_back(pp-cycles.begin());
                partition(pp,cycles.end(),[cycles,pp](tuple<tuple<int,int>,vector<int>,tuple<int,int>> c){return get<2>(c)==get<2>(*pp);});
                pp=partition_point(pp,cycles.end(),[cycles,pp](tuple<tuple<int,int>,vector<int>,tuple<int,int>> c){return get<2>(c)==get<2>(*pp);});
            }
            pps.push_back(cycles.size());

            //we are only interested in cycles with highest tuple(smallest invperm option, biggest invperm option), find the index of that partition
            int max = -1;
            int prev = 0;
            int starti = prev;
            int endi = -1;
            for(auto i : pps){
                if(get<1>(get<2>(cycles[i-1]))>max){
                    max=get<1>(get<2>(cycles[i-1]));
                    starti=prev;
                    endi=i;
                }
                prev=i;
            }

            if(alreadyDefined.size()!=0){
                int maxDef = *max_element(alreadyDefined.begin(),alreadyDefined.end());
                if(maxDef > max){
                    if(maxDef<=minog){
                        extendPerm(copyPerm);
                        vector<int> p = copyPerm->getPerm();
                        addClauses(p,0,r);
                    }
                }
            }

            //Minimize max(invperm(options))
            int min=problem_size;
            int mini = -1;
            int mint = -1;
            for(int i=starti; i<endi; i++){
                int maxsc = problem_size;
                int maxt = -1;
                auto opts = get<1>(cycles[i]);
                auto oopts = get<0>(cycles[i]);
                for(int k=0; k<opts.size();k++){
                    int maxscc = -1;
                    for(int l=0; l<opts.size();l++){
                        int sc=-1;
                        if(k==l)
                            sc=0;
                        else if(opts[l]>opts[k])
                            sc = opts[l]-opts[0];
                        else if(opts[l]<opts[k])
                            sc=(get<1>(oopts)-opts[0])-(opts[l]-get<0>(oopts))+1;
                        
                        maxscc=std::max(sc,maxscc);
                    }
                    if(maxscc<maxsc){
                        maxsc=maxscc;
                        maxt=opts[k];
                    }
                } 

                int tail = (get<1>(oopts)-get<0>(oopts))-maxsc;
                int fst = 0;
                for(int j=0; j<cycles.size(); j++){
                    if(i!=j)
                        fst += (get<1>(oopts)-get<0>(oopts)+1);
                }
                if(fst+tail<=min){
                    min=fst+tail;
                    mini=i;
                    mint=maxt;
                }
            }

            //If max(invperm(options)) small enough, we have found a constraining permutation
            int begin = get<0>(get<2>(cycles[mini]));
            if(begin+min<=minog){
                //COOL WE HAVE A CONSTRAINING PERMUTATION
                //If we fix perm such that starti+min is max(perm C_c) we are done
                //  Note: we don't even need to fix evt, start doesn't matter, only starti part matters
                //We can exclude the values that would make perm C_c < minog

                for(int i=starti; i<=endi; i++){
                    if(i==mini){
                        fixAndPropagate(copyPerm,begin,mint);
                    }
                    else {
                        fixAndPropagate(copyPerm,begin,i);
                    }
                    begin+=(get<1>(get<0>(cycles[i]))-get<0>(get<0>(cycles[i])));
                }
                extendPerm(copyPerm);
                vector<int> p = copyPerm->getPerm();
                addClauses(p,0,r);
            } else if (minog==problem_size-1){
                options_prop.push_back(copyPerm);
            }
        }
    }
}

shared_ptr<pperm_common> MinCheck_V2::inverseUnkown(shared_ptr<pperm_common> perm, int ogVal, int permVal){
    for(int j=0;j<=ogVal;j++){
        if(perm->fixed(j))
            continue;
        auto options = perm->options(j);
        if(find(options.begin(),options.end(),permVal)!=options.end()){
            shared_ptr<pperm_common> copyPerm = perm->copyPerm();

            bool fixed = fixAndPropagate(copyPerm, j, permVal);

            if(fixed)
                return copyPerm;
        }
    }
    return perm;
}