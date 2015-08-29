#ifndef STATUS_COUNT_H_
#define STATUS_COUNT_H_

#include "scheduler.h"
#include "cpu_args.h"
#include <pthread.h>

template <class VertexValue, class EdgeValue, class MessageValue>
inline void* status_count(void* arg) {
  cpu_args<VertexValue, EdgeValue, MessageValue> *ca =
	(cpu_args<VertexValue, EdgeValue, MessageValue>*)arg;
	scheduler<VertexValue, EdgeValue, MessageValue> *s = ca -> scheduler;
	int tid = ca -> tid;
  auto& status_count_array = s->status_count_array;

  // The graph. 
  const auto* g = s->g;
  int count = 0;
  for(size_t i = tid; i < g->num_vertices; i += status_count_thread) {
    count += g->status[i];
  }

  status_count_array[tid] = count;
  return (void *)0;
}

#endif /* ACTIVE_COUNT_H_ */
