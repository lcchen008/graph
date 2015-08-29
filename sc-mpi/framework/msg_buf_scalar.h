#ifndef MSG_BUF_SCALAR_H_
#define MSG_BUF_SCALAR_H_

#include "atomic.h"

template <class MessageValue>
class msg_buffer_scalar
{
	public:
		vector<MessageValue> buf;
		int index;
		int lock;

		void re_init()
		{
				index = 0;
		}

		msg_buffer_scalar()
		{
			lock = 0;
      index = 0;
		}

		void set_length(size_t length)
		{
		  buf.resize(length);	
		}

		void insert(MessageValue &msg)
		{
#if defined (LOCK_BASED)
      get_lock(&lock);
#endif
			buf[index] = msg;
			++index;
#if defined (LOCK_BASED)
      release_lock(&lock);
#endif
		}

    MessageValue& operator[](int idx) {
      return buf[idx];
    }
    int size() {
	return index;
	}
};

#endif
