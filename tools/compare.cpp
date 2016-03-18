#include <iostream>
#include <unordered_set>
#include <fstream>
using namespace std;

int main() {
  ifstream test_is("../test");
  ifstream test1_is("../test1");
  unordered_set<int> test_set;
  unordered_set<int> test1_set;

  int tmp;

  while(test1_is >> tmp) {
    test1_set.insert(tmp);
  }

  while(test_is >> tmp) {
    if (test1_set.find(tmp) == test1_set.end()) {
      cout << tmp << " not found..." << endl;
    }
    test_set.insert(tmp);
  }
}
