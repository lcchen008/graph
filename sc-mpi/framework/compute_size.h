#ifndef COMPUTE_SIZE_H_
#define COMPUTE_SIZE_H_

#include "scheduler.h"
#include "cpu_args.h"
#include <iostream>
#include <math.h>

#include "sse_lib/vtypes.h"
#include <pthread.h>
#include "msg.h"

using namespace std;

template <class VertexValue, class EdgeValue, class MessageValue>
void *compute_size(void *arg) {
  cpu_args<VertexValue, EdgeValue, MessageValue> *ca
      = (cpu_args<VertexValue, EdgeValue, MessageValue> *)arg;  
  scheduler<VertexValue, EdgeValue, MessageValue> *s = ca -> scheduler;
  int tid = ca -> tid;
  auto* msg_buf = s->msg_buf;

  for(int i = tid; i < s->num_buffers; i+= nthread) {
    for(int j = 0; j < num_vmsg_array; j++) {
      msg_buf[i].msg_arrays[j].compute_size();
    }
  }

  return (void *)0;
}

#endif  // COMPUTE_SIZE_H_
