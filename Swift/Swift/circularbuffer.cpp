#include "circularbuffer.h"

circularbuffer::circularbuffer(int size, int clusters, int tokensync, int* current_priority)
{
	circularbuffer::node* cbuffer = new node[size];
	for(int i=0; i< clusters ; i++)
	{ 
		countlocal[i]=0;
	}
	int tempsync = tokensync;
	int j =0;
	currenttoken = clusters - 1;
	int total_priority =0;
	for(int c=0; c<clusters; c++)
	{
		total_priority = total_priority + *(current_priority +c);
	}
	int basicslot = 3 * total_priority;
	count = std::abs((basicslot-1) -(tokensync % basicslot));
	int temp = *(current_priority);  	
	for(int i=0; i< (3 * total_priority) ; i++)
	{ 
		
		if(i%3 == 0)
		{
			if(temp == 0)
			{
				j++;
				temp = *(current_priority+j);
				if(temp == 0)
				{
					j++;
				}
			}
			initialisetoken[i] = j;
			//cout << "first pass :" << initialisetoken[i] << " count: " << i << endl;
			temp--;
			if(j== clusters)
			{
				j=0;
			}
		}
		else
		{
			initialisetoken[i] = -1;
			//cout << "first pass :" << initialisetoken[i] << " count: " << i << endl;
		}
	}
	
	int t=0;
	int tempts = tokensync;
	for(int i=0; i<size; i++)
	{
		if (tempsync < size)
		{
			cbuffer[tempsync].tokentype = initialisetoken[t];
			
			if(initialisetoken[t] != -1)
			{
				cbuffer[tempsync].flag = true;
			}
			else
			{
				cbuffer[tempsync].flag = false;
			}
			t++;
			tempsync++;
			if(t == (3*total_priority))
			{
				t=0;
			}
			
		}
		else
		{
			tempts --;
			basicslot--;
			cbuffer[tempts].tokentype = initialisetoken[basicslot];
			if(initialisetoken[basicslot] != -1)
			{
				cbuffer[tempts].flag = true;
			}
			else
			{
				cbuffer[tempts].flag = false;
			}
			
			if(basicslot == 0)
			{
				basicslot = 3*total_priority;
			}
		}	
		cbuffer[i].destinationid = -1;	
		cbuffer[i].slotid = i;
		//cout << "Current slot number:" << i << " Current token :" << cbuffer[i].tokentype<< " Current flag :" << cbuffer[i].flag << endl;
		if(i != size-1 && i != 0)
		{
			cbuffer[i].next = &cbuffer[i+1];
			cbuffer[i].previous = &cbuffer[i-1];
		}
		if(i == size-1)
		{
			cbuffer[i].next = &cbuffer[0];
			cbuffer[i].previous = &cbuffer[size-2];
		}
		if(i == 0)
		{
			cbuffer[i].next = &cbuffer[i+1];
			cbuffer[i].previous = &cbuffer[size-1];
		}
	}
	head = &cbuffer[0];
	head2p = &cbuffer[size/2];
	
}

void circularbuffer::change_token_priority(int clusters, int tokensync, int* current_priority)
{
	for(int i=0; i< clusters ; i++)
	{ 
		countlocal[i]=0;
	}
	int total_priority =0;
	int j =0;
	
	for(int c=0; c<clusters; c++)
	{
		total_priority = total_priority + *(current_priority +c);
	}
	count = std::abs(((3 * total_priority)-1) -(tokensync % (3 * total_priority)));
	int temp = *(current_priority);  	
	for(int i=0; i< (3 * total_priority) ; i++)
	{ 
		
		if(i%3 == 0)
		{
			if(temp == 0)
			{
				j++;
				temp = *(current_priority+j);
				if(temp == 0)
				{
					j++;
				}
			}
			initialisetoken[i] = j;
			//cout << "second pass :" << initialisetoken[i] << " count: " << i << endl;
			temp--;
			if(j== clusters)
			{
				j=0;
			}
		}
		else
		{
			initialisetoken[i] = -1;
			//cout << "second pass :" << initialisetoken[i] << " count: " << i << endl;
		}
	}
}

