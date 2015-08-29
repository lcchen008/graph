#include <fstream>
#include <iostream>
#include <sstream>
#include <math.h>
#include <algorithm>
#include <sys/time.h>
using namespace std;
#include "semi.h"
#include "graph.h"

#include <omp.h>

#define ITERS 1
#define nthreads 180 

double rtclock()
{
  struct timezone Tzp;
  struct timeval Tp;
  int stat;
  stat = gettimeofday (&Tp, &Tzp);
  if (stat != 0) printf("Error return from gettimeofday: %d",stat);
  return(Tp.tv_sec + Tp.tv_usec*1.0e-6);
}

bool contains(cluster &c, int vertex_id)
{
	for(int i = 0; i < c.size(); i++)	
	{
		if(c[i] == vertex_id)		
			return true;
	}

	return false;
}

float compute_score(graph<cluster_list, float, msg_cluster_list> *g, cluster &c) {
	float Ic = 0;
	float Bc = 0;
	int Vc = c.size();
	
	for (int i = 0; i < c.size(); i++) {
		for (int j = g->vertices[i]; j < g->vertices[i+1]; ++j)	{
			size_t dst = g -> edges[j];	
			float weight = g -> edge_value[j];
			//inner edge
			if (contains(c, dst)) {
				Ic += weight;		
      } else {
				Bc += weight;
      }
		}
	}

	if (Vc == 0 || Vc == 1) {
		return 0;
  } else {
		return (Ic -fB * Bc)/(Vc * (Vc - 1))*2;	
  }
}

bool compare_score(const cluster &c1, const cluster &c2) {
	return c1.score < c2.score;	
}

template <class VertexValue, class EdgeValue, class MessageValue>
void Sc(graph<VertexValue, EdgeValue, MessageValue>* g) {

  omp_set_num_threads(nthreads);
	omp_lock_t *locks = new omp_lock_t[g->num_vertices];
	for(int i = 0; i < g->num_vertices; i++) {
		omp_init_lock(&locks[i]);
  }

  double before_sc = rtclock();
  for (int i = 0; i < ITERS; ++i) {
    // Exchange clusters.
#pragma omp parallel 
    {
#pragma omp for schedule(dynamic,50)
      for (int j = 0; j < g->num_vertices; ++j) {
        msg_cluster_list msg; 
        for (int k = 0; k < CLUSTER_PER_MSG &&
             k < g->vertex_value[j].size(); ++k) {
          msg.insert(g->vertex_value[j][k]);
        }

        for (int m = g->vertices[j];
             m < g->vertices[j+1]; ++m) {
				  omp_set_lock(&locks[g->edges[m]]);
          g->msg_bufs[g->edges[m]].insert(msg);
          omp_unset_lock(&locks[g->edges[m]]);
        }
      }

      // Sort clusters and update the buffers and vertex value.
#pragma omp for schedule(dynamic,50)
      for (int j = 0; j < g->num_vertices; ++j) {
	      cluster_list &my_list = g -> vertex_value[j];	
        cluster to_sort[SORT_MAX];
        int sort_idx = 0;

        for (; sort_idx < SORT_MAX &&
             sort_idx < my_list.size(); ++sort_idx) {
          to_sort[sort_idx] = my_list[sort_idx];
        }

        for (int k = 0; k < g->msg_bufs[j].size(); ++k) {
          msg_cluster_list& cl = g->msg_bufs[j][k];
          for (int m = 0; m < cl.size(); ++m) {
            cluster& c = cl[m];
            if (!contains(c, j)) {
              if (c.size() < VMAX) {
                c.insert(j);
                if (sort_idx < SORT_MAX) {
                  to_sort[sort_idx] = c;
                  ++sort_idx;
                }
              }
            }
          }
        }

        for (int k = 0; k < sort_idx; ++k) {
          to_sort[i].score = compute_score(g, to_sort[i]);
        }

        if (sort_idx > 1) {
          sort(&to_sort[0], &to_sort[0] + sort_idx - 1, compare_score);
        }

        g->vertex_value[j].clear();

        for (int k = 0; k < sort_idx && i < CMAX; ++i) {
          g->vertex_value[j].insert(to_sort[k]);
        }

        g->msg_bufs[j].re_init();
      }
    } // omp parallel
  }

  double after_sc = rtclock();
  cout << "Sc time: " << after_sc - before_sc << endl;
}

template <class VertexValue, class EdgeValue, class MessageValue>
graph<VertexValue, EdgeValue, MessageValue>* ReadGraph(const string filename) {
  ifstream input(filename.c_str()); 
  string line;
  getline(input, line);
  istringstream ss(line);
  int num_vertices;
  ss >> num_vertices;
  int line_no = 0;

  graph<VertexValue, EdgeValue, MessageValue>* g =
      new graph<VertexValue, EdgeValue, MessageValue>(num_vertices,
                                                      num_vertices);

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
  int num_vertices = 425957;
	string filename("../input/com/com-standard");
  graph<cluster_list, float, msg_cluster_list>* g =
      ReadGraph<cluster_list, float, msg_cluster_list>(filename);

  cout << "Read done." << endl;

  // Set vertex value.
  g->vertex_value = new cluster_list[num_vertices];
  for (int i = 0; i < num_vertices; ++i) {
    cluster c;
    c.insert(i);
    g->vertex_value[i].insert(c);
  }

  // Set edge value.
  srand(2014);
  int min = 1, max = 10;
  g->edge_value = new float[g->num_edges];
  for (int i = 0; i < g->num_edges; ++i) {
    g->edge_value[i] = rand()%10 + min;
  }

  // Init msg bufs.
  for (int i = 0; i < g->num_vertices; ++i) {
    for (int j = g->vertices[i];
         j < g->vertices[i+1]; ++j) {
      ++g->in_degree[g->edges[j]].in_degree;
    }
  } 

  for (int i = 0; i < g->num_vertices; ++i) {
    g->msg_bufs[i].set_length(g->in_degree[i].in_degree);
  }

  cout << "Computing..." << endl;
  Sc(g);
  delete g;
}
