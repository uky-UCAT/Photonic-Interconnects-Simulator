#ifndef __CIRCULARBUFFER_H__
#define __CIRCULARBUFFER_H__

#include <iostream>
#include <cmath>        // std::abs
using namespace std;

class circularbuffer{

private:

	struct node{
		bool flag;
		int tokentype; // provide token number of a cluster 
		int slotid;    // Has current slot number
		int destinationid;
		node* next;
		node* previous;
	};
	node* head;
	node* head2p;
	node* temp;
	int size;
	int clusters;
	int tokensync;
	int count;
	int currenttoken;
	int initialisetoken[100];
	int cluster_size;
	int total_nodes;
	int countlocal[4];
	

public:

	circularbuffer(int size, int clusters, int tokensync, int* current_priority);
	void change_token_priority(int clusters, int tokensync, int* current_priority);
	void rotate(int clusters, int* current_priority);
	bool assignnew(int indexnew1, int indexnew2);
	bool assign(int index);
	void print(int size);
	void transferpriority(int total_nodes, int cluster_size, int clusters, int* countnew);

};

	
#endif
