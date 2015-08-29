// The mover thread call back function is defined here.

#ifndef MOVER_H_
#define MOVER_H_

#include "scheduler.h"
#include "cpu_args.h"
#include "msg_buf_interface.h"
#include "conf.h"
#include <vector>
#include <iostream>
#include <pthread.h>
#include "atomic.h"
using namespace std;

template <class VertexValue, class EdgeValue, class MessageValue>
void *move(void *arg) {
	cpu_args<VertexValue, EdgeValue, MessageValue> *ca =
      (cpu_args<VertexValue, EdgeValue, MessageValue> *)arg;  
	scheduler<VertexValue, EdgeValue, MessageValue> *s = ca -> scheduler;

	// Mover id.
	int tid = ca -> tid;
	msg_queue<MessageValue>* queues = s -> mq; 
	auto& active = s->active;	
	vector<bool> local_active(num_working_threads, 1);

	// Counter of total number of threads that are still generating.
	int active_count = num_working_threads;
	msg_queue<MessageValue>* cur_queue;
	msg<MessageValue> v;
  msg_queue<MessageValue>* mq = queues + tid;

	while(active_count) {
		for (int i = 0; i < num_working_threads; ++i) {
			if(!local_active[i])
				continue;
			cur_queue = mq + i * num_queues;
			// Try to read a block from the queue.
			size_t read = 0;
			while (read < msgs_moved && cur_queue -> pop_back(&v)) {
				++read;
        insert_to_buffer(s, v.dst, v.value); 
			}

			if (!(active[i]).load()) {
				// Read possible remaining msgs.
				while (cur_queue->pop_back(&v)) {
          // This is a message being sent to remote worker.
          insert_to_buffer(s, v.dst, v.value); 
        }
				local_active[i] = 0;
				--active_count;	
			}
		}
	}

	return (void *)0;
}

#endif
