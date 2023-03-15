#ifndef __GLOBAL_ARBITER_H__
#define __GLOBAL_ARBITER_H__

//---------------------------------------------------------------------------

#include <cassert>
#include <queue>
#include "GlobalTypeDefs.h"
#include "circularbuffer.h"
#include <iostream>     // std::cout
#include <algorithm>    // std::sort
#include <vector>       // std::vector


using namespace std;

//---------------------------------------------------------------------------

SC_MODULE(TGlobalarbiter)
{

  // I/O Ports
  sc_in_clk          clock;      // The input clock for the arbiter
  sc_in_clk          photonic_clock;
  sc_in<bool>        reset;      // The reset signal for the arbiter

  /** Ports for communication with Arbiter  - START **/
  
  sc_in<TFlit>      req_tx_arbiter_flit[DEFAULT_MESH_DIM_X][DEFAULT_MESH_DIM_Y];
  sc_in<bool>      req_tx_arbiter_hf[DEFAULT_MESH_DIM_X][DEFAULT_MESH_DIM_Y]; // ack from arbiter to send data
  sc_out<bool>      ack_tx_arbiter_hf[DEFAULT_MESH_DIM_X][DEFAULT_MESH_DIM_Y]; // ack from arbiter to send data
  bool              gwi_current_level_rx_req[DEFAULT_MESH_DIM_X][DEFAULT_MESH_DIM_Y];
  
  sc_out<bool>      req_rx_arbiter_ch[DEFAULT_MESH_DIM_X][DEFAULT_MESH_DIM_Y]; // ack from arbiter to send data
  sc_out<int>       mwmr_channel_no[DEFAULT_MESH_DIM_X][DEFAULT_MESH_DIM_Y]; // cordinates of channel from arbiter to send data
  sc_in<bool>       ack_rx_arbiter_ch[DEFAULT_MESH_DIM_X][DEFAULT_MESH_DIM_Y]; // ack to arbiter after sending data
  bool              gwi_current_level_tx_ch[DEFAULT_MESH_DIM_X][DEFAULT_MESH_DIM_Y];
  
  sc_in<bool>      req_txc_arbiter[DEFAULT_MESH_DIM_X][DEFAULT_MESH_DIM_Y]; // ack from arbiter to send data
  sc_in<bool>      ack_txc_arbiter[DEFAULT_MESH_DIM_X][DEFAULT_MESH_DIM_Y]; // ack from arbiter to send data
  sc_out<bool>      ack_txc_gwi[DEFAULT_MESH_DIM_X][DEFAULT_MESH_DIM_Y]; // ack from arbiter to send data
  
  /** Ports for communication with Arbiter  - END **/
  
  circularbuffer* cb[DEFAULT_MWMR_WAVEGUIDES];          //initialization of circularqueue 
  void proc_arbiter_req_rx();
  void proc_arbiter_index_tx();
  void proc_arbiter_ack_tx();
  void proc_arbiter_change_priority();
  void print();
   SC_CTOR(TGlobalarbiter):
   clock(sc_gen_unique_name("reset"))
  {
	  for(int c=0; c<4; c++)
	  {
		   current_priority[c]=1;
	   }
    for(int j=0; j<TGlobalParams::mwmr_waveguides; j++)
    {
		int k = j%8;
		//cout << "k: " << k << endl;
		cb[j] = new circularbuffer(16, 4, k, current_priority);
	}
    SC_METHOD(proc_arbiter_req_rx);
    sensitive << reset;
    sensitive << clock.pos();
     
	SC_METHOD(print);
    //sensitive << reset;
    ////sensitive << photonic_clock.pos();
     //for(int i=0; i<TGlobalParams::mesh_dim_x; i++)
		  //{
			  //for(int j=0; j<TGlobalParams::mesh_dim_y; j++)
			  //{
				  //sensitive <<  req_tx_arbiter_flit[i][j];
			  //}
		  //}
		   
    SC_METHOD(proc_arbiter_index_tx);
    sensitive << reset;
    sensitive << clock.pos();
    SC_METHOD(proc_arbiter_ack_tx);
    sensitive << reset;
    sensitive << clock.pos();
    SC_METHOD(proc_arbiter_change_priority);
    sensitive << reset;
    sensitive << photonic_clock.pos();
   }
   
  private:
  bool 				node_packet_status[DEFAULT_MESH_DIM_X][DEFAULT_MESH_DIM_Y];
  bool 				node_packet_status_temp[DEFAULT_MESH_DIM_X][DEFAULT_MESH_DIM_Y];
  int				current_count[DEFAULT_MWMR_WAVEGUIDES][4];
  int               current_priority[4];
  int               future_priority[4];
  
 };

#endif
