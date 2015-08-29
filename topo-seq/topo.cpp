#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <math.h>
#include "graph.h"
#include <omp.h>
#include <sys/time.h>

#define nthreads 16 

using namespace std;

double rtclock()
{
  struct timezone Tzp;
  struct timeval Tp;
  int stat;
  stat = gettimeofday (&Tp, &Tzp);
  if (stat != 0) printf("Error return from gettimeofday: %d",stat);
  return(Tp.tv_sec + Tp.tv_usec*1.0e-6);
}

template <class VertexValue, class EdgeValue>
void Topo(graph<VertexValue, EdgeValue>* g) {
  // Initialize vertex value. 
  g->vertex_value = new VertexValue[g->num_vertices];
  int count = 0;
  for (int i = 0; i < g->num_vertices; ++i) {
    g->vertex_value[i] = g->in_degree[i].in_degree;
    if (!g->vertex_value[i]) {
      ++count;
    }
  }

  int steps = 0;
  int new_count = 0;

  omp_set_num_threads(nthreads);
	omp_lock_t *locks = new omp_lock_t[g->num_vertices];
	for(int i = 0; i < g->num_vertices; i++) {
		omp_init_lock(&locks[i]);
  }

  double before_topo = rtclock();
  while (count) {
    count = 0;
#pragma omp parallel for
    for (int i = g->num_vertices - 1; i >= 0; --i) {
      if (g->vertex_value[i] == 0) {
        g->vertex_value[i] = -1;
        for (int j = g->vertices[i]; j < g->vertices[i+1]; ++j) {
				  omp_set_lock(&locks[g->edges[j]]);
          --g->vertex_value[g->edges[j]];
				  omp_unset_lock(&locks[g->edges[j]]);
        }
        g->order[i] = g->vertex_value[i];
      }
    }

    for (int i = 0; i < g->num_vertices - 1; ++i) {
      if (g->vertex_value[i] == 0) {
        ++count;
      }
    }
    
    ++steps;
  }

  double after_topo = rtclock();
  cout << "Topo time: " << (after_topo - before_topo) << endl;

  cout << "Total steps: " << steps << endl;
}

template <class VertexValue, class EdgeValue>
graph<VertexValue, EdgeValue>* ReadGraph(const string filename) {
  ifstream input(filename.c_str()); 
  string line;
  getline(input, line);
  istringstream ss(line);
  int num_vertices;
  ss >> num_vertices;
  int line_no = 0;

  graph<VertexValue, EdgeValue>* g =
      new graph<VertexValue, EdgeValue>(num_vertices, num_vertices);

  while (getline(input, line)) {
    g->vertices[line_no] = g->edges.size();
    istringstream ss(line);
    int edge;
    while (ss >> edge) {
      g->edges.push_back(edge); 
      ++g->in_degree[edge].in_degree;
    }
    ++line_no;
  }

  g->num_edges = g->edges.size();
  g->vertices.push_back(g->num_edges);

  return g;
}

int main() {
  graph<float, float>* g = ReadGraph<float, float>("../input/dag/dag-38394-standard");
  // graph<float, float>* g = ReadGraph<float, float>("/work/02687/binren/surabhi/code/topo/input/dag-38394");
  cout << "Read done." << endl;
  cout << "Num edges: " << g->edges.size() << endl;
  Topo(g);
  delete g;
  return 0;
}
