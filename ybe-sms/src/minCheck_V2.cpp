#include "minCheck_V2.h"
#include "global.h"
#include<queue>
#include<iterator>

MinCheck_V2::MinCheck_V2(cycle_set_t cycset, vector<vector<vector<lit_t>>> cycset_lits){
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
    /* printf("MINCHECK!!!!!!!\n");
    printPartiallyDefinedCycleSet(cycset);
    printDomains(cycset); */
    this->depth=0;
    this->cycset=cycset;
    checkMinimality(initialPart, 0);
}

//Backtracking algorithm
void MinCheck_V2::checkMinimality(shared_ptr<pperm_common> perm, int r){

    /* printf("row %d\n",r);
    perm->print(); */
    depth++;
    if(!final && depth>maxDepth){
        throw LimitReachedException();
    }
    
    vector<shared_ptr<pperm_common>> propagated_options = vector<shared_ptr<pperm_common>>();
    //queue<int> rows = queue<int>();

    auto options = perm->options(r);

    //filterOptions(perm, options, r, propagated_options, rows);
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
        if(cycle_perm.size()!=cycle_og.size())
            return false;
        for(int i=1; i<cycle_og.size();i++){
            bool fixed=perm->fix(cycle_og[i],cycle_perm[i]);
            if(!fixed)
                return false;
        }
        return true;
    }
}

bool MinCheck_V2::fixAndPropagate(shared_ptr<pperm_common> perm, int i, int j){
        bool fixed = perm->fix(i,j);

        if(!diagIsId && fixed)
            fixed=fixed&&propagateDecision(perm,i);
        
        return fixed;
}

void MinCheck_V2::extendPerm(shared_ptr<pperm_common> perm){
    if(!useBit)
        return;

    for(int i=0; i<problem_size; i++){
        if(perm->permOf(i)==-1)
            fixAndPropagate(perm,i,perm->options(i)[0]);
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

        int minog = cycset.bitdomains[0][r].dom.find_first();
        bool minOgFixed=cycset.matrix[0][r]!=-1;

        vector<int> permVal = cycset.bitdomains[copyPerm->permOf(0)][copyPerm->permOf(r)].options();
        int pv;
        if(permVal.size()==1)
            pv=permVal[0];
        else
            pv=-1;
        int inv;
        if(permVal.size()==1)
            inv=copyPerm->invPermOf(pv);
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
                        extendPerm(copyPerm);
                        vector<int> p = copyPerm->getPerm();
                        addClauses(p,0,r);
                    } else if (inv==minog){
                        //go to next level
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
                break;
        
            default:
                //copyPerm->print();
                vector<tuple<tuple<int,int>,vector<int>,tuple<int,int>>> cycles = vector<tuple<tuple<int,int>,vector<int>,tuple<int,int>>>();
                for(auto pv : permVal){
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
                        vector<int> par = copyPerm->invOptions(pv);
                        int minp = *min_element(par.begin(),par.end());
                        int maxp = *max_element(par.begin(),par.end());
                        cycles.push_back(tuple<tuple<int,int>,vector<int>,tuple<int,int>>(tuple<int,int>(min,max),vector<int>(1,pv),tuple<int,int>(minp,maxp)));
                    }
                }
                vector<int> pps = vector<int>();
                partition(cycles.begin(),cycles.end(),[cycles](tuple<tuple<int,int>,vector<int>,tuple<int,int>> c){return get<2>(c)==get<2>(cycles[0]);});
                auto pp = partition_point(cycles.begin(),cycles.end(),[cycles](tuple<tuple<int,int>,vector<int>,tuple<int,int>> c){return get<2>(c)==get<2>(cycles[0]);});
                while (pp != cycles.end()){
                    pps.push_back(pp-cycles.begin());
                    partition(pp,cycles.end(),[cycles,pp](tuple<tuple<int,int>,vector<int>,tuple<int,int>> c){return get<2>(c)==get<2>(*pp);});
                    pp=partition_point(pp,cycles.end(),[cycles,pp](tuple<tuple<int,int>,vector<int>,tuple<int,int>> c){return get<2>(c)==get<2>(*pp);});
                }
                pps.push_back(cycles.size());
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
                int min=problem_size;
                int mini = -1;
                int mint = -1;
                int i = starti;
                while (i<endi)
                {
                    int maxsc = problem_size;
                    int maxt = -1;
                    int start = get<1>(cycles[i])[0];
                    do {
                        int first = get<1>(cycles[i])[0];
                        int maxscc = -1;
                        for(auto j : get<1>(cycles[i])){
                            int sc=-1;
                            if(j==first)
                                sc=0;
                            else if(j>first)
                                sc = j-start;
                            else if(j<first)
                                sc=(get<1>(get<0>(cycles[i]))-start)-(j-get<0>(get<0>(cycles[i])))+1;
                            
                            if(sc>=maxscc)
                                maxscc=sc;
                        }
                        if(maxscc<maxsc){
                            maxsc=maxscc;
                            maxt=first;
                        }

                        int prev = 0;
                        int swp = get<1>(cycles[i])[0];
                        int max = get<1>(cycles[i]).size();
                        
                        for(int i=1; i<max;i++){
                            get<1>(cycles[i])[prev]=get<1>(cycles[i])[i];
                            prev=i;
                        }
                        get<1>(cycles[i])[prev]=swp;
                    } while (get<1>(cycles[i])[0]!=start);
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
                    i++;
                }

                if(get<0>(get<2>(cycles[mini]))+min<=minog){
                    int begin = get<0>(get<2>(cycles[mini]));
                    //COOL WE HAVE A CONSTRAINING PERMUTATION
                    //If we fix perm such that starti+min is max(perm C_c) we are done
                    //  Note: we don't even need to fix evt, start doesn't matter, only starti part matters
                    //We can exclude the values that would make perm C_c < minog

                    for(int i=starti; i<=endi; i++){
                        if(i==mini){
                            fixAndPropagate(copyPerm,begin,mint);
                            begin+=(get<1>(get<0>(cycles[i]))-get<0>(get<0>(cycles[i])));
                        }
                        else {
                            fixAndPropagate(copyPerm,begin,i);
                            begin+=(get<1>(get<0>(cycles[i]))-get<0>(get<0>(cycles[i])));
                        }
                    }
                    extendPerm(copyPerm);
                    vector<int> p = copyPerm->getPerm();
                    addClauses(p,0,r);
                }
                break;
        }
    }
}

