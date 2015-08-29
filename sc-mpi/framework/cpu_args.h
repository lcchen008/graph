#ifndef CPU_ARGS_H_
#define CPU_ARGS_H_
#include "scheduler.h"

template <class VertexValue, class EdgeValue, class MessageValue>
struct cpu_args {
  int tid;
  scheduler<VertexValue, EdgeValue, MessageValue> *scheduler;
};

struct test_args {
  int count;
  test_args():count(0) {}
};

#endif
