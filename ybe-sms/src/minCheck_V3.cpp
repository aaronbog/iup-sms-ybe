#include "minCheck_V3.h"
#include "global.h"
#include<queue>
#include<iterator>

MinCheck_V3::MinCheck_V3(cycle_set_t cycset, vector<vector<vector<lit_t>>> cycset_lits){
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
}

MinCheck_V3::MinCheck_V3(){
    return;
}

MinCheck_V3::MinCheck_V3(vector<int> diag, vector<vector<vector<lit_t>>> cycset_lits){
    this->cycset_lits=cycset_lits;
    this->diag=cyclePerm_t(diag);
    
    diagIsId=true;
    if(diagPart){
        for(int i=0; i<problem_size; i++){
            if(this->diag.permOf(i)!=i)
                diagIsId=false;
        }
    }
        
    initialPart = make_shared<pperm_bit>(pperm_bit(diag));
    bitInitialPart = pperm_bit(diag);
}


void MinCheck_V3::MinCheck(cycle_set_t cycset){
    /* printf("MINCHECK!!!!!!!\n");
    printPartiallyDefinedCycleSet(cycset);
    printDomains(cycset); */
    this->depth=0;
    this->cycset=cycset;
    checkMinimality(bitInitialPart, 0,1);
}

//Backtracking algorithm
void MinCheck_V3::checkMinimality(pperm_bit &perm, int r, int c){

    depth++;
    if(!final && depth>maxDepth){
        throw LimitReachedException();
    }
    
    //vector<pperm_bit> propagated_options = vector<pperm_bit>();
    queue<tuple<pperm_bit,int,int>> propagated_options = queue<tuple<pperm_bit,int,int>>();

    //filterOptions(perm, options, r, propagated_options, rows);
    filterOptions(perm, r, c, propagated_options);

    //breadth first slower
    while(!propagated_options.empty()){
        tuple<pperm_bit,int,int> opt = propagated_options.front();
        propagated_options.pop();
        pperm_bit option = get<0>(opt);
        int row = get<1>(opt);
        int col = get<2>(opt);

        if(option.fullDefined()){
            vector<int> p = option.getPerm();
            permFullyDefinedCheck(p,row,col);
            continue;
        }

        if(col<problem_size-1){
            auto new_options = option.options(col+1);
            filterOptions(option, row, col+1, propagated_options);
        } else if (row<problem_size-1) {
            auto new_options = option.options(row+1);
            filterOptions(option, row+1, 0, propagated_options);
        }
    }

    //depth first
    /* for(auto option : propagated_options){
        if(option.fullDefined()){
            vector<int> p = option.getPerm();
            permFullyDefinedCheck(p,r,c);
            continue;
        }
        
        if(c<problem_size-1){
            checkMinimality(option,r,c+1);
        } else if (r<problem_size-1) {
            checkMinimality(option,r+1,0);
        }
    } */
}

bool MinCheck_V3::propagateDecision(pperm_bit &perm, int r){
    int dec = perm.permOf(r);
    auto cycle_og = diag.cycle(r);

    //ensure cycles match == ensure diag is fixed

    if(dec==r){
        for(int i=1; i<cycle_og.size(); i++){
            bool fixed = perm.fix(cycle_og[i],cycle_og[i]);
            if(!fixed)
                return false;
        }
        return true;
    } else if(find(cycle_og.begin(),cycle_og.end(),dec)!=cycle_og.end()){
        
        int dist = find(cycle_og.begin(),cycle_og.end(),dec)-cycle_og.begin();

        int size = cycle_og.size();
        for(int i=1; i<size; i++){
            bool fixed = perm.fix(cycle_og[i],cycle_og[(i+dist)%size]);
            if(!fixed)
                return false;
        }
        return true;
    } else { 
        auto cycle_perm = diag.cycle(dec);
        if(cycle_perm.size()!=cycle_og.size())
            return false;
        for(int i=1; i<cycle_og.size();i++){
            bool fixed=perm.fix(cycle_og[i],cycle_perm[i]);
            if(!fixed)
                return false;
        }
        return true;
    }
}

bool MinCheck_V3::fixAndPropagate(pperm_bit &perm, int i, int j){
        bool fixed = perm.fix(i,j);

        if(!diagIsId && fixed)
            fixed=fixed&&propagateDecision(perm,i);
        
        return fixed;
}

