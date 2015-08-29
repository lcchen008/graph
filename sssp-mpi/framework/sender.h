// Thread task for sending remote messages
// to remote process.

#ifndef SENDER_H
#define SENDER_H 

template <class VertexValue, class EdgeValue, class MessageValue>
inline void* sender(void* arg) {
  cpu_args<VertexValue, EdgeValue, MessageValue>* ca = (cpu_args<VertexValue, EdgeValue, MessageValue> *)arg;  
  scheduler<VertexValue, EdgeValue, MessageValue>* s = ca->scheduler;
  int tid = ca->tid;
  const auto& remote_msg_bufs = s->remote_msg_bufs;
  auto& sender_offset = s->sender_offset;
  int vertex_index;
  int total_size = remote_msg_bufs.size();
  MPI_Request request;

  while (1) {
    vertex_index = sender_offset.fetch_add(sender_block_size);
    if (vertex_index >= total_size) {
      break;
    }

    for (int i = vertex_index; i < vertex_index + sender_block_size &&
         i < total_size; ++i) {
      int size = remote_msg_bufs[i].size();
      if (size) {
        size = 1;
        // Send the whole buffer for the current vertex.
        // Here we use unblocking send.
        MPI_Isend((void *)&remote_msg_bufs[i][0], sizeof(MessageValue)*size,
                  MPI_BYTE, s->peer_rank, remote_msg_bufs[i].dst(),
                  MPI_COMM_WORLD, &request);
      }
    }
  } 

  return (void *) 0;
}

#endif /* SENDER_H */
