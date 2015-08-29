#ifndef MSG_REDUCTION_H_
#define MSG_REDUCTION_H_

#include "../user_funcs.h"
#include "scheduler.h"
#include "cpu_args.h"
#include <iostream>
#include <math.h>
#include "msg_buffer.h"
#include "sse_lib/vtypes.h"
#include <pthread.h>
#include "msg.h"
using namespace std;

// A kernel let is used by one core.
template <class VertexValue, class EdgeValue, class MessageValue>
inline void *reduce(void *arg) {
	cpu_args<VertexValue, EdgeValue, MessageValue> *ca = (cpu_args<VertexValue, EdgeValue, MessageValue> *)arg;  
	scheduler<VertexValue, EdgeValue, MessageValue> *s = ca -> scheduler;
	int tid = ca -> tid;
	msg_queue<MessageValue> &my_queues = s->mq[num_queues * tid];
  auto* msg_buf = s->msg_buf;
	auto& buffer_offset = s->buffer_offset;

	// Process msg buffers. Reduce the messages in SIMD way.	
	size_t num_buffers = s->num_buffers;
	size_t buffer_index;
	
#ifndef SCALAR
	while(1) {
		// Get a buffer block.
    buffer_index = buffer_offset.fetch_add(buffer_block_size,
                                           std::memory_order_relaxed);
		if(buffer_index >= num_buffers) {
			break;
    }

		for(size_t i = buffer_index;
        i < buffer_index + buffer_block_size && i < num_buffers; ++i) {
      auto& buf = msg_buf[i];
			size_t num_vector_arrays = ceil((double)(
              buf.horizontal_index)/(vlen/sizeof(MessageValue)));
			for(int j = 0; j < num_vector_arrays; ++j) {
        auto& array = buf.msg_arrays[j];
				if(array.size()) {

// Blank filling for MIC. Use mask support of MIC.
#if defined (MIC)
#if defined (BUFFER_INIT_VAL)
          // Preprocesses blank elements into BUFFER_INIT_VAL.
          for (int m = array.min_size();
               m < array.size(); ++m) {
            mask set_mask = (array.each_idx < (m + 1));
            array[m].set(set_mask, BUFFER_INIT_VAL);
          }
#endif  // BUFFER_INIT_VAL

// Blank filling for CPU. Use scalar way.
#else  // MIC
#ifdef BUFFER_INIT_VAL
          for (int m = array.min_size();
               m < array.size(); ++m) {
            for (int n = 0; n < array.width; ++n) {
              if (array.each_idx[n] < (m + 1)) {
                array[m][n] = BUFFER_INIT_VAL;
              }
            }
          }
#endif  // BUFFER_INIT_VAL
#endif  // MIC

          // Invoke user defined msg processing function to process.
					process_messages(array, s);
				}
      }
		}
	}

#else  // SCALAR
  while (1) {
    // Get a buffer block.
    buffer_index = buffer_offset.fetch_add(buffer_block_size,
                                           std::memory_order_relaxed);
		if(buffer_index >= num_buffers) {
			break;
    }

    for(size_t i = buffer_index;
        i < buffer_index + buffer_block_size && i < num_buffers; ++i) {
      if (msg_buf[i].size()) {
        process_messages_scalar<VertexValue, EdgeValue, MessageValue>(msg_buf[i].buf, s->g, i);
      }
    }
  }
#endif  // SCALAR

	return (void *)0;
}

#endif
