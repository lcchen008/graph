#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unordered_set>
#include <fstream>
#include <vector>
#include <iostream>
using namespace std;

#define MIN_PER_RANK 1 /* Nodes/Rank: How 'fat' the DAG should be.  */
#define MAX_PER_RANK 5
#define MIN_RANKS 3    /* Ranks: How 'tall' the DAG should be.  */
#define MAX_RANKS 86000
#define RANKS 30000
#define PERCENT 2     /* Chance of having an Edge.  */

void gen_graph(string filename) {
  vector<unordered_set<int> > graph;
  int i, j, k,nodes = 0;
  // srand (time (NULL));
  srand (2014);

  // int RANKS = MIN_RANKS
  //     + (rand () % (MAX_RANKS - MIN_RANKS + 1));

  cout << "Ranks: " << RANKS << endl;

  printf ("digraph {\n");
  for (i = 0; i < RANKS; i++) {
    /* New nodes of 'higher' rank than all nodes generated till now.  */
    int new_nodes = MIN_PER_RANK
        + (rand () % (MAX_PER_RANK - MIN_PER_RANK + 1));

    for (int i = 0; i < new_nodes; ++i) {
      graph.push_back(unordered_set<int>());
    }

    /* Edges from old nodes ('nodes') to new ones ('new_nodes').  */
    for (j = 0; j < nodes; j++) {
      for (k = 0; k < new_nodes; k++) {
        if ( (rand () % 100) < PERCENT) {
          graph[j].insert(k+nodes);
          // printf ("  %d -> %d;\n", j, k + nodes); /* An Edge.  */
        }
      }
    }

    nodes += new_nodes; /* Accumulate into old node set.  */
  }

  // Write graph.
  int total_edges = 0;
  for (auto i = graph.begin(); i != graph.end(); ++i) {
    total_edges += i->size();
  }
  ofstream output(filename.c_str());
  output << " " << graph.size() << " " << total_edges << endl; 
  cout << "Nodes: " << graph.size() << " Edges: " << total_edges << endl;
  for (auto i = graph.begin(); i < graph.end(); ++i) {
    for (auto j = i->begin(); j != i->end(); ++j) {
      output << " " << *j;
    }
    output << endl;
  }

  output.close();
}

int main (void) {
  gen_graph("/work/02687/binren/graph/code/input/dag/dag.txt");
  return 0;
}
