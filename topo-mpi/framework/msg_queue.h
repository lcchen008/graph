#ifndef MSG_QUEUE_H_
#define MSG_QUEUE_H_
#include "msg.h"
#include <atomic>

template <class MessageValue>
class msg_queue {
 public:
  std::atomic<int> head;
  std::atomic<int> tail;
  vector<msg<MessageValue> > queue;

  msg_queue() {
    queue.resize(msg_queue_size);
    head = 0, tail = 0;	
  }
  
  inline int increment(const int& v) const {
    return (v + 1) % msg_queue_size; 
  }

  inline bool empty() const {
    return head.load() == tail.load();	
  }

  inline bool full() const {
    const auto next_head = increment(head);
    return next_head == tail.load();
  }

  inline bool push_front(const int &dst, const MessageValue &message) {
    const auto current_head = head.load();
    const auto next_head = increment(current_head);

    // Not full.
    if (next_head != tail.load()) {
      queue[current_head].dst = dst;
      queue[current_head].value = message;
      head.store(next_head);
      return true;
    }

//    cout << "Full..." << endl;

    return false;
  }

  inline bool pop_back(msg<MessageValue> *ret) {
    const auto current_tail = tail.load();
    if (current_tail == head.load()) {
      return false;
    }

    ret->dst = queue[current_tail].dst;
    ret->value = queue[current_tail].value;
    tail.store(increment(tail));

    return true;
  }

  void re_init() {
    if (!empty()) {
      cout << "****Error: Queue not empty.****" << endl;
    }
    head = 0;	
    tail = 0;
  }
};

#endif
