#ifndef VERTEX_H_
#define VERTEX_H_

template <typename VertexValue,
	 typename EdgeValue,
	 typename MessageValue>
class Vertex {
	public:
		const int vertex_id() const;
		int64 superstep() const;
		const VertexValue& GetValue();
		VertexValue& MutableValue();
		OutEdgeIterator GetOutEdgeIterator();

		// The two user-defined funcs.
		virtual void Compute() = 0;
		void SendMessageTo(const string& dest_vertex, const MessageValue& message);

		void VoteToHalt();
};

#endif
