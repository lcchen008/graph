#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <iostream>
#include <limits.h>
#include <math.h>
#include <vector>

#include "conf.h"
#include "graph.h"
#include "msg_buffer.h"
#include "remote_msg_buffer.h"
#include "msg_block_info.h"
#include "msg_queue.h"
#include "sse_lib/vtypes.h"
#include "util.h"
#include <boost/threadpool.hpp>
using namespace std;

template <class VertexValue, class EdgeValue, class MessageValue>
class scheduler {
 private:
  void init();
  int superstep_; 
 public:
  // The graph to be processed.
  graph<VertexValue, EdgeValue> *g;	
  int my_rank;
  int peer_rank;
  int num_buffers;

  // Message buffers.
#ifdef MSG_INT
  msg_buffer<vint, MessageValue>* msg_buf;
#elif defined MSG_FLOAT 
  msg_buffer<vfloat, MessageValue>* msg_buf;
#elif defined MSG_DOUBLE
  msg_buffer<vdouble, MessageValue>* msg_buf;
#endif

  // Remote message buffers.
  vector<remote_msg_buffer<MessageValue> > remote_msg_bufs;

  // Index_array is used for indexing the vmsg_array which
  // contains messages for a specific vertex.
  // The index is relative to a particular message buffer.
  // Initially, every index is -1.
  // The element values are filled at the time of allocating
  // a message column.
  vector<size_t> index_array;

  // This is used for indexing a remote message to the correct
  // remote send buffer. It maps from a global vid to a send
  // buffer row id.
  vector<size_t> remote_index_array;

  // Because vertices are sorted in in-degree.
  // The index_map maps the positions of before and after sorting.
  // Gened from in_degree.
  int *index_map;

  // Array for storing number of active vertices counted by each
  // counting thread.
  vector<size_t> status_count_array; 

  // Message queues for each thread to sequentially store messages.
  msg_queue<MessageValue> mq[num_queues * num_working_threads];

  // Receive buffer for remote messages.
  vector<MessageValue*> receive_buffer;
  
  // Working thread statuses.
  // 1 stands for still gnerating messages.
  // 0 stands for finished all its work.
  atomic<bool> active[num_working_threads];

  atomic<int> buffer_offset;
  atomic<int> vertex_offset;
  atomic<int> vertex_update_offset;
  atomic<int> sender_offset;
  atomic<int> receiver_offset;
  atomic<int> inserter_offset;
  
  atomic<int> total_write;
  atomic<int> total_read;

  int superstep() {
    return superstep_;
  }

  scheduler(graph<VertexValue, EdgeValue> *g, int my_rank);
  void start();
  inline int get_dst_buf(int dst_vertex);

  // Init for future iterations.
  void re_init() {
	  cout << "***********Re-initing...**************" << endl;
    // re-init buffers.
    int count = 0;
    for (size_t i = 0; i < num_buffers; i++) {
      count += msg_buf[i].horizontal_index;
      msg_buf[i].re_init();	
    }

    cout << "Buffer columns allocated: " << count << endl;

    // re-init remote buffers. 
    for (size_t i = 0; i < remote_msg_bufs.size(); ++i) {
      remote_msg_bufs[i].re_init();
    }

    // Init queues.
    for (size_t i = 0; i < num_working_threads * num_queues; i++) {
      mq[i].re_init();
    }

    // Reset index array.
    for (size_t i = 0; i < g -> num_vertices; ++i) {
      index_array[i] = -1;	
    }

    // Reset active array, i.e., active threads.
    for (int i = 0; i < num_working_threads; i++) {
      active[i].store(1);	
    }

    cout << "Total write: " << total_write << endl;
    cout << "Total read: " << total_read << endl;

    // Reset dynamic parallel processing offsets.
    buffer_offset = 0;
    vertex_offset = 0;
    vertex_update_offset = 0;
    sender_offset = 0;
    receiver_offset = 0;
    inserter_offset = 0;

    total_write = 0;
    total_read = 0;

	  cout << "***********Re-init done.**************" << endl;
  }

  vector<msg_block_info<MessageValue> >* remote_msg_block_info;

  // Reorder the vertices according to the in-degrees.
  void reorder();
  ~scheduler();
};

// Source file containing callbacks for different parallel kernels.
#include "kernel.h"
#include "mover.h"
#include "msg_reduction.h"
#include "compute_size.h"
#include "vertex_update.h"
#include "status_count.h"
#include "remote_inserter.h"
#include "cpu_args.h"
#include "sender.h"
#include "receiver.h"

