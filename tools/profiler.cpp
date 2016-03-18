// This util generates a histgram according to out-degree. 
// The input is 0-based.

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>

#include "graph.h"

using namespace std;

template <class VertexValue, class EdgeValue>
graph<VertexValue, EdgeValue> *ReadGraph(string filename) {
	// Construct graph according to standard format.
	ifstream input(filename.c_str());		
	string line;
	string first;
	int edge;
  int vid = 0;

	cout << "*************************************" << endl;
	cout << "Begin reading graph input." << endl;

  getline(input, line);

  int num_vertices;
  istringstream vs(line); 
  vs >> num_vertices;

	graph<VertexValue, EdgeValue> *g = new graph<VertexValue, EdgeValue>(num_vertices);

	while(getline(input, line)) {
		istringstream ss(line);
		g->vertices[vid] = g->edges.size();
		while(ss >> edge) {
			g->edges.push_back(edge);	
      ++g->in_degree[edge];
		}
    ++vid;
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
	cout<<"Num of vertices: "<<vid<<endl;
	cout<<"Num of edges: "<<g->num_edges<<endl;
	cout<<"Memory space taken: "<< g->num_edges * 4 * 2 / 1048576 <<"MB"<<endl;
	cout << "*************************************" << endl;

	return g;
}

template <class VertexValue, class EdgeValue>
void WriteHistgram(const graph<VertexValue, EdgeValue>* g, string filename) {
  ofstream output(filename.c_str());
  // Output num_vertices, num_edges. 
  // output << " " << g->num_vertices << " " << g->num_edges << endl;

  // Output connectivity.
  int m = 0;
  for (int i = 0; i < g->num_vertices; ++i) {
    ostringstream line;
    int n = g->vertices[i+1] - g->vertices[i];
    m = max(m, n);
    line << i << " " << n;
    output << line.str() << endl;
  }

  cout << "Max Degree: " << m << endl;
}

template <class VertexValue, class EdgeValue>
void WriteInDegreeHistgram(graph<VertexValue, EdgeValue>* g, string filename) {
  ofstream output(filename.c_str());

  // Output connectivity.
  sort(g->in_degree.begin(), g->in_degree.end());
  int m = 0;
  int total_zero = 0;
  for (int i = 0; i < g->num_vertices; ++i) {
    ostringstream line;
    int n = g->in_degree[i];
    if (!n) {
      ++total_zero;
    }
    m = max(m, n);
    line << i << " " << n;
    output << line.str() << endl;
  }

  cout << "Max In Degree: " << m << endl;
  cout << "Total zero indegree: " << total_zero << endl;
}


int main() {

  /*
	string filename("/work/02687/binren/graph/code/input/livejournal/soc-livejournal-standard.txt");
  graph<float, float>* g = ReadGraph<float, float>(4847571, filename);
  cout << "Read done." << endl;
  WriteHistgram(g, "/work/02687/binren/graph/code/input/livejournal/livejournal-histgram.txt");
  cout << "Write done." << endl;
  */

  string filename("/work/02687/binren/graph/code/input/dag/dag-38394-standard");
  graph<float, float>* g = ReadGraph<float, float>(filename);
  cout << "Read done." << endl;
  // WriteInDegreeHistgram(g, "/work/02687/binren/graph/code/input/com/com-indegree-histgram.txt");
  WriteHistgram(g, "/work/02687/binren/graph/code/input/dag/dag-outdegree-histgram.txt");
  cout << "Write done." << endl;

  delete g;
}
