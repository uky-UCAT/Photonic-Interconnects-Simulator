#ifndef __BENCHMARK_H__
#define __BENCHMARK_H__

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>        // std::abs
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h> 
#include <stdio.h> 
#include <systemc.h>
#include "GlobalTypeDefs.h"

using namespace std;

SC_MODULE(benchmark)
{
	sc_in_clk            clock;        // The input clock for the bm
    sc_in<bool>          reset;        // The reset signal for the bm


	void packet_injection_nodeindex();
	bool packet_injection_response(int nodeid_request);
	void clear_vector();


   SC_HAS_PROCESS(benchmark);
  benchmark(sc_module_name name,char* arg_vet[])
  {
	  cout<<"sample print"<< arg_vet[1]<<endl;
	  //cout<<"sample print"<< arg_vet[2]<<endl;
	  //cout<<"sample print"<< arg_vet[3]<<endl;
	  //cout<<"sample print"<< arg_vet[4]<<endl;

		count = 0;
		string benchmark_type = arg_vet[1];

		myfile.open(benchmark_type.c_str());
		if (myfile.is_open()) 
		{
        while (getline(myfile,line)) 
        {
			v.push_back(line);
		}
		}
		myfile.close();
		cout << "Number of values read from " << benchmark_type <<" file are " << v.size() << endl;
		cout << "First Line " << v[0] << endl;
		//cout << "First Line " << v[i-1][1] << endl;
		//cout << "First Line " << v[i-1][2] << endl;
    SC_METHOD(packet_injection_nodeindex);
    sensitive << reset;
    sensitive << clock.pos();


  }

private:

	int count;
	double current_cycle;
	string benchmark_type;
	ifstream myfile;
	string line;
	vector<string> v;
	vector<int> node_id;
	int nodeid_request;

};

#endif

	
