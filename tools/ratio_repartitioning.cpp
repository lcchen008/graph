// Hybrid partitioning for graph.
// This tool is used to round-robinly assign the partions
// from metis to devices according to ratio.
// The input is the partitioning result from metis and
// the output is the round-robin assignment result. 

#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

// input_filename: partitioning result from metis.
// output_filename: round-robin assignment result.
void Convert(vector<int>& ratios,
             const string& input_filename,
             const string& output_filename) {
  int total_ratio = 0;
  for (int i = 0; i < ratios.size(); ++i) {
    total_ratio += ratios[i];
  }

  cout << "Total ratios: " << total_ratio << endl;

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

  ifstream input(input_filename.c_str());
  ofstream output(output_filename.c_str());

  int part;
  while (input >> part) {
    output << local_map[part % total_ratio] << endl;
  }
}

int main() {
  vector<int> ratios;
  ratios.push_back(4);
  ratios.push_back(3);
  Convert(ratios,
          // "../code/input/livejournal/soc-livejournal-undirected.txt.part.256",
          // "../code/input/livejournal/soc-livejournal-2-3-metis");

          "../code/input/pokec/soc-metis-undirected.txt.part.256",
          "../code/input/pokec/soc-hybrid-partition-map-4-3.txt");

          // "../code/input/com/com-undirected.part.256",
          // "../code/input/com/com-hybrid-2-1-metis");

          // "../code/input/dag/dag-38394-undirected.part.512",
          // "../code/input/dag/dag-hybrid-1-4-metis");
}
