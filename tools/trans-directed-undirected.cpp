// This util translates an undirected graph to a directed graph.
// The purpose of this util is to generate input for metis partitioning.
// The input file is 0-based.

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <unordered_set>

#include "graph.h"

using namespace std;

void ReadGraph(string ifilename, string ofilename) {
	// Construct graph.
	ifstream input(ifilename.c_str());		
	string line;
	int edge;
	int line_no = 0;

	cout << "*************************************" << endl;
	cout << "Begin reading graph input." << endl;

  getline(input, line);
  istringstream ss(line);
  int num_vertices;
  ss >> num_vertices;
  cout << "Vertices: " << num_vertices << endl;
  vector<unordered_set<int> > data(num_vertices); 

	while(getline(input, line)) {
		istringstream ss(line);
		while(ss >> edge) {
      // edge--;
      if (line_no >= num_vertices) {
        cout << "Line no out of bound: " << line_no << endl;
      }
      if (edge >= num_vertices) {
        cout << "Edge out of bound: " << edge << endl;
      }
      data[line_no].insert(edge);
      data[edge].insert(line_no);
		}
		line_no++;
	}

  int num_edges = 0;
  for (auto i = data.begin(); i != data.end(); ++i) {
    num_edges += i->size();
  }

  // Write the output to file.
  ofstream output(ofilename.c_str());
  output << " " << num_vertices << " " << num_edges/2 << endl;

  for (int i = 0; i < num_vertices; ++i) {
    ostringstream line;
    for (auto j = data[i].begin(); j != data[i].end(); ++j) {
      line << " ";
      line << *j + 1; 
    }
    output << line.str() << endl;
  }
}

int main() {
  ReadGraph("/work/02687/binren/graph/code/input/dag/dag-38394-standard",
            "/work/02687/binren/graph/code/input/dag/dag-38394-undirected");
  // ReadGraph("/work/02687/binren/graph/code/input/dag/dag.txt",
  //           "/work/02687/binren/graph/code/input/dag/dag-metis-undirected.txt");
}
