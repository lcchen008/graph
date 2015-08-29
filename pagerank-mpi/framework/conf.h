// Configuration file for the whole system.
// Author: chenlinc@cse.ohio-state.edu.
#ifndef CONF_H_
#define CONF_H_

#include <float.h> 

// Common macros shared by MIC and CPU.

#define CEILING(x,y) (((x) + (y) - 1) / (y))

// used by the MPI sendrecv for active count exchange.
#define STATUS_TAG 123
// used by the MPI sendrecv for msg block info exchange.
#define MSG_BLOCK_INFO_TAG 234
#define sender_block_size 20
#define receiver_block_size 20
#define inserter_block_size 20
#define num_iter 20  // Max number of iterations.
#define USE_REDUCTION

//**************MIC macros*************
#if defined (MIC)
#define vlen 64 
#define nthread 840 
#define reduce_thread 60  // This is the optimun number different from nthread.
#define update_thread 60  // This is the optimun number different from nthread.
#define status_count_thread 60 // This is the optimun number different from nthread.
#define compute_size_thread 60 // This is the optimun number different from nthread.
#define msg_sender_thread 1 // For parallel msg sending stage.
#define msg_receiver_thread 1 // For parallel msg receiving stage.
#define msg_inserter_thread 60 // For parllel msg insertion stage.

//if MSG_FLOAT then use float as the message type
//   MSG_INT int as message type
//   MSG_DOUBLE double as message type
#define MSG_FLOAT

// * Needs to be configured by users.
// The initial value in each element of msg buffer.
// For SSSP, it should be max value of float since we are
// computing the mininum value during reduction.
#define BUFFER_INIT_VAL 0 

#define msg_queue_size 2000

// Divide the ques into different classes.
// Each mover only focus on one class.
// #define num_queues 660 
#define num_queues 660 

#define num_moving_threads num_queues

// How many msgs are moved each time from one queue.
#define msgs_moved 2048 

#define num_working_threads (nthread - num_queues)

#define vertices_per_buffer 64 
#define vertices_per_array (vlen/4)

// Number of buffers.
// #define buffer_num_parts CEILING(NUM_VERTICES, vertices_per_buffer)  

// This is the total number of preallocated vector msg arrays in each
// buffer. Change the number '4' to specific type when chaning message
// value type.
#define num_vmsg_array (vertices_per_buffer/(vlen/(4)))

// Used for dynamic scheduling for msg reduction.
#define buffer_block_size 10
// Used for dynamic scheduling for msg generation.
#define vertex_block_size 100 
#define update_vertex_block_size 10000 

// ************AVX macros ******************
#elif defined (AVX)
#define vlen 32 

// **************default SSE macros, used by CPU**************
#else
#define vlen 16
#define nthread 32 
#define reduce_thread 64  // This is the optimun number different from nthread.
#define update_thread 64  // This is the optimun number different from nthread.
#define status_count_thread 64 // This is the optimun number different from nthread.
#define compute_size_thread 64 // This is the optimun number different from nthread.
#define msg_sender_thread 1 // For parallel msg sending stage.
#define msg_receiver_thread 1 // For parallel msg receiving stage.
#define msg_inserter_thread 64 // For parllel msg insertion stage.

//if MSG_FLOAT then use float as the message type
//   MSG_INT int as message type
//   MSG_DOUBLE double as message type
#define MSG_FLOAT

// * Needs to be configured by users.
// The initial value in each element of msg buffer.
// For SSSP, it should be max value of float since we are
// computing the mininum value during reduction.
#define BUFFER_INIT_VAL FLT_MAX

#define msg_queue_size 400000

// Divide the ques into different classes.
// Each mover only focus on one class.
#define num_queues 16 

#define num_moving_threads num_queues

// How many msgs are moved each time from one queue.
#define msgs_moved 2048 

#define num_working_threads (nthread - num_queues)

#define vertices_per_buffer 64 
#define vertices_per_array (vlen/4)

// This is the total number of preallocated vector msg arrays in each
// buffer. Change the number '4' to specific type when chaning message
// value type.
#define num_vmsg_array (vertices_per_buffer/(vlen/(4)))

// Used for dynamic scheduling for msg reduction.
#define buffer_block_size 20
// Used for dynamic scheduling for msg generation.
#define vertex_block_size 500
#define update_vertex_block_size 10000 
// Used for parallel sending of remote messages.
#define remote_buffer_block_size 50

#endif

#endif  // CONF_H_