void MinCheck_V3::extendPerm(pperm_bit &perm){
    for(int i=0; i<problem_size; i++){
        if(perm.permOf(i)==-1)
            fixAndPropagate(perm,i,perm.options(i)[0]);
    }
}

void MinCheck_V3::filterOptions(pperm_bit &perm, int r, int c, queue<tuple<pperm_bit,int,int>> &options_prop){
    int rperm = perm.permOf(r);
    int cperm = perm.permOf(c);
    if(rperm==-1 && cperm==-1){
        auto rowCycle = diag.cycle(r);
        bool sameCycle = find(rowCycle.begin(),rowCycle.end(),c)!=rowCycle.end();
        if(sameCycle){
            auto options = perm.options(c);
            for(int co : options){
                auto copyPerm = pperm_bit(perm);
                bool fixed = fixAndPropagate(copyPerm,c,co);
                if(!fixed)
                    continue;
                bool safe = checkPerm(copyPerm,r,c);
                if(safe){
                    options_prop.push(tuple<pperm_bit,int,int>{copyPerm,r,c});
                }
            }
            //options_prop=combinePerms(validOptions);
            //options_prop=validOptions;
        } else {
            auto roptions = perm.options(r);
            auto coptions = perm.options(c);
            for(int ro: roptions){
                for(int co : coptions){
                    auto copyPerm = pperm_bit(perm);
                    bool fixed = fixAndPropagate(copyPerm,r,ro);
                    if(!fixed)
                        continue;
                    fixed = fixed && fixAndPropagate(copyPerm,c,co);
                    if(!fixed)
                        continue;
                    bool safe = checkPerm(copyPerm,r,c);
                    if(safe){
                        options_prop.push(tuple<pperm_bit,int,int>{copyPerm,r,c});
                    }
                }
            }
            //options_prop=combinePerms(validOptions);
            //options_prop=validOptions;
        }
    }
    else if(cperm==-1){
        auto options = perm.options(c);
        for(int co : options){
            auto copyPerm = pperm_bit(perm);
            bool fixed = fixAndPropagate(copyPerm,c,co);
            if(!fixed)
                continue;
            bool safe = checkPerm(copyPerm,r,c);
            if(safe){
                options_prop.push(tuple<pperm_bit,int,int>{copyPerm,r,c});
            }
        }
        //options_prop = combinePerms(validOptions);
        //options_prop=validOptions;
    } else if (rperm==-1){
        auto options = perm.options(r);
        for(int ro : options){
            auto copyPerm = pperm_bit(perm);
            bool fixed = fixAndPropagate(copyPerm,r,ro);
            if(!fixed)
                continue;
            bool safe = checkPerm(copyPerm,r,c);
            if(safe){
                options_prop.push(tuple<pperm_bit,int,int>{copyPerm,r,c});
            }
        }
        //options_prop = combinePerms(validOptions);
        //options_prop=validOptions;
    } else {
        bool safe = checkPerm(perm,r,c);
        if(safe){
            options_prop.push(tuple<pperm_bit,int,int>{perm,r,c});
        }
    }
}