void MinCheck_V2::filterOptions2(shared_ptr<pperm_common> perm, vector<int> &options, int r, vector<shared_ptr<pperm_common>> &options_prop){
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

        int ogVal = cycset.matrix[0][r];
        int permVal = cycset.matrix[copyPerm->permOf(0)][copyPerm->permOf(r)];
        int invVal = permVal==-1 ? -1 : copyPerm->invPermOf(permVal);

        //M[i,j] unknown
        if(ogVal==-1){
            int minog = cycset.bitdomains[0][r].dom.find_first();
            //p^-1 M[p(i),p(j)] known
            //If less than min og, breaking perm found, else skip.
            if(invVal!=-1){
                if(invVal<=minog){
                    extendPerm(copyPerm);
                    vector<int> p = copyPerm->getPerm();
                    addClauses(p,0,r);
                }
            //p^-1 M[p(i),p(j)] unknown
            // if M[p(i),p(j)] known, extend perm s.t. p^-1 M[p(i),p(j)] <= min M[i,j]
            // else skip.
            } else {
                if(permVal!=-1){
                    shared_ptr<pperm_common> invFixed = inverseUnkown(copyPerm,minog,permVal);
                    if(invFixed->invPermOf(permVal)!=-1){
                        extendPerm(invFixed);
                        vector<int> p = invFixed->getPerm();
                        addClauses(p, 0,r);
                    }
                }
            }
        
        //M[i,j] known
        } else {
            //p^-1 M[p(i),p(j)] known
            // check
            if(invVal!=-1){
                if(invVal<ogVal){
                    extendPerm(copyPerm);
                    vector<int> p = copyPerm->getPerm();
                    addClauses(p,0,r);
                } 
                else if (invVal==ogVal)
                    options_prop.push_back(copyPerm);
            //p^-1 M[p(i),p(j)] unknown
            // if M[p(i),p(j)] known, extend perm s.t. p^-1 M[p(i),p(j)] <= M[i,j]
            // else skip.
            } else {
                if(permVal!=-1){
                    shared_ptr<pperm_common> invFixed = inverseUnkown(copyPerm,ogVal,permVal);
                    int inv = invFixed->invPermOf(permVal);
                    if(inv==ogVal){
                        options_prop.push_back(invFixed);
                    } else if(inv!=-1 && inv<ogVal){
                        extendPerm(invFixed);
                        vector<int> p = invFixed->getPerm();
                        addClauses(p,0,r);
                    }
                } else {
                    if(ogVal==problem_size-1){
                        options_prop.push_back(copyPerm);
                    }
                    /* else{
                        for(int opt=0; opt<ogVal;opt++){
                            int permOpt = copyPerm.permOf(opt);
                            if(find(cycset.domains[copyPerm.permOf(0)][copyPerm.permOf(r)].dom.begin(),cycset.domains[copyPerm.permOf(0)][copyPerm.permOf(r)].dom.end(),permOpt)!=cycset.domains[copyPerm.permOf(0)][copyPerm.permOf(r)].dom.end()){
                                //DAS NIE OKE WANT DAN IS MATRIX NIET MINIMAAL GEGEVEN DEZE PERMUTATIE
                            }
                        }
                        printf("--------------------------\n");
                        printf("PROBLEEMGEVALLETJE\n");
                        diag.print();
                        printf("--\n");
                        copyPerm.print();
                        printf("og = %d\n",ogVal);
                        cycset.domains[copyPerm.permOf(0)][copyPerm.permOf(r)].printDomain();
                    } */
                }
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