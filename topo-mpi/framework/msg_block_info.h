// A data structure containing the information of one
// message block. It contains the size and tag of the
// message block. A message block essentially contains
// a bunch of message values sent to/from remote process.
// The tag is the global vertex id of the destination
// vertex that the messages are being sent to.

#ifndef MSG_BLOCK_INFO_H
#define MSG_BLOCK_INFO_H 

template <class MessageValue>
class msg_block_info {
 public:
  int size_;
  int tag_;
  MessageValue value_;
  msg_block_info(int size, int tag, MessageValue value) : size_(size), tag_(tag), value_(value) {}
  msg_block_info() {}
  inline int size() const {
    return size_;
  }
  inline int tag() const {
    return tag_;
  }
  inline MessageValue value() const {
    return value_;
  }
};

#endif /* MSG_BLOCK_INFO_H */
