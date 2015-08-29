#include<iostream>
#include<fstream>
#include<sstream>
#include<string.h>
#include<malloc.h>
#include <mutex>
#include<stdlib.h>
#include<string>
#include<sys/time.h>
#include<math.h>
#include<omp.h>
#include <vector>
// #include "atomic.h"
#define INT_MAX 88888888
//#define NUM_VERTICES 2000000
#define NUM_VERTICES 1632803

using namespace std;
double rtclock();

int main()
{
  string line;
  string first;
  //int index=0;
  int *vertices=new int[NUM_VERTICES + 1];
  for (int i = 0; i < NUM_VERTICES; ++i) {
    vertices[i] = -1;
  }
  //vector<int> vertices(NUM_VERTICES, -1);
  int *edges=new int[37000005];
  // vector<int> edges;
  int *level_seq=new int[NUM_VERTICES];
  int *level_par=new int[NUM_VERTICES];
  int *visited= new int[NUM_VERTICES];
  int *h_graph_mask=new int[NUM_VERTICES];
  int *h_updating_graph_mask=new int[NUM_VERTICES];
  int edge,i,j;
  int num_edges, num_vertices, num_act_vertices;
  double s_time,e_time;

  //ifstream input("../bfs/input/graph-2000000");
  ifstream input("../bfs/input/soc-out.txt");

  int edge_index = 0;
  while(getline(input,line))
  {
    //vertices[line_no]=index;
    istringstream ss(line);
    ss>>first;
    int vid=atoi(first.substr(0,first.size()-1).c_str())-1;
    vertices[vid] = edge_index;
    while(ss>>edge)
    {
      edges[edge_index] = edge - 1;
      ++edge_index;
    }
  }

  num_vertices = NUM_VERTICES;
  vertices[NUM_VERTICES] = edge_index;
  num_edges = edge_index;

  // Condense.
  for (int i = num_vertices - 1; i >= 0; --i) {
    if (vertices[i] == -1) {
      vertices[i] = vertices[i+1];
    }
  }

  for(i=0;i<num_vertices;i++)
  {
    level_seq[i]=INT_MAX;
    level_par[i]=INT_MAX;
    h_graph_mask[i]=0;
    h_updating_graph_mask[i]=0;
    visited[i]=0;
  }
  
  level_seq[0]=0;
  level_par[0]=0;
  h_graph_mask[0]=1;
  visited[0]=1;
  num_act_vertices=1;
  int flag;
  s_time=rtclock();

  int num_iter = 0;
  int active_count = 0;
  do
  {
    flag=0;
    for(int i=0;i<num_vertices;i++)
    {
      if(h_graph_mask[i]==1)
      {
        h_graph_mask[i]=0;
        for(int j=vertices[i]; j<vertices[i+1]; j++)
        {
          if(visited[edges[j]]==0 && level_seq[i]+1<=level_seq[edges[j]])
          {
            level_seq[edges[j]]=level_seq[i]+1;
            h_updating_graph_mask[edges[j]]=1;
          }
        }
      }
    }

    for(int i=0;i<num_vertices;i++)
    {
      if(h_updating_graph_mask[i]==1)
      {
        h_graph_mask[i]=1;
        visited[i]=1;
        flag=1;
        h_updating_graph_mask[i]=0;
      }
    }
    ++num_iter;
  } while(flag==1);

  e_time=rtclock();

  cout << "num of iter: " << num_iter << endl;
  cout<<"Sequential time is "<<e_time-s_time<<endl;

  for(int i=0;i<num_vertices;i++)
  {
    level_par[i]=INT_MAX;
    h_graph_mask[i]=0;
    h_updating_graph_mask[i]=0;
    visited[i]=0;
  }

  level_par[0]=0; 
  h_graph_mask[0]=1;
  visited[0]=1;

  omp_lock_t* locks = new omp_lock_t[num_vertices];
  volatile int* ilocks = new volatile int[num_vertices];
  std::mutex* mutices = new std::mutex[num_vertices];

  for (int i = 0; i < num_vertices; ++i) {
    omp_init_lock(&locks[i]);
    ilocks[i] = 0;
  }

  s_time=rtclock();
  omp_set_num_threads(16);
  int num_iter_par = 0;
  
  do
  {
    flag=0;
#pragma omp parallel
    {
#pragma omp for
      for(i=0;i<num_vertices;i++)
      {
        if(h_graph_mask[i]==1)
        {
          h_graph_mask[i]=0;
          for(j=vertices[i];j<vertices[i+1];j++)
          {
            int neighbor = edges[j];
            if(visited[neighbor]==0)
            {	 
              //omp_set_lock(&locks[neighbor]);
              //mutices[neighbor].lock();
              level_par[neighbor]=level_par[i]+1;
              h_updating_graph_mask[neighbor]=1;
              //mutices[neighbor].unlock();
              //omp_unset_lock(&locks[neighbor]);
            }
          }
        }
      }

#pragma omp for 
      for(i=0;i<num_vertices;i++)
      {
        if(h_updating_graph_mask[i]==1)
        {
          h_graph_mask[i]=1;
          visited[i]=1;
          flag=1;
          h_updating_graph_mask[i]=0;	
        }
      }
    }
    ++num_iter_par;
  }
  while(flag==1);

  e_time=rtclock();
  for (int i = 0; i < num_vertices; ++i) {
    omp_destroy_lock(&locks[i]);
  }
  delete[] locks;
  delete[] ilocks;
  delete[] mutices;

  int diff=0;
  for(i=0;i<num_vertices;i++)
  {
    if(level_par[i]!=level_seq[i])
      diff++;
  }

  cout << "Num of iter par: " << num_iter_par << endl;
  cout<<"NUMBER OF VERTICES :"<<num_vertices<<"\n";
  cout<<"Parallel TIME TAKEN :"<<e_time-s_time<<"\n";
  if(diff==0)
    cout<<"No differences\n";
  else
    cout<< diff << " differences found\n";
  return 0;
}

double rtclock()
{
  struct timezone Tzp;
  struct timeval Tp;
  int stat;
  stat = gettimeofday (&Tp, &Tzp);
  if (stat != 0) printf("Error return from gettimeofday: %d",stat);
  return(Tp.tv_sec + Tp.tv_usec*1.0e-6);
}


