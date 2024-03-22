#include "clause.h"
#include "useful.h"
#include <math.h>

void encodeEntries(cnf_t *cnf, vector<int> d, int &nextFree, vector<vector<vector<lit_t>>> &cycset_lits)
{   for (int i = 0; i < problem_size; i++)
        for (int j = 0; j < problem_size; j++)
            for (int k = 0; k < problem_size; k++)
                {if(i!=j && (!smallerEncoding||k!=d[i]))
                    cycset_lits[i][j][k] = nextFree++;}
    
    for(int i=0; i<problem_size; i++)
        for(int j=0; j<problem_size; j++)
            {if(i!=j)
                exactlyOne(cnf, cycset_lits[i][j], nextFree);}

    for(int i=0; i<problem_size; i++)
        for(int k=0; k<problem_size; k++)
        {
            if(d[i]==k)
                continue;

            vector<int> to_encode;
            //clause_t cl;
            for(int j=0; j<problem_size; j++)
                {if(j!=i)
                    to_encode.push_back(cycset_lits[i][j][k]);
                }
            exactlyOne(cnf,to_encode,nextFree);
            /* cnf->push_back(cl);
            cl.clear(); */
        }
}

void encodeOrder(cnf_t *cnf, vector<int> d, int &nextFree, vector<vector<vector<lit_t>>> &cycset_lits_ord, vector<vector<vector<lit_t>>> &cycset_lits)
{   for (int i = 0; i < problem_size; i++)
        for (int j = 0; j < problem_size; j++)
        {
            int prev=-1;
            for (int k = 0; k < problem_size-1; k++)
            {
                if(i!=j && k!=d[i])
                {
                    cycset_lits_ord[i][j][k] = nextFree++;
                    if(prev!=-1){
                        clause_t cl;
                        cl.push_back(-cycset_lits_ord[i][j][prev]);
                        cl.push_back(cycset_lits_ord[i][j][k]);
                        cnf->push_back(cl);
                        prev=k;
                    }
                    clause_t cll;
                    for(int l=0; l<=k; l++){
                        if(l!=d[i]){
                            clause_t cl = vector<int>{-cycset_lits[i][j][l],cycset_lits_ord[i][j][k]};
                            cll.push_back(cycset_lits[i][j][l]);
                            cnf->push_back(cl);
                        }
                    }
                    cll.push_back(-cycset_lits_ord[i][j][k]);
                    
                    cnf->push_back(cll);
                    if(prev==-1){
                        prev=k;
                    }
                }
            }
        }
}

void encodeEntries(cnf_t *cnf, int &nextFree, vector<vector<vector<lit_t>>> &cycset_lits)
{
    for(int i=0; i<problem_size; i++)
        for(int j=0; j<problem_size; j++)
            exactlyOne(cnf, cycset_lits[i][j], nextFree);

    for(int i=0; i<problem_size; i++)
        for(int k=0; k<problem_size; k++)
        {
            clause_t cl;
            for(int j=0; j<problem_size; j++)
                cl.push_back(cycset_lits[i][j][k]);
            cnf->push_back(cl);
            cl.clear();
        }
    
    for(int i=0; i<problem_size; i++)
        {
            clause_t cl;
            for(int j=0; j<problem_size; j++)
                cl.push_back(cycset_lits[j][j][i]);
            cnf->push_back(cl);
            cl.clear();
        }
}

void exactlyOne(cnf_t *cnf, vector<int> eo, int &nextFree)
{
    vector<int>toEO=vector<int>();
    for(int i : eo){
        if(i!=0)
            toEO.push_back(i);
    }
    if(toEO.size()<=6)
    {
        atMostOne(cnf, toEO);
        atLeastOne(cnf, toEO);
    }
    else
    {
        auto p=commanderEncoding(toEO, nextFree);
        for(auto cl : p.second)
            cnf->push_back(cl);
        atLeastOne(cnf, toEO);
    }
}

void atMostOne(cnf_t *cnf, vector<int> alo)
{
    clause_t cl;
    for(std::size_t i=0, max=alo.size(); i<max; i++)
        for(std::size_t j=i+1; j<max; j++)
        {
            cl.push_back(-alo[i]);
            cl.push_back(-alo[j]);
            cnf->push_back(cl);
            cl.clear();
        }
}

void atLeastOne(cnf_t *cnf, vector<int> amo)
{
    clause_t cl;
    for(std::size_t i = 0, max=amo.size(); i<max; i++)
        cl.push_back(amo[i]);
    cnf->push_back(cl);
}

