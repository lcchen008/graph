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

  int num_vertices = 38394;
	// string filename1("../input/dag/dag.txt");
	// string partition_filename1("../input/dag/dag-1-0-metis");  // CPU-only.
	// string partition_filename1("../input/dag/dag-0-1-metis");  // MIC-only.
	// string partition_filename1("../input/dag/dag-2-3-metis");

  string filename1("../input/dag/dag-38394-standard");
	// string partition_filename1("../input/dag/dag-38394-1-0-metis");  // CPU-only.
	// string partition_filename1("../input/dag/dag-38394-blk-partition-map-1-4.txt");  // CPU-only.
	// string partition_filename1("../input/dag/dag-rr-partition-map-1-4.txt");  // CPU-only.
	// string partition_filename1("../input/dag/dag-hybrid-1-4-metis");  // CPU-only.
	string partition_filename1("../input/dag/dag-38394-0-1-metis");  // MIC-only.
	// string partition_filename1("../input/dag/dag-38394-blk-partition-map-1-1.txt");  // MIC-only.
	// string partition_filename1("../input/dag/dag-2-3-metis");

  graph<float, float>* g1 = GetMyGraph<float, float>(filename1,
                                                     partition_filename1,
                                                     num_vertices, my_rank);

  for (int i = 0; i < num_vertices; ++i) {
    int in_degree = g1->get_in_degree(i);
    g1->set_vertex_value(i, 0);
    g1->set_vertex_value(i, in_degree);
    if (in_degree > 0) {
      g1->set_status(i, 0);
    } else {
      g1->set_status(i, 1);
    }
  }

  scheduler<float, float, float> s(g1, my_rank);
	s.start();
  delete g1;
	cout << "Finish." << endl;
  MPI_Finalize();
	return 0;
}
