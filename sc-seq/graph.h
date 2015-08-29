// A skeloton for CSR graph format.

#ifndef GRAP_H_
#define GRAP_H_

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;
#include "msg_buf.h"

struct in_degree_map {
  int vid;
  int in_degree;
};

bool compare(in_degree_map i1, in_degree_map i2) {
  return i1.in_degree > i2.in_degree;
}

template <class VertexValue, class EdgeValue, class MessageValue>
class graph {
	public:
    size_t global_num_vertices;
		size_t num_vertices;
		size_t num_edges;

		// Array vertices store the starting position
    // of the edge going from each vertex. vertices
    // should end with num_edges while generating
    // CSR graph.
		vector<int> edges;	
		vector<long long> vertices;
    vector<in_degree_map> in_degree;

    // Records the number of messages sent to each remote vertex.
    // The messages are sent from local vertices.
    vector<int>  remote_in_degree;
	  vector<int> updated;

    // An array for mapping global id to local position.
    // It is used for redirecting edge value to correct
    // local position. This array only serves for local
    // graph, and thus contains only the mapping for local
    // vertices.
    int* global_to_local_map;
    vector<int> local_to_global_map;

		VertexValue *vertex_value;
		EdgeValue *edge_value;
    vector<VertexValue> order;
    vector<msg_buf<MessageValue> > msg_bufs;

		graph(size_t global_num_vertices, size_t num_vertices);
		~graph();

		// Set all vertices to inactive.
    void set_vertex_value(size_t global_vertex_id, VertexValue value);

    // Returns -1 if the global_vertex_id is in remote.
    int get_in_degree(size_t global_vertex_id);
};

template <class VertexValue, class EdgeValue, class MessageValue>
graph<VertexValue, EdgeValue, MessageValue>::graph(size_t global_num_vertices,
                                     size_t num_vertices) {
  this->global_num_vertices = global_num_vertices;
	this->num_vertices = num_vertices;
	this->vertices.resize(num_vertices, -1);
  this->in_degree.resize(num_vertices);
  this->order.resize(num_vertices);
  this->updated.resize(num_vertices, 0);
  this->msg_bufs.resize(num_vertices);
  
  // Initialize.
  for(int i = 0; i < num_vertices; i++) {
    in_degree[i].vid = i;	
    in_degree[i].in_degree = 0;
  }

  edge_value = 0;
  vertex_value = 0;
  global_to_local_map = 0;
}

template <class VertexValue, class EdgeValue, class MessageValue>
graph<VertexValue, EdgeValue, MessageValue>::~graph() {
  if (num_vertices && vertex_value) {
	  delete [] vertex_value;
  }
  if (edge_value && num_vertices) {
    delete[] edge_value;
  }
  if (global_to_local_map) {
    delete[] global_to_local_map;
  }
}

template <class VertexValue, class EdgeValue, class MessageValue>
void graph<VertexValue, EdgeValue, MessageValue>::set_vertex_value(size_t global_vertex_id,
                                                     VertexValue value) {
  int local_id = global_to_local_map[global_vertex_id];
  if (local_id == -1) {
    return;
  } else {
    vertex_value[local_id] = value;    
  }
}

template <class VertexValue, class EdgeValue, class MessageValue>
int graph<VertexValue, EdgeValue, MessageValue>::get_in_degree(size_t global_vertex_id) {
  int local_id = global_to_local_map[global_vertex_id];
  if (local_id == -1) {
    return -1;
  } else {
    return in_degree[local_id].in_degree;
  }
}

#endif
