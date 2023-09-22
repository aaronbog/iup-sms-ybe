#include "clause.h"
#include "useful.h"
#include <math.h>

void encodeEntries(cnf_t *cnf)
{
    for(int i=0; i<size; i++)
        for(int j=0; j<size; j++)
            exactlyOne(cnf, cycset_lits[i][j]);

    for(int i=0; i<size; i++)
        for(int k=0; k<size; k++)
        {
            clause_t cl;
            for(int j=0; j<size; j++)
                cl.push_back(cycset_lits[i][j][k]);
            cnf->push_back(cl);
            cl.clear();
        }
    
    for(int i=0; i<size; i++)
        {
            clause_t cl;
            for(int j=0; j<size; j++)
                cl.push_back(cycset_lits[j][j][i]);
            cnf->push_back(cl);
            cl.clear();
        }
    
}

void exactlyOne(cnf_t *cnf, vector<int> eo)
{
    if(eo.size()<=6)
    {
        atMostOne(cnf, eo);
        atLeastOne(cnf, eo);
    }
    else
    {
        auto p=commanderEncoding(eo);
        for(auto cl : p.second)
            cnf->push_back(cl);
        atLeastOne(cnf, eo);
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

pair<int,cnf_t> commanderEncoding(vector<int> amo)
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

        auto p1 = commanderEncoding(vector<int>(amo.begin(), amo.begin()+p));
        auto p2 = commanderEncoding(vector<int>(amo.begin()+p, amo.begin()+2*p));
        auto p3 = commanderEncoding(vector<int>(amo.begin()+2*p, amo.end()));
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

    int cmd = nextFreeVariable++;
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

void YBEClauses(cnf_t *cnf)
{
    int t = 0;
    for (int i=0; i<size; i++)
        for (int j=i+1; j<size; j++)
            for (int k=0; k<size; k++)
            {
                exactlyOne(cnf, ybe_left_lits[t]);
                exactlyOne(cnf, ybe_right_lits[t]);
                exactlyOne(cnf, ybe_lits[t]);
                clause_t cl;
                for(int a = 0; a<size; a++)
                    for(int b=0; b<size; b++)
                    {
                        cl.push_back(-cycset_lits[i][j][a]);
                        cl.push_back(-cycset_lits[i][k][b]);
                        if(size*a+b<size*size)
                            cl.push_back(ybe_left_lits[t][size*a+b]);
                        cnf->push_back(cl);
                        cl.clear();

                        cl.push_back(-cycset_lits[j][i][a]);
                        cl.push_back(-cycset_lits[j][k][b]);
                        if(size*a+b<size*size)
                            cl.push_back(ybe_right_lits[t][size*a+b]);
                        cnf->push_back(cl);
                        cl.clear();
                    }

                for(int a = 0; a<size*size; a++)
                    for(int b=0; b<size; b++)
                    {
                        cl.push_back(-ybe_left_lits[t][a]);
                        cl.push_back(-cycset_lits[floor(a/size)][a%size][b]);
                        cl.push_back(ybe_lits[t][b]);
                        cnf->push_back(cl);
                        cl.clear();

                        cl.push_back(-ybe_right_lits[t][a]);
                        cl.push_back(-cycset_lits[floor(a/size)][a%size][b]);
                        cl.push_back(ybe_lits[t][b]);
                        cnf->push_back(cl);
                        cl.clear();
                    }
                
                t+=1;
            }
}