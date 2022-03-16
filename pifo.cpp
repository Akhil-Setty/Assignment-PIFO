#include <iostream>
#include "pifo.hpp"

using namespace std;

// Print the pifo queue (for debug)
void PIFO::print() {
	cout << "PIFO has " << num_nodes << " elements:" << endl;
	Node* node = head; 
  while (node != NULL) {
	  cout << "(" << node->nodeId << "," << node->priority << ")"<< endl;
	  node = node->next;
	}
}

void PIFO::enqueue(const string& nodeId, const uint64_t priority) {
	Node* new_node = new Node;
	new_node->next = NULL;
	new_node->nodeId = nodeId;
	new_node->priority = priority;
	/* 
		insert at the beginning if PIFO is empty
		If not empty insert according to priority
	*/
	if (head == NULL)
		head = new_node;
	else {
		Node* node = head;
		Node* prev = NULL;
		while (node != NULL) {
			if (node->priority > new_node->priority) {
				new_node->next = node; 
				if (prev != NULL) {
					prev->next = new_node;
				}
				break;
			}
			prev = node;
			node = node->next;
		}
		// insert at the end if lowest priority
		if (node == NULL && prev != NULL)
		{
			prev->next = new_node;
		}
	}
	num_nodes++;
}

int PIFO::dequeue(string& nodeId, uint64_t& priority) {
	if(head == NULL)
		return -1;
	Node* top_node = head;
	head = head->next;
	nodeId = top_node->nodeId;
	priority = top_node->priority;
	delete(top_node);
	num_nodes--;
	return 0;
}

uint32_t PIFO::size()
{
	return  num_nodes;
}
