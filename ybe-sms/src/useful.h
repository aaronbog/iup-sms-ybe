#ifndef USEFUL_H
#define USEFUL_H

#include <algorithm>
#include <utility>
#include <vector>
#include <iostream>
#include <numeric>
#include <string>
#include <string.h>
#include <cassert>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <chrono>
#include <boost/dynamic_bitset.hpp>

using namespace std;
using namespace chrono;

typedef int lit_t;
typedef vector<lit_t> clause_t;
typedef vector<clause_t> cnf_t;

void printCnf(cnf_t *cnf);

#define PRINT_CURRENT_LINE                            \
    printf("Line %d, file %s\n", __LINE__, __FILE__); \
    fflush(stdout);

#define EXIT_UNWANTED_STATE                                                          \
    {                                                                                \
        printf("Error: unexpected state at line %d, file %s\n", __LINE__, __FILE__); \
        exit(EXIT_FAILURE);                                                          \
    }

typedef enum {
    True_t=1,
    False_t=0,
    Unknown_t=-1
} truth_vals;

/* typedef struct domain_t{
    std::vector<int> dom;
    
    domain_t(int problem_size){
        dom=vector<int>(problem_size, -1);
        iota(dom.begin(),dom.end(),0);
    }
    
    void add_value(int n){
        if(find(dom.begin(),dom.end(),n)==dom.end()){
            dom.emplace_back(n);
        }
    }

    void delete_value(int n){
        auto it = find(dom.begin(),dom.end(),n);
        if(it!=dom.end()){
            dom.erase(it);
        }
    }

    bool is_empty(){
        return dom.size()==0;
    }

    void printDomain(){
        printf("{");
        for(int i : dom){
            printf("%d",i);
        }
        printf("}");
    }
} domain_t; */

typedef struct bitdomain_t{
    boost::dynamic_bitset<> dom;

    bitdomain_t(int problem_size){
        dom.resize(problem_size);
        dom.set();
    }

    void add_value(int n){
        dom[n]=1;
    }

    void set_value(int n){
        dom.reset();
        dom[n]=1;
    }

    void delete_value(int n){
        dom[n]=0;
    }

    bool is_empty(){
        return dom.none();
    }

    vector<int> options(){
        if(is_empty()){
            return vector<int>();
        } else {
            vector<int> opts;
            for (boost::dynamic_bitset<>::size_type i = 0; i < dom.size(); ++i)
            {
                if(dom[i])
                    opts.push_back(i);
            }
            return opts;
        }
    }

    void printDomain(){
        printf("{");
        for (boost::dynamic_bitset<>::size_type i = 0; i < dom.size(); ++i)
        {
            if(dom[i])
                printf("%d",i);
        }
        printf("}");
    }
} bitdomain_t;

typedef struct cycle_set_t{
    std::vector<vector<int>> matrix;
    std::vector<vector<vector<truth_vals>>> assignments;
    std::vector<vector<vector<int>>> cycset_lits;
    //std::vector<vector<domain_t>> domains;
    std::vector<vector<bitdomain_t>> bitdomains;

    cycle_set_t(int problem_size, std::vector<vector<vector<int>>> lits){
        cycset_lits=lits;
        assignments=vector<vector<vector<truth_vals>>>(problem_size, vector<vector<truth_vals>>(problem_size, vector<truth_vals>(problem_size, Unknown_t)));
        matrix=vector<vector<int>>(problem_size, vector<int>(problem_size, -1));
        //domains=vector<vector<domain_t>>(problem_size,vector<domain_t>(problem_size,domain_t(problem_size)));
        bitdomains=vector<vector<bitdomain_t>>(problem_size,vector<bitdomain_t>(problem_size,bitdomain_t(problem_size)));
    }

    cycle_set_t() = default;
} cycle_set_t;

/* void rotate_matrix_cols(std::vector<vector<int>> &og_mat, vector<int> perm);
void rotate_matrix_rows(std::vector<vector<int>> &og_mat, vector<int> perm);
void swap_matrix_cols(std::vector<vector<int>> &og_mat, int i, int j);
void swap_matrix_rows(std::vector<vector<int>> &og_mat, int i, int j);
void apply_perm(std::vector<vector<int>> &og_mat, std::vector<vector<int>> perm, std::vector<int> invperm); */

/* typedef struct {
    std::vector<int> perm;
    std::vector<int> inverse;
    std::vector<vector<int>> cycPerm;

    void extendPerm(int i, int j);
    void extendInvPerm(int i, int j);
    void permToCyclePerm();
} perm_t; */

typedef struct partialPerm_t{
    std::vector<int> element;
    std::vector<bool> part;

    partialPerm_t(std::vector<int> perm);
    partialPerm_t();
    partialPerm_t copyPerm();
    int permOf(int p);
    vector<int> options(int p);
    int invPermOf(int p);
    bool fixed(int p);
    bool fix(int p, int pp);
    void print();
    bool fullDefined();
} partialPerm_t;


typedef struct cyclePerm_t{
    std::vector<int> element;
    std::vector<int> part;

    cyclePerm_t(std::vector<int> perm);
    cyclePerm_t();
    int permOf(int p);
    int invPermOf(int p);
    vector<int> cycle(int el);
    void print();
} cyclePerm_t;

//perm_t newPerm();

vector<vector<int>> permToCyclePerm(vector<int> &perm);
void printCycleSet(const cycle_set_t &cycset);
void fprintCycleSet(FILE *stream, const cycle_set_t &cycset);
void printPartiallyDefinedCycleSet(const cycle_set_t &cycset);
void printDomains(const cycle_set_t &cycset);
void printAssignments(const cycle_set_t &cycset);
void part(int n, vector<int>& v, int level, vector<vector<int>>& parts);
void makeDiagonals(vector<vector<int>>& parts, vector<vector<int>>& permutations);
void cycleToParts(vector<vector<int>> &perm, vector<int> &ord, vector<bool> &part);
#endif