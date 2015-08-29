#ifndef USER_FUNCS_
#define USER_FUNCS_

#include <float.h>
#include <math.h>
#include <unordered_set>

#include "framework/sse_lib/vtypes.h"
#include "framework/kernel.h"
#include "semi.h"

template <class VertexValue,
          class EdgeValue,
          class MessageValue>
inline void generate_messages(msg_queue<MessageValue>* queues,
                              size_t vertex_id,
                              graph<VertexValue, EdgeValue>* g,
                              scheduler<VertexValue, EdgeValue, MessageValue>* s) {
	const int n = g->vertices[vertex_id + 1] - g->vertices[vertex_id]; 			
	//we only send the 5 best clusters...
	msg_cluster_list msg;

	for(int i = 0; i < CLUSTER_PER_MSG &&
      i < g -> vertex_value[vertex_id].size(); ++i) {
		msg.insert(g->vertex_value[vertex_id][i]);	
	}
		
	for(int i = g->vertices[vertex_id];
      i < g->vertices[vertex_id + 1]; ++i) {
		emit_messages(queues, g->edges[i], msg, s);
	}
}

bool contains(cluster &c, int vertex_id)
{
	for(int i = 0; i < c.size(); i++)	
	{
		if(c[i] == vertex_id)		
			return true;
	}

	return false;
}

float compute_score(graph<cluster_list, float> *g, cluster &c) {
	float Ic = 0;
	float Bc = 0;
	int Vc = c.size();
	
	for (size_t i = 0; i < c.size(); i++) {
		for (size_t j = g -> vertices[i]; j < g -> vertices[i+1]; j++)	{
			size_t dst = g -> edges[j];	
			float weight = g -> edge_value[j];
			//inner edge
			if (contains(c, dst)) {
				Ic += weight;		
      } else {
				Bc += weight;
      }
		}
	}

	if (Vc == 0 || Vc == 1) {
		return 0;
  } else {
		return (Ic -fB * Bc)/(Vc * (Vc - 1))*2;	
  }
}

bool compare_score(const cluster &c1, const cluster &c2) {
	return c1.score > c2.score;	
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

// Scalar reduction method.
template <class VertexValue, class EdgeValue, class MessageValue>
inline void process_messages_scalar(vector<MessageValue>& msgs,
                                    graph<VertexValue, EdgeValue>* g,
                                    size_t vertex_id) {
	cluster_list &my_list = g -> vertex_value[vertex_id];	
	cluster to_sort[SORT_MAX];
	int sort_idx = 0;
	for (; sort_idx < SORT_MAX && sort_idx < my_list.size(); sort_idx++) {
		to_sort[sort_idx] = my_list[sort_idx];	
	}	

	for (int i = 0; i < msgs.size(); i++) {
		msg_cluster_list &cl = msgs[i];
		for (int j = 0; j < cl.size(); j++) {
			cluster &c = cl[j];	
			if (!contains(c, vertex_id)) {
				if (c.size() < VMAX) {
					c.insert(vertex_id);
					if (sort_idx < SORT_MAX) {
						to_sort[sort_idx] = c;
						sort_idx++;
					}
				}
			}
		}
	}

	//compute score for each cluster in to_sort
	for (int i = 0; i < sort_idx; i++) {
		to_sort[i].score = compute_score(g, to_sort[i]);
	}

	//sort
	if (sort_idx > 1)
		sort(&to_sort[0], &to_sort[0] + sort_idx - 1, compare_score);		

	//set value
	my_list.clear();	

	for (int i = 0; i < sort_idx && i < CMAX; i++)	{
		my_list.insert(to_sort[i]);			
	}
}

template <class VertexValue,
          class EdgeValue,
          class MessageValue>
inline void update_vertex(MessageValue& msg,
                          graph<VertexValue, EdgeValue>* g,
                          size_t vertex_id,
                          scheduler<VertexValue, EdgeValue, MessageValue>* s) {
  // if(msg < g->vertex_value[vertex_id]) {
  //   g->status[vertex_id] = 1;	
  //   g->vertex_value[vertex_id] = msg;
  // } else {
  //   g->status[vertex_id] = 0;	
  // }
}

// Used for reducing remote messages before sending.
template <class VertexValue,
          class EdgeValue,
          class MessageValue>
inline void combine(MessageValue* dst, MessageValue& msg) {
  // *dst = min(*dst, msg);
}

#endif  // USER_FUNCS_
