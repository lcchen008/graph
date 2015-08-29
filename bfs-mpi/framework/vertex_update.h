#ifndef VERTEX_UPDATE_H_
#define VERTEX_UPDATE_H_

#include <pthread.h>

#include "../user_funcs.h"
#include "cpu_args.h"
#include "msg.h"
#include "scheduler.h"

template <class VertexValue, class EdgeValue, class MessageValue>
inline void* update(void* arg) {
	cpu_args<VertexValue, EdgeValue, MessageValue> *ca =
	(cpu_args<VertexValue, EdgeValue, MessageValue>*)arg;
	scheduler<VertexValue, EdgeValue, MessageValue> *s = ca->scheduler;
	int tid = ca->tid;
	graph<VertexValue, EdgeValue> *g = s->g;
	auto& vertex_offset = s->vertex_update_offset;
	vector<size_t> &index_array = s->index_array;	
	auto& in_degree = s->g->in_degree;
	auto& num_vertices = g->num_vertices;
  auto* buf = s->msg_buf;
	int vertex_index;

	while(1) {
		vertex_index = vertex_offset.fetch_add(update_vertex_block_size,
                                           std::memory_order_relaxed);
		if(vertex_index >= num_vertices) {
			break;
    }

		for(int i = vertex_index;
        i < vertex_index + update_vertex_block_size && i < num_vertices; ++i) {
			if(index_array[i] != -1) {  // At least one message is inserted in column i.
				int buffer_id = s->get_dst_buf(i);   	
				int array_id = index_array[i] / (vertices_per_array);  
				int col = index_array[i] % (vertices_per_array);
				MessageValue& m = buf[buffer_id].msg_arrays[array_id][0][col]; 
				update_vertex<VertexValue, EdgeValue, MessageValue>(m, g, in_degree[i].vid, s);
			}
		}
	}

	return (void *)0;
}

#endif
