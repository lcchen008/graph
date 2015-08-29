// Message array storing messages as aligned vectors.

#ifndef VMSG_ARRAY_H_
#define VMSG_ARRAY_H_

#include <vector>
#include "aligned_allocator.hpp"
#include "conf.h"
#include "sse_lib/vtypes.h"
#include <iostream>
using namespace std;

template <class VMessageValue, class MessageValue>
class vmsg_array {
 public: 
  vint32 each_idx;
  std::vector<VMessageValue, aligned_allocator<VMessageValue, vlen> > buf;
  int width;
  int size_;
  int min_size_;

  vmsg_array() {
#ifdef MSG_FLOAT
    width = vlen/sizeof(float);
#elif defined MSG_INT
    width = vlen/sizeof(int);
#elif defined MSG_DOUBLE
    width = vlen/sizeof(double);
#endif
    each_idx = 0;
    size_ = 0;
  }

  void insert(int &column_idx, MessageValue &msg) {
    int line_num = each_idx[column_idx];
    
    // if(line_num > 0 || buf.size() == line_num) {
    //   return;
    // }

    buf[line_num][column_idx] = msg;
    each_idx[column_idx]++;
  }

  void resize(size_t length) {
    buf.resize(length);	
  }

  ~vmsg_array() {
  }

  int size() {
    return size_;
  }

  int min_size() {
    return min_size_;
  }

  void compute_size() {
#ifdef MSG_FLOAT
    width = vlen/sizeof(float);
#elif defined MSG_INT
    width = vlen/sizeof(int);
#elif defined MSG_DOUBLE
    width = vlen/sizeof(double);
#endif
    size_ = each_idx[0];
    min_size_ = each_idx[0];
    for(int i = 1; i < width; i++) {
      size_ = max(size_, each_idx[i]);	
      min_size_ = min(min_size_, each_idx[i]);
    }
  }

  int total_msg() {
    int total = 0;
    for (int i = 0; i < width; ++i) {
      total += each_idx[i];
    }
    return total;
  }

  void re_init() {
#ifdef MSG_FLOAT
    width = vlen/sizeof(float);
#elif defined MSG_INT
    width = vlen/sizeof(int);
#elif defined MSG_DOUBLE
    width = vlen/sizeof(double);
#endif
    each_idx = 0;
    size_ = 0;
  }

  VMessageValue & operator[](int idx) {
    return buf[idx];
  }
}; 

#endif
