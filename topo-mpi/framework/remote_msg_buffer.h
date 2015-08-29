#ifndef REMOTE_MSG_BUF_H
#define REMOTE_MSG_BUF_H 

#include <vector>

#include "atomic.h"

template <class MessageValue>
class remote_msg_buffer {
 public: 
  // Destination vid in global rank.
  int dst_;
  // Next insertion position, i.e., the tail.
  int index_;
#if defined (LOCK_BASED)
  int lock_;
#endif
  // Space for storing the messages.
  mutable vector<MessageValue> buf;
  remote_msg_buffer() : index_(0) {
#if defined (LOCK_BASED)
  lock_ = 0;
#endif
  }
  void set_length(int len) {
    buf.resize(len);
  }
  int size() const {
    return index_;
  }
  int dst() const {
    return dst_;
  }
  void insert(MessageValue& msg) {
#if defined (LOCK_BASED)
  get_lock(&lock_);
#endif
    buf[index_] = msg; 
    ++index_;
#if defined (LOCK_BASED)
  release_lock(&lock_);
#endif
  }
  void re_init() {
    index_ = 0;
  }
  MessageValue& operator[](int idx) {
    return buf[idx];
  }
  void resize(int size) {
    buf.resize(size);
  }
};

#endif /* REMOTE_MSG_BUF_H */
