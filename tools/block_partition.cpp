// This tool generates block partitioning
// scheme. It outputs a file containing the mapping
// between vertices to partition IDs.

#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

void GenMap(int num_vertices, vector<int>& ratios, string filename) {
  int total_ratio = 0;
  for (int i = 0; i < ratios.size(); ++i) {
    total_ratio += ratios[i];
  }

  cout << "Total ratios: " << total_ratio << endl;

  vector<int> counts(ratios.size());

  for (int i = 0; i < ratios.size() - 1; ++i) {
    counts[i] = num_vertices * ratios[i] / total_ratio;
  }

  int remain = num_vertices;
  for (int i = 0; i < counts.size(); ++i) {
    remain -= counts[i];
  }

  counts.back() = remain;

  // output.
  ofstream output(filename.c_str());
  for (int i = 0; i < counts.size(); ++i) {
    for (int j = 0; j < counts[i]; ++j) {
      output << i << endl;
    }
  }

  output.close();
}

int main() {
  vector<int> ratios;
  ratios.push_back(4);
  ratios.push_back(3);
  GenMap(1632803, ratios, "/work/02687/binren/graph/code/input/pokec/pokec-blk-partition-map-4-3.txt");
}
