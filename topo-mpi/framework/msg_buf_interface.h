#ifndef MSG_BUF_INTERFACE_H
#define MSG_BUF_INTERFACE_H 

#include "scheduler.h" 

template <class VertexValue,
          class EdgeValue,
          class MessageValue>
inline void insert_to_buffer(
    scheduler<VertexValue, EdgeValue, MessageValue>*& s,
    int& dst, MessageValue& value) {
  int local_dst = s->g->global_to_local_map[dst];  
  if (local_dst == -1) {
    s->remote_msg_bufs[s->remote_index_array[dst]].insert(value);
  } else {
    // Redirect: new_dst is in a new position after sorting.
    int new_dst = s->index_map[local_dst];
    // dst_buf is the destination buffer for this message.
    int dst_buf = s->get_dst_buf(new_dst);
    if(s->index_array[new_dst] == -1) {
      s->index_array[new_dst] = s->msg_buf[dst_buf].allocate_column();
    } 
    s->msg_buf[dst_buf].insert(s->index_array[new_dst], value);	
  }
}

#endif /* MSG_BUF_INTERFACE_H */
