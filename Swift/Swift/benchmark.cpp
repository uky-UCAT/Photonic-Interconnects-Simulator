#include "benchmark.h"

void benchmark::packet_injection_nodeindex()
{
	if(!reset.read())
	{
		if(!node_id.empty())
		{
			node_id.clear(); // clearing previous cycle node entries
		}

		int lineNumber=v.size();
		if((lineNumber-2) > count)
		{

			double _current_cycle= sc_time_stamp().to_double()/1000;
			double cc= _current_cycle;
			//cout<< "current time: "<< cc << endl;
			std::string stringitem[3];
			int i=0;

			while(i<16)
			{
				std:: string str = v[count];
				std::istringstream myline(str);
				int word_no = 0;
				while(std::getline(myline, str, ' '))
				{
					stringitem[word_no] = str;
					word_no++;
				}
				double temp =  double(atof(stringitem[0].c_str()));
				if(temp == cc)
				{
					const char* temp2 = stringitem[1].c_str();
					int temp_nid = atoi(temp2);
					for(int k=0;k<4;k++)
					{
						node_id.push_back(temp_nid + k*16);      // Pushing ids of nodes that inject packets in the current cycle

					}
					count++;
					i++;
				}
				else
				{
					i=17;
				}
			}
		}


		//for(int i=0; i < node_id.size(); i++)
		//{
		//cout<< sc_time_stamp().to_double()/1000<<" : node id"<< node_id[i]<<endl;
		//}
	}

}

//This function provides response to the processing element, whether to inject traffic or not 
bool benchmark::packet_injection_response(int nodeid_request)
{
	if(!node_id.empty())
	{
		if(std::find(node_id.begin(), node_id.end(), nodeid_request) != node_id.end())
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

void benchmark::clear_vector()
{
	//v.clear();

}










