// This tool transforms the pokec graph input to 0-based format.
// Each vertex takes a line. Each line contains all the edges.
// e.g., 1 2 3
//       0 3 2
//       
//       2 3 1
// Alternatively, invoking function ConvertToUndirected will
// output a format that is suitable for metis partioner.

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_set>

#include "graph.h"

using namespace std;

// Convert to standard format.
void WriteGraphToStandard(const graph<float, float>* g,
                          const string& filename) {
  cout << "Writing standard graph..." << endl;
  ofstream output(filename.c_str());
  // Output num_vertices, num_edges. 
  output << " " << g->num_vertices << " " << g->num_edges << endl;

  // Output connectivity.
  for (int i = 0; i < g->num_vertices; ++i) {
    ostringstream line;
    for (int j = g->vertices[i]; j < g->vertices[i+1]; ++j) {
      line << " ";
      line << g->edges[j];
    }
    output << line.str() << endl;
  }

  output.close();
}

// Convert to undirected format.
void ConvertToUndirected(const graph<float, float>* g,
                         const string& filename) {

  vector<unordered_set<int> > data(g->num_vertices);
  int edge;

  for (int i = 0; i < g->num_vertices; ++i) {
    for (int j = g->vertices[i]; j < g->vertices[i+1]; ++j) {
      edge = g->edges[j];
      data[i].insert(edge);
      data[edge].insert(i);
    }
  }

  ofstream output(filename.c_str());

  int num_edges = 0;
  for (auto i = data.begin(); i != data.end(); ++i) {
    num_edges += i->size();
  }

  cout << "Writing, undirected number of edges: " << num_edges/2 << endl;

  // Write the output to file.
  output << " " << g->num_vertices << " " << num_edges/2 << endl;

  for (int i = 0; i < g->num_vertices; ++i) {
    ostringstream line;
    for (auto j = data[i].begin(); j != data[i].end(); ++j) {
      line << " ";
      line << *j + 1; 
    }
    output << line.str() << endl;
  }

  output.close();
}

// Raw pokec input is 1 based. 
void TransGraph(int num_vertices,
                const string& input_filename,
                const string& output_filename) {
  // Read in graph.
	graph<float, float> *g =
      new graph<float, float>(num_vertices);
  ifstream input(input_filename.c_str());

  string line;
  string first;
  string second;

  int prev_vid = -1;
  int vid, edge;

  while (getline(input, line)) {
    istringstream ss(line);

    //the first is the source
		ss >> first;
		vid = atoi(first.c_str()) - 1;
		ss >> second;
		edge = atoi(second.c_str()) - 1;
		if(vid != prev_vid) {
      g->vertices[vid] = g->edges.size();
			prev_vid = vid;
		}
    g->edges.push_back(edge);
  }

	g->num_edges = g->edges.size();
	cout << "Directed edges: " << g->num_edges << endl;

	// A dummy last vertex should be appended to notify the end.
	g -> vertices.push_back(g->num_edges);

	// Condense.
	for(int i = g->num_vertices - 1; i >= 0; i--) {
		if(g->vertices[i] == -1) {
			g->vertices[i] = g->vertices[i + 1];	
		}
	}
  
  input.close(); 

  // Convert graph.
  cout << "Converting graph." << endl;
  // WriteGraphToStandard(g, output_filename);
  ConvertToUndirected(g, output_filename);
}

int main() {
  TransGraph(1632803,
             "../code/input/pokec/soc-pokec-relationships.txt",
             "../code/input/pokec/soc-pokec-undirected.txt");
}
