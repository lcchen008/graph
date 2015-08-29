#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <math.h>
#include <omp.h>
#include <sys/time.h>
using namespace std;
#include "graph.h"
#define nthreads 16 

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
void Bfs(graph<VertexValue, EdgeValue>* g) {
  omp_set_num_threads(nthreads);
	omp_lock_t *locks = new omp_lock_t[g->num_vertices];
	for(int i = 0; i < g->num_vertices; i++) {
		omp_init_lock(&locks[i]);
  }

  g->updated[0] = 1;
  g->visited[0] = 1;
  bool flag = 1;
  double before_bfs = rtclock();

  float first_total, second_total = 0;

  cout << "Before bfs " << endl;
  
  int iter = 0;
  while (flag) {
    flag = 0;
#pragma omp parallel
    {
#pragma omp for schedule (dynamic, 50)// num_threads(nthreads)
    for (int i = 0; i < g->num_vertices; ++i) {
      // cout << "Iter: " << iter << endl;
      if (g->updated[i]) {
        g->updated[i] = 0;
        for (int j = g->vertices[i];
             j < g->vertices[i+1]; ++j) {
          if (!g->visited[g->edges[j]] &&
              g->vertex_value[i] + 1 <= g->vertex_value[g->edges[j]]) {
				    omp_set_lock(&locks[g->edges[j]]);
            g->vertex_value[g->edges[j]] = g->vertex_value[i] + 1;
            g->updating[g->edges[j]] = 1;
				    omp_unset_lock(&locks[g->edges[j]]);
          }
        }
      }
    } 

#pragma omp for schedule (dynamic, 50)// num_threads(nthreads)
    for (int i = 0; i < g->num_vertices; ++i) {
      if (g->updating[i]) {
        g->updated[i] = 1;
        g->visited[i] = 1;
        flag = 1;
        g->updating[i] = 0;
      } 
    }
    }
    ++iter;
  }

  cout << "After bfs " << endl;

  double after_bfs = rtclock();
  cout << "Time: " <<(after_bfs - before_bfs) << endl;
  cout << "iter: " << iter << endl;
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

  cout << "Num of vertices: " << num_vertices << endl;

  while (getline(input, line)) {
    g->vertices[line_no] = g->edges.size();
    istringstream ss(line);
    int edge;
    while (ss >> edge) {
      g->edges.push_back(edge); 
    }
    ++line_no;
  }

  g->num_edges = g->edges.size();
  g->vertices.push_back(g->num_edges);

  return g;
}

int main() {
	string filename("../input/soc-metis.txt");
	// string filename("/work/02687/binren/graph/code/input/livejournal/soc-livejournal-standard.txt");
  graph<float, float>* g = ReadGraph<float, float>(filename);
  cout << "Read done." << endl;
  g->vertex_value = new float[g->num_vertices];
  // Set vertex value.
  for (int i = 0; i < g->num_vertices; ++i) {
    g->vertex_value[i] = 999999;
  }
  g->vertex_value[0] = 0;

  cout << "Set vertex value done." << endl;
  Bfs(g);
  delete g;
}