bool MinCheck_V3::checkPerm(pperm_bit &perm, int r, int c){

    int minog = cycset.bitdomains[r][c].firstel;
    bool minOgFixed=cycset.matrix[r][c]!=-1;
    vector<int> permVal = cycset.bitdomains[perm.permOf(r)][perm.permOf(c)].options();
    int pv;
    
    if(permVal.size()==1)
        pv=permVal[0];
    else
        pv=-1;
    
    int inv;
    if(permVal.size()==1)
        inv=perm.invPermOf(pv);
    else
        inv=-1;
    
    switch(permVal.size())
    {
        case 1:
            // If inv <= og -> USEFUL, else USELESS
            // < -> BREAK
            // = -> GO TO NEXT LEVEL
            if(inv!=-1){
                if(inv<minog){
                    extendPerm(perm);
                    vector<int> p = perm.getPerm();
                    addClauses(p,r,c);
                } else if (inv==minog){
                    return true;
                } else {
                    return false;
                }
            } else {
                vector<int> opt = perm.invOptions(pv);
                int m = *min_element(opt.begin(),opt.end());
                if((m<=minog && !minOgFixed) || (m<minog && minOgFixed)){
                    fixAndPropagate(perm,m,pv);
                    extendPerm(perm);
                    vector<int> p = perm.getPerm();
                    addClauses(p,r,c);
                } else if (m==minog && minOgFixed){
                    fixAndPropagate(perm,m,pv);
                    return true;
                } else {
                    return false;
                }
            }
            break;
    
        default:
            
            //make list of cycles = tuple(start cycle, end cycle), vector[options from cycle], tuple(smallest invperm option, biggest invperm option)
            vector<tuple<tuple<int,int>,vector<int>,tuple<int,int>>> cycles = vector<tuple<tuple<int,int>,vector<int>,tuple<int,int>>>();
            vector<int> alreadyDefined = vector<int>();

            for(auto pv : permVal){
                auto iopt = perm.invOptions(pv);
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
                    vector<int> p = perm.getPerm();
                    addClauses(p,r,c);
                } else {
                    return false;
                }
            }

            if(minog==problem_size-1){
                return true;
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
                    if(maxDef<=minog)
                        return true;
                    else
                        return false;
                }
            }

            //Minimize max(invperm(options))
            int min=problem_size;
            int mini = -1;
            int mint = -1;
            for(int i=starti; i<endi; i++){
                int maxsc = problem_size;
                int maxt = -1;
                for(int k=0; k<get<1>(cycles[i]).size();k++){
                    int maxscc = -1;
                    for(int l=0; l<get<1>(cycles[i]).size();l++){
                        int sc=-1;
                        if(k==l)
                            sc=0;
                        else if(get<1>(cycles[i])[l]>get<1>(cycles[i])[k])
                            sc = get<1>(cycles[i])[l]-get<1>(cycles[i])[0];
                        else if(get<1>(cycles[i])[l]<get<1>(cycles[i])[k])
                            sc=(get<1>(get<0>(cycles[i]))-get<1>(cycles[i])[0])-(get<1>(cycles[i])[l]-get<0>(get<0>(cycles[i])))+1;
                        
                        if(sc>=maxscc)
                            maxscc=sc;
                    }
                    if(maxscc<maxsc){
                        maxsc=maxscc;
                        maxt=get<1>(cycles[i])[k];
                    }
                } 

                int tail = (get<1>(get<0>(cycles[i]))-get<0>(get<0>(cycles[i])))-maxsc;
                int fst = 0;
                for(int j=0; j<cycles.size(); j++){
                    if(i==j)
                        continue;
                    else
                        fst += (get<1>(get<0>(cycles[j]))-get<0>(get<0>(cycles[j]))+1);
                }
                if(fst+tail<=min){
                    min=fst+tail;
                    mini=i;
                    mint=maxt;
                }
            }

            //If max(invperm(options)) small enough, we have found a constraining permutation
            if(get<0>(get<2>(cycles[mini]))+min<=minog){
                int begin = get<0>(get<2>(cycles[mini]));
                //COOL WE HAVE A CONSTRAINING PERMUTATION
                //If we fix perm such that starti+min is max(perm C_c) we are done
                //  Note: we don't even need to fix evt, start doesn't matter, only starti part matters
                //We can exclude the values that would make perm C_c < minog

                for(int i=starti; i<=endi; i++){
                    if(i==mini){
                        fixAndPropagate(perm,begin,mint);
                        begin+=(get<1>(get<0>(cycles[i]))-get<0>(get<0>(cycles[i])));
                    }
                    else {
                        fixAndPropagate(perm,begin,i);
                        begin+=(get<1>(get<0>(cycles[i]))-get<0>(get<0>(cycles[i])));
                    }
                }
                extendPerm(perm);
                vector<int> p = perm.getPerm();
                addClauses(p,r,c);
            } else {
                return false;
            }
            break;
    }
}

pperm_bit MinCheck_V3::inverseUnkown(pperm_bit &perm, int ogVal, int permVal){
    for(int j=0;j<=ogVal;j++){
        if(perm.fixed(j))
            continue;
        auto options = perm.options(j);
        if(find(options.begin(),options.end(),permVal)!=options.end()){
            pperm_bit copyPerm = pperm_bit(perm);

            bool fixed = fixAndPropagate(copyPerm, j, permVal);

            if(fixed)
                return copyPerm;
        }
    }
    return perm;
}