template <class VertexValue, class EdgeValue, class MessageValue>
scheduler<VertexValue, EdgeValue, MessageValue>::
scheduler(graph<VertexValue, EdgeValue> *g, int my_rank) {
  // Get the graph from the input.
  this->my_rank = my_rank;
  this->peer_rank = my_rank ? 0 : 1;

  this -> g = g;	
  for(int i = 0; i < num_working_threads; i++) {
    active[i] = 1;	
  }

  receive_buffer.resize(msg_receiver_thread, 0);

  if (!g->num_vertices) {
    this->num_buffers = 0;
  } else {
    this->num_buffers = CEILING(g->num_vertices, vertices_per_buffer);
  }

#ifdef MSG_INT
  msg_buf = new msg_buffer<vint, MessageValue>[num_buffers];
#elif defined MSG_FLOAT 
  msg_buf = new msg_buffer<vfloat, MessageValue>[num_buffers];
#elif defined MSG_DOUBLE
  msg_buf = new msg_buffer<vdouble, MessageValue>[num_buffers];
#endif

  init();
  superstep_ = 0;
}

template <class VertexValue, class EdgeValue, class MessageValue>
inline int scheduler<VertexValue, EdgeValue, MessageValue>::get_dst_buf(int dst_vertex) {
  return dst_vertex / vertices_per_buffer;
}

template <class VertexValue, class EdgeValue, class MessageValue>
void scheduler<VertexValue, EdgeValue, MessageValue>::init() {
  // Partition the graph into parts along vertices.
  index_array.resize(g->num_vertices, -1);
  status_count_array.resize(status_count_thread, 0);
  
  int remain = g->num_vertices;
  int num_per_part = ceil((double)g->num_vertices/num_working_threads); 
  int index = 0;
  int start, end;
  buffer_offset = 0;
  vertex_offset = 0;
  vertex_update_offset = 0;
  sender_offset = 0;
  receiver_offset = 0;
  inserter_offset = 0;
  
  total_read = 0;
  total_write = 0;
  cout << "Reordering..." << endl;
  double before_reorder = rtclock();
  // Reorder according to in-degree.
  reorder();
  double after_reorder = rtclock();
  cout << "Reorder time: " << (after_reorder - before_reorder) << endl;

  // Allocate remote_index_array.
  remote_index_array.resize(g->global_num_vertices);

  // Allocate remote vertex in-degree buffer.
  // Also allocate remote msg buffer for storing
  // remote messages (for sending out).
  int total_remote_edges = 0;
  for (int i = 0; i < g->global_num_vertices; ++i) {
    int degree = g->remote_in_degree[i];
    if (degree) {
      total_remote_edges += degree;
      remote_index_array[i] = remote_msg_bufs.size();
      remote_msg_bufs.push_back(remote_msg_buffer<MessageValue>());
      remote_msg_bufs.back().dst_ = i;
      remote_msg_bufs.back().resize(degree);
    }
  }

  cout << "****** remote edges: ****** " << total_remote_edges << endl; 
}