void circularbuffer::rotate(int clusters, int* current_priority)    // function that rotates circular buffer
{
	circularbuffer::head = circularbuffer::head->previous;
	circularbuffer::head2p = circularbuffer::head2p->previous;
	int total_priority =0;
	for(int c=0; c<clusters; c++)
	{
		total_priority = total_priority + *(current_priority +c);
	}
	//cout<< count << " IC "<< initialisetoken[count] <<endl;
	if (count >= 0)
	{
		circularbuffer::head->tokentype = initialisetoken[count];
		//cout << "sample value: "<< initialisetoken[count] << " count : "<< count << endl;
		if (initialisetoken[count] != -1)
		{
			circularbuffer::head->flag = true;
		}
		else
		{
			circularbuffer::head->flag = false;
		}
		count--;
	}
	else
	{
		count = (3*total_priority)-1;
		//cout<< count << " IC "<< initialisetoken[count] <<endl;
		circularbuffer::head->tokentype = initialisetoken[count];
		count--;
	}
}

void circularbuffer::transferpriority(int total_nodes, int cluster_size, int clusters, int* countnew) // function that transfers priority dynamically
{
	int core_index = cluster_size-1;
	circularbuffer::node* tmp;
	circularbuffer::node* tmp2p;
	tmp = circularbuffer::head;
	tmp2p = circularbuffer::head2p;
	for(int k=0; k<clusters; k++)
	{
	for(int i=0; i< core_index/8 ; i++)
	{
		if(core_index/8 == 0)
		{
			tmp=tmp;
			tmp2p=tmp2p;
		}
		else
		{
			tmp = tmp->next;
			tmp2p = tmp2p->next;
		}
	}
	
	if(tmp->flag == 1 && tmp->tokentype == k)
	{
		countlocal[k]++;
		int temp = countlocal[k];
		*(countnew+k) = temp;
		tmp->tokentype = (k+1)% clusters;
	}
	//cout<<"cluster No: "<< k << " Countnew :" << temp<< "Core index :" << core_index << " Token_type :" << tmp->tokentype << " token_flag: "<< tmp->flag << endl;
	
	tmp = circularbuffer::head;
	tmp2p = circularbuffer::head2p;
	if(core_index < total_nodes)
	{
		core_index = core_index + cluster_size;
	}
	else
	{
		core_index = cluster_size-1;
	}
}

}	

bool circularbuffer::assignnew(int indexnew1, int indexnew2)
{
	circularbuffer::node* tmp1;
	circularbuffer::node* tmp2p1;
	bool ret1 = false;
	tmp1 = circularbuffer::head;
	tmp2p1 = circularbuffer::head2p;
	int clusternumber = indexnew1/16;
	for(int i=0; i< indexnew1/2 ; i++)
	{
		if(indexnew1/2 == 0)
		{
			tmp1=tmp1;
			tmp2p1=tmp2p1;
		}
		else
		{
			tmp1 = tmp1->next;
			tmp2p1 = tmp2p1->next;
		}
	} 
	if (tmp1->flag == 1 && tmp1->tokentype == clusternumber)
	{
		tmp1->flag =0;
		tmp1->tokentype = -1;
		ret1 = true;
	 }
	 else
	 {
		 ret1 = false;
	 }
	 return ret1; 
 }

bool circularbuffer::assign(int index){
	circularbuffer::node* tmp;
	circularbuffer::node* tmp2p;
	bool ret = false;
	tmp = circularbuffer::head;
	tmp2p = circularbuffer::head2p;
	int clusternumber = index/16;
	for(int i=0; i< index/8 ; i++)
	{
		if(index/8 == 0)
		{
			tmp=tmp;
			tmp2p=tmp2p;
		}
		else
		{
			tmp = tmp->next;
			tmp2p = tmp2p->next;
		}
	} 
	if (tmp->flag == 1 && tmp->tokentype == clusternumber)
	{
		tmp->flag =0;
		tmp->tokentype = -1;
		ret = true;
	 } 
	 else 
	 {
		ret = false; 
	 }
	 return ret; 
 }
 
 void circularbuffer::print(int size)    // print details of circular buffer
{
	circularbuffer::node* tmp;
	tmp = circularbuffer::head;
	for(int i=0; i< size ; i++)
	{
		cout << "Current slot number:" << tmp->slotid << " Current token :" << tmp->tokentype << " Current flag :" << tmp->flag << endl;
		tmp = tmp->next;
	}
	
	//for(int i=0; i<4;i++)
	//{
		//cout<<"cluster No: "<< i <<" tokens changed : "<< countlocal[i]<<endl;
	//} 
}
 
 
	
	
		
	
