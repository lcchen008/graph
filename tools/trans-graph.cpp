// This util converts the original file like soc to standard
// format that Metis can accept. Even a vertex has no neighbors,
// it takes a line with empty neighbors.

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>

#include "graph.h"

using namespace std;

template <class VertexValue, class EdgeValue>
graph<VertexValue, EdgeValue> *ReadGraph(int num_vertices, string filename) {
	// Construct graph.
	graph<VertexValue, EdgeValue> *g = new graph<VertexValue, EdgeValue>(num_vertices);
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

  cout << endl;
	cout<<"Read done :)"<<endl;
	cout<<"Num of vertices: "<<line_no<<endl;
	cout<<"Num of edges: "<<g->num_edges<<endl;
	cout<<"Memory space taken: "<< g->num_edges * 4 * 2 / 1048576 <<"MB"<<endl;
	cout << "*************************************" << endl;

  input.close();
	return g;
}

template <class VertexValue, class EdgeValue>
void WriteGraph(const graph<VertexValue, EdgeValue>* g, string filename) {
  ofstream output(filename.c_str());
  // Output num_vertices, num_edges. 
  output << " " << g->num_vertices << " " << g->num_edges << endl;

  // Output connectivity.
  for (int i = 0; i < g->num_vertices; ++i) {
    ostringstream line;
    for (int j = g->vertices[i]; j < g->vertices[i+1]; ++j) {
      line << " ";
      line << g->edges[j]; // 1 based. Remove + 1 to change to 0 based.
    }
    output << line.str() << endl;
  }
  output.close();
}

int main() {
	string filename("/work/02687/binren/graph/code/input/dag/dag-38394");
  graph<float, float>* g = ReadGraph<float, float>(38394, filename);
  cout << "Read done." << endl;
  WriteGraph(g, "/work/02687/binren/graph/code/input/dag/dag-38394-standard");
  cout << "Write done." << endl;
  delete g;
}
