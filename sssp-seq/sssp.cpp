#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <math.h>
#include <omp.h>
#include <sys/time.h>

#define nthreads 180 

using namespace std;
#include "graph.h"

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
void Sssp(graph<VertexValue, EdgeValue>* g) {
  omp_set_num_threads(nthreads);
	omp_lock_t *locks = new omp_lock_t[g->num_vertices];
	for(int i = 0; i < g->num_vertices; i++) {
		omp_init_lock(&locks[i]);
  }

  int count = 1;

  double before_sssp = rtclock();
  while (count) {
    cout << count << endl;
    count = 0;
#pragma omp parallel for schedule(dynamic, 50)
    for (int j = g->num_vertices - 1; j >= 0; --j) {
      if (g->updated[j] == 1) {
      for (int k = g->vertices[j]; k < g->vertices[j+1]; ++k) {
          if (g->vertex_value[j] + g->edge_value[k] <
              g->vertex_value[g->edges[k]]) {
				    omp_set_lock(&locks[g->edges[k]]);
            g->vertex_value[g->edges[k]] = g->vertex_value[j] + g->edge_value[k];
            g->updated[g->edges[k]] = 1;
				    omp_unset_lock(&locks[g->edges[k]]);
          }
        }
      }
      g->updated[j] = 0;
    }

    // count total.
    for (int i = 0; i < g->num_vertices; ++i) {
      count+=g->updated[i];
    }
  } 

  double after_sssp = rtclock();
  cout << "Sssp time: " << (after_sssp - before_sssp) << endl;
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
	string filename("../input/soc-metis.txt");
  graph<float, float>* g = ReadGraph<float, float>(filename);
  cout << "Read done." << endl;
  g->vertex_value = new float[g->num_vertices];
  // Set vertex value.
  for (int i = 0; i < g->num_vertices; ++i) {
    g->vertex_value[i] = 999999;
  }
  g->vertex_value[0] = 0;
  g->updated[0] = 1;
  cout << "Set vertex value done." << endl;
  // Set edge value.
  g->edge_value = new float[g->num_edges];
  for (int i = 0; i < g->num_edges; ++i) {
    g->edge_value[i] = 1;
  }
  cout << "Set edge value done." << endl;
  Sssp(g);
  delete g;
}
