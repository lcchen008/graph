#ifndef REMOTE_INSERTER_H
#define REMOTE_INSERTER_H 

#include "msg_buf_interface.h"

template <class VertexValue, class EdgeValue, class MessageValue>
inline void* inserter(void* arg) {
  cpu_args<VertexValue, EdgeValue, MessageValue>* ca = (cpu_args<VertexValue, EdgeValue, MessageValue> *)arg;  
  scheduler<VertexValue, EdgeValue, MessageValue>* s = ca->scheduler;
  int tid = ca->tid;
  const auto* remote_msg_block_info = s->remote_msg_block_info;
  auto& inserter_offset = s->inserter_offset;
  int vertex_index;
  int total_size = remote_msg_block_info->size();

  while (1) {
    vertex_index = inserter_offset.fetch_add(inserter_block_size);

    if (vertex_index >= total_size) {
      break;
    }

    for (int i =  vertex_index; i < vertex_index + inserter_block_size &&
         i < total_size; ++i) {
      // Save msgs to local msg_buf.
      int dst = (*remote_msg_block_info)[i].tag();
      MessageValue value = (*remote_msg_block_info)[i].value();
      insert_to_buffer(s, dst, value); 
    }
  }

  return (void *) 0;
}


#endif /* REMOTE_INSERTER_H */