template <class VertexValue, class EdgeValue, class MessageValue>
inline void scheduler<VertexValue, EdgeValue, MessageValue>::start() {
  pthread_t work_tid[num_working_threads];	
  pthread_t move_tid[num_moving_threads];	
  pthread_t compute_size_tid[compute_size_thread];
  pthread_t reduce_tid[reduce_thread];
  pthread_t update_tid[update_thread];
  pthread_t status_count_tid[status_count_thread];
  pthread_t msg_sender_tid[msg_sender_thread];
  pthread_t msg_receiver_tid[msg_receiver_thread];
  pthread_t msg_inserter_tid[msg_inserter_thread];
  cpu_args<VertexValue, EdgeValue, MessageValue> work_args[num_working_threads];
  cpu_args<VertexValue, EdgeValue, MessageValue> move_args[num_moving_threads];
  cpu_args<VertexValue, EdgeValue, MessageValue> reduce_args[reduce_thread];
  cpu_args<VertexValue, EdgeValue, MessageValue> update_args[update_thread];
  cpu_args<VertexValue, EdgeValue, MessageValue> compute_size_args[compute_size_thread];
  cpu_args<VertexValue, EdgeValue, MessageValue> status_count_args[status_count_thread];
  cpu_args<VertexValue, EdgeValue, MessageValue> msg_sender_args[msg_sender_thread];
  cpu_args<VertexValue, EdgeValue, MessageValue> msg_receiver_args[msg_sender_thread];
  cpu_args<VertexValue, EdgeValue, MessageValue> msg_inserter_args[msg_inserter_thread];

  double all = 0;
  double total_work = 0;
  double total_move = 0;
  double total_reduce = 0;
  double total_update = 0;
  double total_receive = 0;
  double total_insert = 0;
  int total_active = 0;

  for(int it = 0; it < num_iter; it++) {
    // Count number of active vertices. If the number of actice vertices
    // is 0, we will end the computation.
    cout << "Iter: " << it << endl;
    double before_status_count = rtclock();
    for(int i = 0; i < status_count_thread; i++) {
      status_count_args[i].tid = i;
      status_count_args[i].scheduler = this;
      pthread_create(&status_count_tid[i], NULL,
                     &status_count<VertexValue, EdgeValue, MessageValue>,
                     &status_count_args[i]);
    }

    for (const auto& thread : status_count_tid) {
      pthread_join(thread, NULL);
    }

    int total_active_vertices = 0;

    for (const auto& count : status_count_array) {
      total_active_vertices += count;
    }

    cout << "Rank: " << my_rank << " status count done." << endl;

    // Devices exchange status count with each other.
    int peer_active_count;
    MPI_Status status;

    MPI_Sendrecv(&total_active_vertices, 1, MPI_INT, peer_rank,
                 STATUS_TAG, &peer_active_count, 1, MPI_INT,
                 peer_rank, STATUS_TAG, MPI_COMM_WORLD, &status);
    cout << "After receive ..." << endl;

    double after_status_count = rtclock();
    total_active_vertices += peer_active_count;

    cout << endl;
    cout << "Iter: " << it << "..." << endl;
    cout << "Active vertex count: " << total_active_vertices << endl;
    total_active += total_active_vertices;

    if (!total_active_vertices) {
      cout << "No vertice is active, ending the computation :)" << endl;
      cout << endl;
	    break;
    }

    // ===================================================================

    double before_init = rtclock();
    re_init();
    double after_init = rtclock();

    // Message generation stage.
    // Start worker threads.
    for(int i = 0; i < num_working_threads; i++) {
      work_args[i].tid = i;
      work_args[i].scheduler = this;
      pthread_create(&work_tid[i], NULL, &compute<VertexValue,
                     EdgeValue, MessageValue>, &work_args[i]);
    }

#ifndef LOCK_BASED
    // Start mover threads if lock-based method is not used.
    for(int i = 0; i < num_moving_threads; i++) {
      move_args[i].tid = i;
      move_args[i].scheduler = this;
      pthread_create(&move_tid[i], NULL,
                     &move<VertexValue, EdgeValue, MessageValue>,
                     &move_args[i]);
    }
#endif

    double before_gen = rtclock();
    double before_work = rtclock();
    
    for(const auto& thread : work_tid) {
      pthread_join(thread, NULL);	
    }

    double after_work = rtclock();
    total_work += (after_work - before_work);

    cout << my_rank <<  " working threads finish." << endl;

    double before_move = rtclock();

#ifndef LOCK_BASED
    // Join mover threads if lock-based method is not used.
    for(const auto& thread : move_tid) {
      pthread_join(thread, NULL);	
    }
#endif

    double after_move = rtclock();
    double after_gen = rtclock();

    total_move += (after_move - before_move);
    cout << "Mover threads finish." << endl;

    cout << my_rank << " Gen time: " << (after_gen - before_gen) << endl;
    all += (after_gen - before_gen);

    // Remote msg exchange stage.
    // Prepare information for sending.
    int total_msg_blocks = 0;
    vector<msg_block_info<MessageValue> > my_remote_msg_block_info;
    for (int i = 0; i < remote_msg_bufs.size(); ++i) {
      if (remote_msg_bufs[i].size()) {
       //  my_remote_msg_block_info.emplace_back(remote_msg_bufs[i].size(),
       //                                        remote_msg_bufs[i].dst());
        my_remote_msg_block_info.emplace_back(1,
                                              remote_msg_bufs[i].dst(),
                                              remote_msg_bufs[i][0]);
        ++total_msg_blocks;
      }
    }

    int peer_msg_blocks;
    MPI_Sendrecv(&total_msg_blocks, 1, MPI_INT, peer_rank,
                 MSG_BLOCK_INFO_TAG, &peer_msg_blocks, 1, MPI_INT,
                 peer_rank, MSG_BLOCK_INFO_TAG , MPI_COMM_WORLD, &status);

    cout << my_rank << " total msg block: " << total_msg_blocks << endl;

    // Allocate receive space. The space will be used for receiving the msg block
    // info.
    vector<msg_block_info<MessageValue> > remote_msg_blocks(peer_msg_blocks);

    // Exchange remote msg block info.
    double before_receive = rtclock();
    MPI_Sendrecv(&my_remote_msg_block_info[0],
                 sizeof(msg_block_info<MessageValue>)*total_msg_blocks,
                 MPI_BYTE, peer_rank,
                 MSG_BLOCK_INFO_TAG, &remote_msg_blocks[0],
                 sizeof(msg_block_info<MessageValue>)*peer_msg_blocks, MPI_BYTE,
                 peer_rank, MSG_BLOCK_INFO_TAG , MPI_COMM_WORLD, &status);
    double after_receive = rtclock();

    total_receive += (after_receive - before_receive);

    cout << "Receive time: " << (after_receive - before_receive) << endl;

    // This pointer is set to the scheduler object, such that the inserter 
    // kernel is able to get the block info and conduct data receiving. 
    remote_msg_block_info = &remote_msg_blocks;

    // Start parallel remote thread inserters.
    for (int i = 0; i < msg_inserter_thread; ++i) {
      msg_inserter_args[i].tid = i;
      msg_inserter_args[i].scheduler = this;
      pthread_create(&msg_inserter_tid[i], NULL,
                     &inserter<VertexValue, EdgeValue, MessageValue>,
                     &msg_inserter_args[i]);
    }

    double before_insert = rtclock();
    // Join sender threads.
    for (const auto& thread : msg_inserter_tid) {
      pthread_join(thread, NULL);
    }
    double after_insert = rtclock();

    total_insert += (after_insert - before_insert);

    cout << "Insert time: " << (after_insert - before_insert) << endl;

    /*
    // Start remote msg sender threads.
    for (int i = 0; i < msg_sender_thread; ++i) {
      msg_sender_args[i].tid = i;
      msg_sender_args[i].scheduler = this;
      pthread_create(&msg_sender_tid[i], NULL,
                     &sender<VertexValue, EdgeValue, MessageValue>,
                     &msg_sender_args[i]);
    }

    // Join sender threads.
    for (const auto& thread : msg_sender_tid) {
      pthread_join(thread, NULL);	
    }

    cout << my_rank << " remote msg send done." << endl;

    // Start remote msg receiver threads.
    for (int i = 0; i < msg_receiver_thread; ++i) {
      msg_receiver_args[i].tid = i;
      msg_receiver_args[i].scheduler = this;
      pthread_create(&msg_receiver_tid[i], NULL,
                     &receiver<VertexValue, EdgeValue, MessageValue>,
                     &msg_receiver_args[i]);
    }

    double before_receive = rtclock();
    // Join receiver threads.
    for (const auto& thread : msg_receiver_tid) {
      pthread_join(thread, NULL);	
    }
    double after_receive = rtclock();

    total_receive += (after_receive - before_receive);
    cout << "Receive time: " << (after_receive - before_receive);

    */

    cout << my_rank << " remote msg receive done." << endl;

    // ======for testing purposes======
    // int total_msg = 0;
    // for (int i = 0; i < num_buffers; ++i) {
    //   total_msg += msg_buf[i].total_msg();
    // }
    // cout << "====> Total msg count: " << total_msg << endl;
    // ======for testing purposes======

    // Message reduction stage.
    // Before reduce, compute each vmsg array size.
    for(int i = 0; i < compute_size_thread; i++) {
      compute_size_args[i].tid = i;		
      compute_size_args[i].scheduler = this;
      pthread_create(&compute_size_tid[i], NULL,
                     &compute_size<VertexValue,
                     EdgeValue, MessageValue>,
                     &compute_size_args[i]);
    }

    for(const auto& thread : compute_size_tid) {
      pthread_join(thread, NULL);		
    }

    // Reduce.
    for(int i = 0; i < reduce_thread; i++) {
      reduce_args[i].tid = i;		
      reduce_args[i].scheduler = this;
      pthread_create(&reduce_tid[i], NULL,
                     &reduce<VertexValue, EdgeValue, MessageValue>,
                     &reduce_args[i]);
    }

    double before_reduce = rtclock();

    // Join all the reduction threads.
    for(const auto& thread : reduce_tid) {
      pthread_join(thread, NULL);		
    }

    double after_reduce = rtclock();

    total_reduce += (after_reduce - before_reduce);

    cout << "Reduce time: " << after_reduce - before_reduce << endl;

    all += (after_reduce - before_reduce);

    cout << "Messages reduce done." << endl;

    // Second, vertex update stage.
    for(int i = 0; i < update_thread; i++) {
      update_args[i].tid = i;	
      update_args[i].scheduler = this;
      pthread_create(&update_tid[i], NULL,
                     &update<VertexValue,
                     EdgeValue, MessageValue>,
                     &update_args[i]);
    }

    double before_update = rtclock();

    // Join all the update threads.
    for(int i = 0; i < update_thread; ++i) {
      pthread_join(update_tid[i], NULL);	
    }

    double after_update = rtclock();

    all += (after_update - before_update);
    total_update += (after_update - before_update);
    cout << "Update time: " << (after_update - before_update) << endl;
    ++superstep_;

    cout << endl;
  }

  cout << my_rank <<  " all time : " << all << endl;
  cout << "Total reduce time: " << total_reduce << endl;
  cout << "Total working time: " << total_work << endl;
  cout << "Total moving time: " << total_move << endl;
  cout << "Total update time: " << total_update << endl;
  cout << "Total comm time: " << total_receive << endl;
  cout << "Total insert time: " << total_insert << endl;
  cout << "Accumulative total active vertices: " << total_active << endl;
}

