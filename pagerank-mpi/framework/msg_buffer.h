#ifndef MSG_BUF_H
#define MSG_BUF_H
#include "vmsg_array.h"
#include "conf.h"
#include <iostream>
#include <atomic>

#include "atomic.h"

#if defined (LOCK_BASED)
#include <mutex>
#endif

using namespace std;

template <class VMessageValue, class MessageValue>
class msg_buffer {
	public:
    // Number of vertices in each vector.
    // Each msg buffer has a few vectors.
		int width;

		// Indicates how many columns have been taken in the horizontal direction.
		atomic<int> horizontal_index;

		vector<vmsg_array<VMessageValue, MessageValue>,
			aligned_allocator<vmsg_array<VMessageValue, MessageValue>, vlen> > msg_arrays;

#if defined (LOCK_BASED)
    vector<int> vertical_mutices;
    int horizontal_mutex;
#endif

		// Used for column allocation.
    int allocate_column() {
      return horizontal_index.fetch_add(1, std::memory_order_relaxed); 
    }

		msg_buffer() {
			width = vlen/sizeof(MessageValue);
			msg_arrays.resize(num_vmsg_array);
			horizontal_index = 0;
#if defined (LOCK_BASED)
      vertical_mutices.resize(vertices_per_buffer, 0);
      horizontal_mutex = 0;
#endif
		}

		void set_length(size_t length) {
			for(int i = 0; i < num_vmsg_array; i++) {
				msg_arrays[i].resize(length);
			}
		}

		void insert(size_t column_idx, MessageValue &msg) {
			size_t array_idx = column_idx/width; 
			int in_idx = column_idx%width;
#if defined (LOCK_BASED)
      get_lock(&vertical_mutices[column_idx]);
#endif
			msg_arrays[array_idx].insert(in_idx, msg);
#if defined (LOCK_BASED)
      release_lock(&vertical_mutices[column_idx]);
#endif
		}

		void re_init() {
			horizontal_index = 0;	
			for(size_t i = 0; i < msg_arrays.size(); i++) {
				msg_arrays[i].re_init();	
			}
		}

    int total_msg() {
      int total = 0;
			for(size_t i = 0; i < msg_arrays.size(); i++) {
        total += msg_arrays[i].total_msg();
      }
      return total;
    }
};

#endif
