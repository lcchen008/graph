#ifndef KERNEL_
#define KERNEL_

#include "scheduler.h"
#include "cpu_args.h"
#include <iostream>
#include <math.h>

#include "atomic.h"
#include "../user_funcs.h"
#include "sse_lib/vtypes.h"
#include <pthread.h>
#include "msg.h"

using namespace std;

template <class VertexValue, class EdgeValue, class MessageValue>
inline void emit_messages(msg_queue<MessageValue> *queues,
                          int &dst, MessageValue &msg,
                          scheduler<VertexValue, EdgeValue,
                          MessageValue> *s) {
#if defined (LOCK_BASED)
  // Directly insert to msg buffer.
  int local_dst = s->g->global_to_local_map[dst];

  if (local_dst == -1) {  // Remote msg.
    int remote_array_index = s->remote_index_array[dst];
#if defined (USE_REDUCTION) 
    if (!s->remote_msg_bufs[remote_array_index].index_) {
      s->remote_msg_bufs[remote_array_index].insert(msg);
    } else { // Reduce immediately.
      combine<VertexValue, EdgeValue, MessageValue>(
          &s->remote_msg_bufs[remote_array_index][0], msg);
    }
#else  // USE_REDUCTION
    s->remote_msg_bufs[remote_array_index].insert(msg);
#endif  // USE_REDUCTION
  } else {  // Local msg.
#ifndef SCALAR
    dst = local_dst;
    auto*& index_map = s->index_map;
    int new_dst = index_map[dst];
    int dst_buf = s->get_dst_buf(new_dst);
    if (s->index_array[new_dst] == -1) {
      get_lock(&s->msg_buf[dst_buf].horizontal_mutex);
      if (s->index_array[new_dst] == -1) {
        s->index_array[new_dst] = (s->msg_buf[dst_buf].horizontal_index)++;
      }
      release_lock(&s->msg_buf[dst_buf].horizontal_mutex);
    }
    s->msg_buf[dst_buf].insert(s->index_array[new_dst], msg);
#else  // SCALAR, directly insert to the buffer.
    s->msg_buf[local_dst].insert(msg); 
#endif  // SCALAR
  }

#else  // LOCK_BASED
  // Insert to the correct msg queue.
  int mod = dst % num_queues;
  while (!queues[mod].push_front(dst, msg));
#endif  // LOCK_BASED
}

template <class VertexValue, class EdgeValue, class MessageValue>
inline void* compute(void* arg) {
  cpu_args<VertexValue, EdgeValue, MessageValue>* ca = (cpu_args<VertexValue, EdgeValue, MessageValue> *)arg;  
  scheduler<VertexValue, EdgeValue, MessageValue>* s = ca->scheduler;
  int tid = ca->tid;
  msg_queue<MessageValue> &my_queues = s->mq[num_queues*tid];

  graph<VertexValue, EdgeValue> *g = s->g;	
  auto& active = s->active;
  auto& vertex_offset = s->vertex_offset;
  size_t num_vertices = g->num_vertices;
  size_t vertex_index;
  msg_queue<MessageValue> *queues = s->mq + tid*num_queues;	

  while (1) {
    vertex_index = vertex_offset.fetch_add(vertex_block_size,
                                           std::memory_order_relaxed);
    if (vertex_index >= num_vertices) {
      break;
    }

    for (size_t i = vertex_index; i < vertex_index + vertex_block_size &&
        i < num_vertices; ++i) {
      if (g->status[i].load()) {
        generate_messages(queues, i, g, s);		
      }
    }
  }

  // Notify the mover that message generation is done.
  active[tid].store(0);
  return (void *)0;
}

#endif
