#ifndef RECEIVER_H
#define RECEIVER_H 

#include "msg_buf_interface.h"

template <class VertexValue, class EdgeValue, class MessageValue>
inline void* receiver(void* arg) {
  cpu_args<VertexValue, EdgeValue, MessageValue>* ca = (cpu_args<VertexValue, EdgeValue, MessageValue> *)arg;  
  scheduler<VertexValue, EdgeValue, MessageValue>* s = ca->scheduler;
  int tid = ca->tid;
  const auto* remote_msg_block_info = s->remote_msg_block_info;
  auto& receiver_offset = s->receiver_offset;
  int vertex_index;
  int total_size = remote_msg_block_info->size();
  MPI_Status status;
  MessageValue* receive_buf = s->receive_buffer[tid];

  while (1) {
    vertex_index = receiver_offset.fetch_add(receiver_block_size);
    if (vertex_index >= total_size) {
      break;
    }

    for (int i =  vertex_index; i < vertex_index + receiver_block_size &&
         i < total_size; ++i) {
      // Receive remote msgs.
      MPI_Recv(receive_buf,
               (*remote_msg_block_info)[i].size()*sizeof(MessageValue),
               MPI_BYTE, s->peer_rank, (*remote_msg_block_info)[i].tag(),
               MPI_COMM_WORLD, &status);

      // Save msgs to local msg_buf.
      int size = (*remote_msg_block_info)[i].size();
      int dst = (*remote_msg_block_info)[i].tag();
      for (int j = 0; j < size; ++j) {
        // if (tid == 0)
        // cout << "Dst: " << dst << " Value: " << receive_buf[j] << endl;
        insert_to_buffer(s, dst, receive_buf[j]); 
      }
    }
  }

  return (void *) 0;
}

#endif /* RECEIVER_H */
