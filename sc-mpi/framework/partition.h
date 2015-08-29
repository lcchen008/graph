#ifndef PARTITION_H
#define PARTITION_H 

#include "graph.h"

#include <vector>
#include <iostream>
using namespace std;

// Function for constructing a graph from an input file.
// It constructs a graph from a graph input as well as
// a partitioning file.
template <class VertexValue, class EdgeValue>
graph<VertexValue, EdgeValue>* GetMyGraph(string graph_filename,
                                          string partition_filename,
                                          int global_num_vertices,
                                          int my_rank) {
  int* global_to_local_map = new int[global_num_vertices];
  ifstream partition_input(partition_filename.c_str());
  int part;
  int count = 0;
  int my_num_vertices = 0;

  while (partition_input >> part) {
    global_to_local_map[count] = -1;
    if (part == my_rank) {
      global_to_local_map[count] = my_num_vertices;
      ++my_num_vertices;
    }
    ++count;
  }

  graph<VertexValue, EdgeValue>* g =
      new graph<VertexValue, EdgeValue>(global_num_vertices,
                                        my_num_vertices);

  g->global_to_local_map = global_to_local_map;
  g->remote_in_degree.resize(global_num_vertices, 0);

  // Readin graph.
  ifstream graph_input(graph_filename.c_str());		
	string line;
	int edge;
	int line_no = 0;

  // Exhaust the first line.
  getline(graph_input, line);

  int vertex_count = 0;
	while(getline(graph_input, line)) {
    // Get in-degree info.
    istringstream iss(line);
    int iedge;
    while (iss >> iedge) {
      // --iedge;
      int local_vid = g->global_to_local_map[iedge];
      // Generate local vertex indegree.
      if (local_vid != -1) {
        ++g->in_degree[local_vid].in_degree;
      }
    } 

    // Store to local edges.
    if (g->global_to_local_map[line_no] != -1) {
      g->local_to_global_map.push_back(line_no);
      g->vertices[vertex_count] = g->edges.size(); 
		  istringstream ss(line);
      while(ss >> edge) {
			  g->edges.push_back(edge);	
        // Gen remote vertex indegree (going from local vertices).
        if (g->global_to_local_map[edge] == -1) {
          ++g->remote_in_degree[edge];
        }
		  }
      ++vertex_count;
    }
		++line_no;
	}

  cout << my_rank <<  " vertex count: " << vertex_count << endl;
  cout << my_rank << " line no: " << line_no << endl;

	g->num_edges = g->edges.size();
	cout << my_rank << " edges: " << g->num_edges << endl;

	// A dummy last vertex should be appended to notify the end.
	g->vertices.push_back(g->num_edges);

	// Condense.
	for(int i = g->num_vertices - 1; i >= 0; i--) {
		if(g->vertices[i] == -1) {
			g->vertices[i] = g->vertices[i + 1];	
		}
	}

	g->vertex_value = new VertexValue[g->num_vertices];

  /*
	for(int i = 0; i < g->num_vertices; i++) {
		g->vertex_value[i] = INT_MAX;
  }

  // By default, all vertices are deactived in graph DS.
  // Here we set the first vertex in rank 0 as source.

  int local_id = global_to_local_map[0];
  cout << "====> Local ID is: " << local_id << endl;
  if (local_id != -1) {
	  g->status[local_id] = 1;
	  g->vertex_value[local_id] = 0;
  }

	// Gen edge value, i.e., the weight.
	g->edge_value = new float[g->num_edges];
	srand(2014);
	int max = 10, min = 1;
  
  cout << "After genning edge value..." << endl;

	for(size_t i = 0; i < g->num_edges; i++) {
		(g -> edge_value)[i] = rand() % (max - min + 1) + min;
	}

  cout << my_rank << " graph load done." << endl;
  */

  return g;
}

#endif /* PARTITION_H */
