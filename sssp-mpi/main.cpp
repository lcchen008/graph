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
using namespace std;

int main(int argc, char* argv[]) {
  int provided;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  cout << "My rank is: " << my_rank << endl;

  int num_vertices = 1632803;
	string filename1("../input/soc-metis.txt");
	string partition_filename1("../input/pokec/soc-partition-map-1-0.txt");  // CPU-only.
	// string partition_filename1("../input/pokec/soc-partition-map-0-1.txt");  // MIC-only.
	// string partition_filename1("../input/pokec/soc-metis-2-3-metis");
	// string partition_filename1("../input/pokec/soc-partition-map-3-13.txt");
	// string partition_filename1("../input/soc-metis-undirected.txt.part.2");
	// string partition_filename1("../input/soc-metis-1-0-metis");
	// string partition_filename1("../input/pokec/soc-partition-map-2-3.txt");
	// string partition_filename1("../input/soc-partition-map-1-9.txt");
  graph<float, float>* g1 = GetMyGraph<float, float>(filename1,
                                                     partition_filename1,
                                                     num_vertices, my_rank);
  
  for (int i = 0; i < num_vertices; ++i) {
    g1->set_status(i, 0);
    g1->set_vertex_value(i, INT_MAX);
  } 

  g1->set_status(0, 1);
  g1->set_vertex_value(0, 0);

  g1->edge_value = new float[g1->num_edges];

  // Set edge value.
  for (int i = 0; i < g1->num_edges; ++i) {
    g1->edge_value[i] = 1;
  }

  scheduler<float, float, float> s(g1, my_rank);
	s.start();
  delete g1;
	cout << "Finish." << endl;
  MPI_Finalize();
	return 0;
}
