#ifndef MSG_BUF_H
#define MSG_BUF_H 

template <class MessageValue>
class msg_buf {
 public:
  vector<MessageValue> buf;
  int index;

  msg_buf() {
    index = 0;
  }

  void set_length(int size) {
    buf.resize(size);
  }

  void re_init() {
    index = 0;
  }

  int size() {
    return index;
  }

  MessageValue& operator[](int idx) {
    return buf[idx];
  }

  void insert(MessageValue& v) {
    buf[index] = v;
    ++index;
  }
};

#endif /* MSG_BUF_H */
