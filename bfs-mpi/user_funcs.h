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
  float msg = g->vertex_value[vertex_id] + 1;
  const auto* gtlm = s->g->global_to_local_map; 
  for(size_t i = g->vertices[vertex_id]; i < g->vertices[vertex_id +1]; ++i) {
    int local_id = gtlm[g->edges[i]];
    if (local_id != -1 && msg >= g->vertex_value[local_id])
      continue;
    emit_messages(queues, g->edges[i], msg, s);
  }
  g->status[vertex_id] = 0;
}

template <class VMessageValue,
          class VertexValue,
          class EdgeValue,
          class MessageValue>
inline void process_messages(vmsg_array<VMessageValue, MessageValue>& vmsgs,
                             scheduler<VertexValue, EdgeValue, MessageValue>* s) {
}

template <class VertexValue,
          class EdgeValue,
          class MessageValue>
inline void update_vertex(MessageValue& msg,
                          graph<VertexValue, EdgeValue>* g,
                          size_t vertex_id,
                          scheduler<VertexValue, EdgeValue, MessageValue>* s) {
  if (msg < g->vertex_value[vertex_id]) {
    g->status[vertex_id] = 1;	
    g->vertex_value[vertex_id] = msg;
  }
}

// Used for reducing remote messages before sending.
template <class VertexValue,
          class EdgeValue,
          class MessageValue>
inline void combine(MessageValue* dst, MessageValue& msg) {

}

#endif  // USER_FUNCS_
