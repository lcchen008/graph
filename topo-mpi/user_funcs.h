#ifndef USER_FUNCS
#define USER_FUNCS

#include <float.h>
#include <math.h>

#include "framework/sse_lib/vtypes.h"
#include "framework/kernel.h"

//Scalar send
template <class VertexValue, class EdgeValue, class MessageValue>
inline void generate_messages(msg_queue<MessageValue>* queues,
                              size_t vertex_id,
                              graph<VertexValue, EdgeValue>* g,
                              scheduler<VertexValue, EdgeValue, MessageValue>* s) {

  float msg = 1;
  for(size_t i = g->vertices[vertex_id]; i < g->vertices[vertex_id + 1]; i++) {
    emit_messages(queues, g->edges[i], msg, s);
  }
  g->status[vertex_id]=0;
}

//SIMD compute
template <class VMessageValue, class MessageValue>
inline void process_messages(vmsg_array<VMessageValue, MessageValue>& vmsgs) {
  for (int i = 0; i < vmsgs.size(); ++i) {
    vmsgs[0] += vmsgs[i];
  }
}

template <class VertexValue, class EdgeValue, class MessageValue>
inline void update_vertex(MessageValue& msg, graph<VertexValue, EdgeValue>* g,
                          size_t vertex_id,
                          scheduler<VertexValue, EdgeValue, MessageValue>* s) {
  g->vertex_value[vertex_id] -= msg;
  if (g->vertex_value[vertex_id] == 0) {
    g->status[vertex_id] = 1;
  }
}

// Used for reducing remote messages before sending.
template <class VertexValue,
class EdgeValue,
class MessageValue>
inline void combine(MessageValue* dst, MessageValue& msg) {
  *dst += msg;
}

#endif