template <class VertexValue, class EdgeValue, class MessageValue>
void scheduler<VertexValue, EdgeValue, MessageValue>::reorder() {

  if (!g->num_vertices) {
    return;
  }

  index_map = new int[g->num_vertices];		

  // Sort according to indegree.
  sort(g->in_degree.begin(), g->in_degree.end(), compare);

  // Gen map_index. Original vid -> current position.
  for(int i = 0; i < g->num_vertices; i++) {
    if(g->in_degree[i].in_degree) {
      index_map[g->in_degree[i].vid] = i;	
    }
  }

  // Calculate length of message buffer for each partition.
  int width = vertices_per_buffer;
  // ceil((double)g->num_vertices/num_buffers);

  cout << "Rank: " << my_rank << " width is: " << width << endl;
  cout << "Rank: " << my_rank << " vlen is: " << vlen << endl;

  size_t remain = g->num_vertices;
  size_t start, end;
  size_t index = 0;

  size_t max_indeg = 0;
  size_t min_indeg = INT_MAX;
  for(int i = 0; i < num_buffers; i++) {
    start = index;		
    if(remain >= width)	{
      end = start + width - 1;			
    } else {
      end = g->num_vertices - 1;	
    }

    size_t max_length = 0;
    size_t min_length = INT_MAX;

    for(int j = start; j <= end; j++) {
      if(g->in_degree[j].in_degree > max_length) {
        max_length = g->in_degree[j].in_degree;
      }
      if(g->in_degree[j].in_degree < min_length) {
        min_length = g->in_degree[j].in_degree;
      }
    }

    // Init the buffers. Set to max length in each group.
    max_length++;
    msg_buf[i].set_length(max_length);

    index = end + 1;
    remain -= (end - start + 1);
    max_indeg = max(max_indeg, max_length);
    min_indeg = min(min_indeg, min_length);
  }

  // Allocate receive buffer for each receiving thread.
  for (int i = 0; i < msg_receiver_thread; ++i) {
    if (max_indeg) {
      receive_buffer[i] = new MessageValue[max_indeg];
    }
  }

  cout << "===========> Max length: " << max_indeg << endl;
  cout << "===========> Min length: " << min_indeg << endl;
}

template <class VertexValue, class EdgeValue, class MessageValue>
scheduler<VertexValue, EdgeValue, MessageValue>::~scheduler() {
  if (num_buffers) {
    delete[] msg_buf;
  }
  if (g->num_vertices) {
    delete[] index_map;
  }
  for (int i = 0; i < msg_receiver_thread; ++i) {
    if (receive_buffer[i]) {
      delete[] receive_buffer[i];
    }
  }
}

#endif
