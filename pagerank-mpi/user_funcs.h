#ifndef USER_FUNCS_
#define USER_FUNCS_

#include <float.h>
#include <math.h>
#include <unordered_set>

#include "framework/sse_lib/vtypes.h"
#include "framework/kernel.h"

template <class VertexValue,
          class EdgeValue,
          class MessageValue>
inline void generate_messages(msg_queue<MessageValue>* queues,
                              size_t vertex_id,
                              graph<VertexValue, EdgeValue>* g,
                              scheduler<VertexValue, EdgeValue, MessageValue>* s) {
  const int n = g->vertices[vertex_id + 1] - g->vertices[vertex_id];
  float vertex_value = g->vertex_value[vertex_id];
  float msg = vertex_value / n;

  for(size_t i = g->vertices[vertex_id]; i < g->vertices[vertex_id +1]; ++i) {
    emit_messages(queues, g->edges[i], msg, s);
  }
}

template <class VMessageValue,
          class VertexValue,
          class EdgeValue,
          class MessageValue>
inline void process_messages(vmsg_array<VMessageValue, MessageValue>& vmsgs,
                             scheduler<VertexValue, EdgeValue, MessageValue>* s) {
  for (int i = 0; i < vmsgs.size(); ++i) {
    vmsgs[0] += vmsgs[i];
  }
}

template <class VertexValue,
          class EdgeValue,
          class MessageValue>
inline void update_vertex(MessageValue& msg,
                          graph<VertexValue, EdgeValue>* g,
                          size_t vertex_id,
                          scheduler<VertexValue, EdgeValue, MessageValue>* s) {
  g->vertex_value[vertex_id] = msg;
}

// Used for reducing remote messages before sending.
template <class VertexValue,
          class EdgeValue,
          class MessageValue>
inline void combine(MessageValue* dst, MessageValue& msg) {
  *dst += msg;
}

#endif  // USER_FUNCS_
