#include "useful.h"
#include "global.h"

void printCycleSet(const cycle_set_t &cycset)
{
    for (auto row : cycset.matrix)
    {
      for (auto value : row)
        printf("%d ", value);
      printf("\n");
    }
}

void fprintCycleSet(FILE *stream, const cycle_set_t &cycset)
{
    for (auto row : cycset.matrix)
    {
      for (auto value : row)
        fprintf(stream, "%d ", value);
      fprintf(stream, "\n");
    }
}

void printPartiallyDefinedCycleSet(const cycle_set_t &cycset)
{
    for (auto row : cycset.matrix)
    {
      for (auto value : row)
      {
        if (value<0)
        {
          printf("* ");
        } 
        else
        {
          printf("%d ", value);
        }
      }
      printf("\n");
    }
}

void printDomains(const cycle_set_t &cycset)
{
    for (auto row : cycset.domains)
    {
      for (auto dom : row)
      {
        dom.printDomain();
        printf("\t");
      }
      printf("\n");
    }
}

void printAssignments(const cycle_set_t &cycset)
{
    for (int i=0; i<problem_size; i++)
    {
      printf("ROW %d: ",i);
      for (int j=0; j<problem_size; j++)
      {
        for (int k=0; k<problem_size; k++){
          if(cycset.assignments[i][j][k]==True_t){
            printf("1 ");
          } else if (cycset.assignments[i][j][k]==False_t){
            printf("0 ");
          } else {
            printf("* ");
          }
        }
        printf("\t");
      }
      printf("\n");
    }
}

void printCnf(cnf_t *cnf)
{
  for(auto cl : *cnf)
  {
    for(auto lit: cl)
      printf("%d ", lit);
    printf("\n\n");
  }
}

/* https://stackoverflow.com/questions/22988674/partition-of-an-integer-number-of-partitions */
void part(int n, vector<int>& v, int level, vector<vector<int>>& parts){
    if(n<1)
        return ;
    v[level]=n;
    vector<int> p;
    for(int i=0; i<=level; i++)
      if(v[i]!=1)
        p.push_back(v[i]);
    if(p.size() != 0)
      parts.push_back(p);
    
    int first=(level==0) ? 1 : v[level-1];

    for(int i=first;i<=n/2;i++){
        v[level]=i;
        part(n-i, v, level+1, parts);
    }
}

void makeDiagonals(vector<vector<int>>& parts, vector<vector<int>>& permutations)
{
  vector<vector<vector<int>>> cycles;
  for(size_t i = 0; i<parts.size(); i++)
  {
    vector<int> part;
    if(parts[i].size()==1)
    {
      for(int j=0; j<parts[i][0]; j++)
        part.push_back(j);
      cycles.push_back(vector<vector<int>>{part});
      part.clear();
    }
    else
    {
      int toAdd=0;
      vector<vector<int>> cycs;
      for(size_t j=0; j<parts[i].size();j++)
      {
        for(int k=0; k<parts[i][j]; k++)
        {
          part.push_back(k+toAdd);
        }
        cycs.push_back(part);
        part.clear();
        toAdd+=parts[i][j];
      }
      cycles.push_back(cycs);
      cycs.clear();
    }
  }

  vector<int> perm=vector<int>();
  for(auto part : cycles)
  {
    for(int j=0; j<problem_size; j++)
      perm.push_back(j);
    for(auto p : part)
    {
      for(size_t i = 0; i<p.size()-1; i++)
        perm[p[i]]=p[i+1];
      perm[p[p.size()-1]]=p[0];
    }
    permutations.push_back(perm);
    perm.clear();
  }
}

perm_t newPerm(){
  perm_t newPerm;
  newPerm.perm=vector<int>(problem_size,-1);
  newPerm.inverse=vector<int>(problem_size,-1);
  return newPerm;
}

void extendPerm(perm_t perm, int i, int j){
  perm.perm[i]=j;
  perm.inverse[j]=i;
}

void extendInvPerm(perm_t perm, int i, int j){
  perm.inverse[i]=j;
  perm.perm[j]=i;
}

vector<vector<int>> permToCyclePerm(vector<int> &perm){
  vector<vector<int>> cycles;
  vector<int> all = vector<int>(problem_size,-1);
  iota(all.begin(),all.end(),0);
  
  while(all.size()!=0){
    int i = *min_element(all.begin(),all.end());
    vector<int> cycle=vector<int>();
    while (find(cycle.begin(),cycle.end(),i)==cycle.end()){
      cycle.push_back(i);
      all.erase(find(all.begin(),all.end(),i));
      i=perm[i];
    }
    cycles.push_back(cycle);
  }
  return cycles;
}

void swap_matrix_cols(std::vector<vector<int>> &og_mat, int i, int j){
  for(int r=0; r<problem_size; r++){
    int swp = og_mat[r][i];
    og_mat[r][i]=og_mat[r][j];
    og_mat[r][j]=swp;
  }
}
void swap_matrix_rows(std::vector<vector<int>> &og_mat, int i, int j){
  og_mat[i].swap(og_mat[j]);
}

void rotate_matrix_rows(std::vector<vector<int>> &og_mat, vector<int> cycPerm){
  vector<int> swap = vector<int>();
  copy(og_mat[cycPerm[0]].begin(),og_mat[cycPerm[0]].end(),back_inserter(swap));
  int prev = 0;
  int max = cycPerm.size();
  for(int i=1; i<max;i++){
    og_mat[cycPerm[prev]]=move(og_mat[cycPerm[i]]);
    prev=i;
  }
  og_mat[cycPerm[prev]]=move(swap);
}

void rotate_matrix_cols(std::vector<vector<int>> &og_mat, std::vector<int> cycPerm){
  for(int r=0; r<problem_size; r++){
      int prev = 0;
      int swp = og_mat[r][cycPerm[0]];
      int max = cycPerm.size();
      for(int i=1; i<max;i++){
        og_mat[r][cycPerm[prev]]=og_mat[r][cycPerm[i]];
        prev=i;
      }
      og_mat[r][cycPerm[prev]]=swp;
  }
}

void apply_perm(std::vector<vector<int>> &og_mat, std::vector<vector<int>> perm, vector<int> invperm){
  for(auto cyc : perm){
    if(cyc.size()==2){
      swap_matrix_cols(og_mat,cyc[0],cyc[1]);
      swap_matrix_rows(og_mat,cyc[0],cyc[1]);
    } else if (cyc.size()>2){
      rotate_matrix_cols(og_mat,cyc);
      rotate_matrix_rows(og_mat,cyc);
    }
  }
  for(int i=0; i<problem_size; i++){
    for(int j=0; j<problem_size; j++){
      if(og_mat[i][j]!=-1)
        og_mat[i][j]=invperm[og_mat[i][j]];
    }
  }
}