pair<int,cnf_t> commanderEncoding(vector<int> amo, int &nextFree)
{
    int a,b,c;
    cnf_t clauses;
    if(amo.size()==0)
        return make_pair(0, cnf_t());
    if(amo.size()==1)
        return make_pair(amo[0], cnf_t());
    if(amo.size()==2)
    {
        a = amo[0];
        b = amo[1];
        c = 0;
        clauses = cnf_t();
    }
    if(amo.size()==3)
    {
        a = amo[0];
        b = amo[1];
        c = amo[2];
        clauses = cnf_t();
    }
    if(amo.size()>3)
    {
        int p=ceil(amo.size()/3);

        auto p1 = commanderEncoding(vector<int>(amo.begin(), amo.begin()+p), nextFree);
        auto p2 = commanderEncoding(vector<int>(amo.begin()+p, amo.begin()+2*p),nextFree);
        auto p3 = commanderEncoding(vector<int>(amo.begin()+2*p, amo.end()), nextFree);
        a = p1.first;
        b = p2.first;
        c = p3.first;

        for(auto cl : p1.second)
            clauses.push_back(cl);
        for(auto cl : p2.second)
            clauses.push_back(cl);
        for(auto cl : p3.second)
            clauses.push_back(cl);
    }

    int cmd = nextFree++;
    clause_t cl;
    cl.push_back(-a);
    cl.push_back(cmd);
    clauses.push_back(cl);
    cl.clear();

    cl.push_back(-b);
    cl.push_back(cmd);
    clauses.push_back(cl);
    cl.clear();

    cl.push_back(a);
    cl.push_back(b);
    if(c!=0)
    {
        cl.push_back(c);
        cl.push_back(-cmd);
        clauses.push_back(cl);
        cl.clear();

        cl.push_back(-c);
        cl.push_back(cmd);
        clauses.push_back(cl);
        cl.clear();
    }
    else
    {
        cl.push_back(-cmd);
        clauses.push_back(cl);
        cl.clear();
    }

    cl.push_back(-a);
    cl.push_back(-b);
    clauses.push_back(cl);
    cl.clear();

    if(c!=0)
    {
        cl.push_back(-a);
        cl.push_back(-c);
        clauses.push_back(cl);
        cl.clear();

        cl.push_back(-b);
        cl.push_back(-c);
        clauses.push_back(cl);
        cl.clear();
    }

    return make_pair(cmd,clauses);
}

