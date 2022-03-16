#include <string>

typedef struct Node{
	std::string nodeId;
	uint64_t priority;
	Node* next;
}Node;

class PIFO {
	private:
		Node* head;
		uint32_t num_nodes;

	public:
		PIFO()
		{
			head = NULL;
			num_nodes = 0;
		}

		~PIFO()
		{
			Node* node = head;
			while(node != NULL) {
				Node* curr_node =  node;
				node = node->next;
				delete(curr_node);
			}
		}

		// Print the pifo queue (for debug)
		void print(); 
		uint32_t size();
		void enqueue(const std::string& nodeId, const uint64_t priority); 
		int dequeue(std::string& nodeId, uint64_t& priority); 
};

