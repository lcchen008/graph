#include <mpi.h>
#include "framework/scheduler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <queue>
#include <unordered_set>
#include "framework/conf.h"
#include "framework/partition.h"
#include "semi.h"
using namespace std;

graph<float, float> *read_graph(int num_vertices, string filename) {
	// Construct graph.
	graph<float, float> *g = new graph<float, float>(num_vertices,
                                                   num_vertices);
	ifstream input(filename.c_str());		
	string line;
	string first;
	int edge;
	int line_no=0;

	cout << "*************************************" << endl;
	cout << "Begin reading graph input." << endl;

	while(getline(input, line)) {
		line_no++;
		istringstream ss(line);
		ss >> first;
		int vid = atoi(first.substr(0, first.size() - 1).c_str()) - 1;
		g->vertices[vid] = g->edges.size();
		while(ss >> edge) {
			g->edges.push_back(edge-1);	
		}
	}
	g->num_edges = g->edges.size();
	cout<<"Edges: "<<g->num_edges<<endl;

	// A dummy last vertex should be appended to notify the end.
	g -> vertices.push_back(g->num_edges);

	// Condense.
	for(int i = num_vertices - 1; i >= 0; i--) {
		if(g->vertices[i] == -1) {
			g->vertices[i] = g->vertices[i + 1];	
		}
	}

	g->vertex_value = new float[num_vertices];
	//g->vertex_value[0] = 0;

  // By default, all vertices are deactived in graph DS.
  // Here we set the first vertex as source.
	g->status[0] = 1;
	g->vertex_value[0] = 0;

	for(int i = 1; i < g->num_vertices; i++) {
		g->vertex_value[i] = INT_MAX;
  }

	// Gen edge value, i.e., the weight.
	g->edge_value = new float[g->num_edges];
	srand(2014);
	int max = 10, min = 1;

	for(size_t i = 0; i < g -> num_edges; i++) {
		(g -> edge_value)[i] = rand() % (max - min + 1) + min;
	}

  cout << endl;
	cout<<"Read done :)"<<endl;
	cout<<"Num of vertices: "<<line_no<<endl;
	cout<<"Num of edges: "<<g->num_edges<<endl;
	cout<<"Memory space taken: "<< g->num_edges * 4 * 2 / 1048576 <<"MB"<<endl;
	cout << "*************************************" << endl;

	return g;
}

void seq(graph<float, float> *g, int* levels) {
  double before_seq = rtclock();
  cout << "Seq in progress..." << endl;
  queue<int> q[2];
  bool use = 0;
  q[use].push(0);
  unordered_set<int> s;
  s.insert(0);
  levels[0] = 0;
  int total_size = 1;
  int level = 1;

  while (!q[use].empty()) {
    while (!q[use].empty()) {
      int front = q[use].front();
      q[use].pop();
      for (int j = g->vertices[front]; j < g->vertices[front+1]; ++j) {
        if (s.find(g->edges[j]) == s.end()) {
          levels[g->edges[j]] = level;
          q[!use].push(g->edges[j]);
          s.insert(g->edges[j]);
        }
      }
    }
    use = !use;
    ++level;
    cout << "size of queue: " << q[use].size() << endl;
    total_size += q[use].size();
  }
  double after_seq = rtclock();
  cout << "total size: " << total_size << endl;
  cout << "seq time: " << after_seq - before_seq << endl;
}

int main(int argc, char* argv[]) {
  int provided;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  cout << "My rank is: " << my_rank << endl;

	// string filename("../input/soc-out.txt");
	// graph<float, float> *g = read_graph(2132803, filename);
  // int* levels = new int[g->num_vertices];
  // for (int i = 0; i < g->num_vertices; ++i) {
  //   levels[i] = INT_MAX;
  // }

  // seq(g, levels);
	
  // int diffs = 0;
  // for (int i = 0; i < g->num_vertices; ++i) {
  //   if (levels[i] != g->vertex_value[i]) {
  //     ++diffs;
  //     cout << "Vertex " << i << " not equal: "
  //          << levels[i] << " vs " << g->vertex_value[i] << endl;
  //   }
  // }

	// delete g;
  // delete levels;
  // cout << "Num of diffs: " << diffs << endl;

  int num_vertices = 425957;
	string filename1("../input/com/com-standard");
	// string partition_filename1("../input/com/com-1-0-metis");  // MIC-only.
  // string partition_filename1("../input/com/com-blk-partition-map-2-1.txt");
  // string partition_filename1("../input/com/com-rr-partition-map-2-1.txt");
  string partition_filename1("../input/com/com-hybrid-2-1-metis");
	// string partition_filename1("../input/com/com-1-0-metis");  // CPU-only.
	// string partition_filename1("../input/pokec/soc-partition-map-0-1.txt");  // MIC-only.
	// string partition_filename1("../input/pokec/soc-metis-2-3-metis");
	// string partition_filename1("../input/pokec/soc-partition-map-3-13.txt");
	// string partition_filename1("../input/soc-metis-undirected.txt.part.2");
	// string partition_filename1("../input/soc-metis-1-0-metis");
	// string partition_filename1("../input/pokec/soc-partition-map-2-3.txt");
	// string partition_filename1("../input/soc-partition-map-1-9.txt");
  graph<cluster_list, float>* g1 = GetMyGraph<cluster_list, float>
      (filename1,
       partition_filename1,
       num_vertices, my_rank);
  
  // Set vertex value.
  for (int i = 0; i < num_vertices; ++i) {
    g1->set_status(i, 1);
    cluster c;
    c.insert(i);
    int local_id = g1->global_to_local_map[i];
    if (local_id != -1) {
      g1->vertex_value[local_id].insert(c);
    }
  } 

  g1->edge_value = new float[g1->num_edges];

  // Set edge value.
  srand(2014);
  int min = 1, max = 10;
  for (int i = 0; i < g1->num_edges; ++i) {
    g1->edge_value[i] = rand() % 10 + min;
  }

  scheduler<cluster_list, float, msg_cluster_list> s(g1, my_rank);
	s.start();
  delete g1;
	cout << "Finish." << endl;
  MPI_Finalize();
	return 0;
}