void YBEClauses(cnf_t *cnf, vector<int> d, int &nextFree, vector<vector<vector<lit_t>>> &cycset_lits, vector<vector<lit_t>> &ybe_left_lits, vector<vector<lit_t>> &ybe_right_lits, vector<vector<lit_t>> &ybe_lits)
{
    int t = 0;
    clause_t cl;

    for (int i=0; i<problem_size; i++)
        for (int j=i+1; j<problem_size; j++)
            for (int k=0; k<problem_size; k++)
            {
                for(int a = 0; a<problem_size; a++)
                    for(int b=0; b<problem_size; b++)
                    {
                        if((!smallerEncoding  && (i!=k || d[i]==b)) || (smallerEncoding && (a!=d[i] && ((i!=k&&d[i]!=b)|| (i==k&&d[i]==b))))){
                            if(i!=k){
                                cl.push_back(-cycset_lits[i][k][b]);
                            }
                            cl.push_back(-cycset_lits[i][j][a]);
                            if(problem_size*a+b<problem_size*problem_size){
                                if(ybe_left_lits[t][problem_size*a+b]==0)
                                    ybe_left_lits[t][problem_size*a+b]=nextFree++;
                                cl.push_back(ybe_left_lits[t][problem_size*a+b]);
                            }
                            cnf->push_back(cl);
                            cl.clear();
                        }

                        if((!smallerEncoding && (j!=k || d[j]==b)) || (smallerEncoding && (a!=d[j] && ((j!=k&&d[j]!=b)|| (j==k&&d[j]==b))))){
                            if(j!=k){
                                cl.push_back(-cycset_lits[j][k][b]);
                            }
                            cl.push_back(-cycset_lits[j][i][a]);
                            if(problem_size*a+b<problem_size*problem_size){
                                if(ybe_right_lits[t][problem_size*a+b]==0)
                                    ybe_right_lits[t][problem_size*a+b]=nextFree++;
                                cl.push_back(ybe_right_lits[t][problem_size*a+b]);
                            }
                            cnf->push_back(cl);
                            cl.clear();
                        }
                    }

                vector<int> litsCopy;
                copy(ybe_left_lits[t].begin(), ybe_left_lits[t].end(),back_inserter(litsCopy));
                litsCopy.erase(remove(litsCopy.begin(),litsCopy.end(),0),litsCopy.end());
                litsCopy.shrink_to_fit();
                exactlyOne(cnf,litsCopy,nextFree);
                
                litsCopy.clear();
                copy(ybe_right_lits[t].begin(), ybe_right_lits[t].end(),back_inserter(litsCopy));
                litsCopy.erase(remove(litsCopy.begin(),litsCopy.end(),0),litsCopy.end());
                litsCopy.shrink_to_fit();
                exactlyOne(cnf,litsCopy,nextFree);
                t+=1;
            }

    t = 0;
    for (int i=0; i<problem_size; i++)
        for (int j=i+1; j<problem_size; j++)
            for (int k=0; k<problem_size; k++)
            {
                for(int a = 0; a<problem_size*problem_size; a++)
                    for(int b=0; b<problem_size; b++){
                        if(ybe_left_lits[t][a]!=0)
                        {
                            int r = floor(a/problem_size);
                            int c = a%problem_size;
                            if((!smallerEncoding && (r!=c || d[r]==b)) || (smallerEncoding && ((r!=c&&b!=d[r])|| (r==c&&d[r]==b)))){
                                if(r!=c)
                                    cl.push_back(-cycset_lits[r][c][b]);
                                cl.push_back(-ybe_left_lits[t][a]);
                                if(ybe_lits[t][b]==0)
                                    ybe_lits[t][b]=nextFree++;
                                cl.push_back(ybe_lits[t][b]);
                                cnf->push_back(cl);
                                cl.clear();
                            }
                        }

                        if(ybe_right_lits[t][a]!=0)
                        {
                            int r = floor(a/problem_size);
                            int c = a%problem_size;
                            if((!smallerEncoding && (r!=c || d[r]==b)) || (smallerEncoding && ((r!=c&&b!=d[r])|| (r==c&&d[r]==b)))){
                                if(r!=c)
                                    cl.push_back(-cycset_lits[r][c][b]);
                                cl.push_back(-ybe_right_lits[t][a]);
                                if(ybe_lits[t][b]==0)
                                    ybe_lits[t][b]=nextFree++;
                                cl.push_back(ybe_lits[t][b]);
                                cnf->push_back(cl);
                                cl.clear();
                            }
                        }
                    }
                
                vector<int> litsCopy;
                copy(ybe_lits[t].begin(), ybe_lits[t].end(),back_inserter(litsCopy));
                litsCopy.erase(remove(litsCopy.begin(),litsCopy.end(),0),litsCopy.end());
                litsCopy.shrink_to_fit();
                exactlyOne(cnf,litsCopy,nextFree);
                t+=1;
            }
}

void YBEClauses(cnf_t *cnf, int &nextFree, vector<vector<vector<lit_t>>> &cycset_lits, vector<vector<lit_t>> &ybe_left_lits, vector<vector<lit_t>> &ybe_right_lits, vector<vector<lit_t>> &ybe_lits)
{
    int t = 0;
    for (int i=0; i<problem_size; i++)
        for (int j=i+1; j<problem_size; j++)
            for (int k=0; k<problem_size; k++)
            {
                exactlyOne(cnf, ybe_left_lits[t],nextFree);
                exactlyOne(cnf, ybe_right_lits[t],nextFree);
                exactlyOne(cnf, ybe_lits[t],nextFree);
                clause_t cl;
                for(int a = 0; a<problem_size; a++)
                    for(int b=0; b<problem_size; b++)
                    {
                        cl.push_back(-cycset_lits[i][j][a]);
                        cl.push_back(-cycset_lits[i][k][b]);
                        if(problem_size*a+b<problem_size*problem_size)
                            cl.push_back(ybe_left_lits[t][problem_size*a+b]);
                        cnf->push_back(cl);
                        cl.clear();

                        cl.push_back(-cycset_lits[j][i][a]);
                        cl.push_back(-cycset_lits[j][k][b]);
                        if(problem_size*a+b<problem_size*problem_size)
                            cl.push_back(ybe_right_lits[t][problem_size*a+b]);
                        cnf->push_back(cl);
                        cl.clear();
                    }

                for(int a = 0; a<problem_size*problem_size; a++)
                    for(int b=0; b<problem_size; b++)
                    {
                        cl.push_back(-ybe_left_lits[t][a]);
                        cl.push_back(-cycset_lits[floor(a/problem_size)][a%problem_size][b]);
                        cl.push_back(ybe_lits[t][b]);
                        cnf->push_back(cl);
                        cl.clear();

                        cl.push_back(-ybe_right_lits[t][a]);
                        cl.push_back(-cycset_lits[floor(a/problem_size)][a%problem_size][b]);
                        cl.push_back(ybe_lits[t][b]);
                        cnf->push_back(cl);
                        cl.clear();
                    }
                
                t+=1;
            }
}