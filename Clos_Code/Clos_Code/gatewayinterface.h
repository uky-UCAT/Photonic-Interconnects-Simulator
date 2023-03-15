#ifndef __GATEWAYINTERFACE_H__
#define __GATEWAYINTERFACE_H__

//---------------------------------------------------------------------------

#define GI_BUFFER_DEPTH 2
#define GLOBAL_TX_INDEX 1
#define LOCAL_TX_INDEX  0
#define RESET_REQ_QUEUE 200
#define ONE_CYCLE 0.5

#include <systemc.h>
#include <vector>
#include <algorithm>
#include "buffer.h"
#include "status.h"
//#include "reservationtable.h"
#include "GlobalTypeDefs.h"
extern unsigned int lambda[];

SC_MODULE(TGatewayInterface)
{
  // I/O Ports
  sc_in_clk          clock;      // The input clock for the router
  sc_in_clk          photonic_clock;
  sc_in<bool>        reset;               // The reset signal for the router

  /** Ports for the bottom electrical layer connect with Hub1 and Hub2- START **/
  sc_in<TFlit>       flit_rx [HUBS_PER_CLUSTER];
  sc_in<bool>          req_rx[HUBS_PER_CLUSTER];
  sc_out<bool>         ack_rx[HUBS_PER_CLUSTER];
  
  sc_out<TFlit>      flit_tx[HUBS_PER_CLUSTER];
  sc_out<bool>       req_tx[HUBS_PER_CLUSTER];
  sc_in<bool>        ack_tx[HUBS_PER_CLUSTER];
  
  sc_out<int>        free_slots[HUBS_PER_CLUSTER];
  sc_in<int>         free_slots_neighbor[HUBS_PER_CLUSTER];
  
 
  /** Ports for the bottom electrical layer - END **/
  
  /** Ports for optical communication  - START **/
  
  sc_out<TFlit>                  clos_data_tx[DEFAULT_CLOS_WAVEGUIDES];
  sc_in<TFlit>                   clos_data_rx[DEFAULT_CLOS_WAVEGUIDES];
  
  /** Ports for optical communication  - END **/
  
  int cycle_count;

  TBuffer            buffer_local[HUBS_PER_CLUSTER];         // Buffer for input channel from the electrical side
  TBuffer            buffer_receive;         // Buffer for input channel from the optical side
  bool               current_level_rx[HUBS_PER_CLUSTER];    // Current level for Alternating Bit Protocol (ABP)
  bool               current_level_tx[HUBS_PER_CLUSTER];    // Current level for Alternating Bit Protocol (ABP)
  bool               current_level_tx_local[HUBS_PER_CLUSTER];    // Local Current level for Alternating Bit Protocol (ABP)
  
  TStats             stats;                           // Statistics
  
  int 				 cluster_id;
  unsigned long routed_flits;
  
//  static long flitsThroughGWI;


  // Threads and Functions
  void 				 delayCount();
  void               local_to_local_rx_process(); // The receiving process from eletrical hubs to GWI
  void               clos_to_local_rx_process(); // The receive data from clos network to local buffer
  void 				 clos_to_local_tx_process(); // The Tx process that send data from clos network to local hubs
  void               local_to_clos_tx_process(); // Send the data from local buffers to clos network
  void               local_to_local_tx_process(); // Send the data from one hub to another
  unsigned long     getRoutedFlits();       // Returns the number of routed flits 
  double            getPower();	
  // Constructor

  SC_CTOR(TGatewayInterface)
  {
	  buffer_receive.SetMaxBufferSize(PHOTONIC_BUFFER_DEPTH);

	  SC_METHOD(delayCount);
	  sensitive << reset;
	  sensitive << clock.pos();

	  SC_METHOD(local_to_local_rx_process);
      sensitive << reset;
      sensitive << clock.pos();
      
      SC_METHOD(clos_to_local_tx_process);
      sensitive << reset;
      sensitive << clock.pos();

      SC_METHOD(clos_to_local_rx_process);
      sensitive << reset;
      sensitive << photonic_clock.pos();
      
      SC_METHOD(local_to_clos_tx_process);
      sensitive << reset;
      sensitive << clock.pos();
      
      SC_METHOD(local_to_local_tx_process);
      sensitive << reset;
      sensitive << clock.pos();
    	    	            
  }

  private:   
  
  vector<int>        reserve_req_queue;
  vector<int>        data_ack_queue;
  sc_uint<2>         req_source_id;
  int                prev_winner;
  int 				 prev_srcid;
  int                prev_dstid;
  double 			 previous_transfer;
  double 			 previous_transfer_photonic[DEFAULT_CLOS_WAVEGUIDES];
  int 			 previous_transfer_srcid[DEFAULT_CLOS_WAVEGUIDES];
  bool               safe_to_reserve; // safe to send reservation to arbiter
  bool               safe_to_send;    // safe to send data to crossbar
  public:
  unsigned int local_drained;
};  

#endif

