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
  float my_min = g->vertex_value[vertex_id];
  float weight; 
  float msg;

  for(size_t i = g->vertices[vertex_id]; i < g->vertices[vertex_id + 1]; ++i) {
    weight = g -> edge_value[i];
    msg = my_min + weight; 
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
  vfloat m = FLT_MAX;
  for(int i = 0; i < vmsgs.size(); ++i)	{
    m = vmin(vmsgs[i], m);
  }
  vmsgs[0] = m;
}

template <class VertexValue,
          class EdgeValue,
          class MessageValue>
inline void update_vertex(MessageValue& msg,
                          graph<VertexValue, EdgeValue>* g,
                          size_t vertex_id,
                          scheduler<VertexValue, EdgeValue, MessageValue>* s) {
  if(msg < g->vertex_value[vertex_id]) {
    g->status[vertex_id] = 1;	
    g->vertex_value[vertex_id] = msg;
  } else {
    g->status[vertex_id] = 0;	
  }
}

// Used for reducing remote messages before sending.
template <class VertexValue,
          class EdgeValue,
          class MessageValue>
inline void combine(MessageValue* dst, MessageValue& msg) {
  *dst = min(*dst, msg);
}

#endif  // USER_FUNCS_
