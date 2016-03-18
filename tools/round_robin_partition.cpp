// This tool generates round robin graph partitioning
// scheme. It outputs a file containing the mapping
// between vertices to partition IDs.

#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

void GenProcessPartitionMap(int num_vertices,
                            vector<int>& ratios,
                            string filename) {
  int total_ratio = 0;
  for (int i = 0; i < ratios.size(); ++i) {
    total_ratio += ratios[i];
  }

  cout << "Total ratios: " << total_ratio << endl;

  // For the number of vertices divisible by the total ratio.
  int num_total_ratios = num_vertices/total_ratio;

  // Gen a local map, for e.g., ratios are 1, 2, 3:
  // then the local map is: 0, 1, 1, 2, 2, 2.
  vector<int> local_map(total_ratio);
  int index = 0;
  for (int i = 0; i < ratios.size(); ++i) {
    for (int j = 0; j < ratios[i]; ++j) {
      local_map[index] = i;
      ++index;
    }  
  }

  cout << "Outputting..." << endl;
  ofstream output(filename.c_str());

  // Ouput the partition map.
  for (int i = 0; i < num_vertices; ++i) {
     int pos = i % total_ratio;
     output << local_map[pos] << endl;
  }
  
  output.close();
}

int main() {
  vector<int> ratios;
  ratios.push_back(2);
  ratios.push_back(1);
  GenProcessPartitionMap(425957, ratios, "/work/02687/binren/graph/code/input/com/com-rr-partition-map-2-1.txt");
}
