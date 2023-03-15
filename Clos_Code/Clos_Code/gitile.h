#ifndef __TGITILE_H__
#define __TGITILE_H__

//---------------------------------------------------------------------------

#include <systemc.h>
#include "processingelement.h"
#include "hub.h"
#include "gatewayinterface.h"
#include "GlobalTypeDefs.h"

SC_MODULE(TGITile)
{

  // I/O Ports

  sc_in_clk           clock;        // The input clock for the tile
  sc_in_clk          photonic_clock;
  sc_in<bool>         reset;        // The reset signal for the tile

  sc_in<TFlit>        flit_rx_clos[DEFAULT_CLOS_WAVEGUIDES];   // The input channels to clos network
 
  sc_out<TFlit>       flit_tx_clos[DEFAULT_CLOS_WAVEGUIDES];   // The output channels to clos network

  
  // NoP signals inside tile
  sc_signal<TFlit>    flit_Nop_temp;   // The duplicated output channels
  
  // Signals for Processing element to Hub1&Hub2 and Hub1&Hub2 to GWI

  sc_signal<TFlit>    flit_rx_local[DIRECTIONS+1][HUBS_PER_CLUSTER];   // The input channels
  sc_signal<bool>     req_rx_local[DIRECTIONS+1][HUBS_PER_CLUSTER];    // The requests associated with the input channels
  sc_signal<bool>     ack_rx_local[DIRECTIONS+1][HUBS_PER_CLUSTER];    // The outgoing ack signals associated with the input channels

  sc_signal<TFlit>    flit_tx_local[DIRECTIONS+1][HUBS_PER_CLUSTER];   // The output channels
  sc_signal<bool>     req_tx_local[DIRECTIONS+1][HUBS_PER_CLUSTER];    // The requests associated with the output channels
  sc_signal<bool>     ack_tx_local[DIRECTIONS+1][HUBS_PER_CLUSTER];    // The outgoing ack signals associated with the output channels
  
  sc_signal<int>     free_slots_local[DIRECTIONS+1][HUBS_PER_CLUSTER];
  sc_signal<int>     free_slots_neighbor_local[DIRECTIONS+1][HUBS_PER_CLUSTER];

  // Instances
  TGatewayInterface*   gwi;				// Gateway interface
  THub*               h[HUBS_PER_CLUSTER];             // Hub instance
  TProcessingElement* pe[PE_PER_HUB][HUBS_PER_CLUSTER];         // Processing Element instance
  
  // BENCHMARK INTIALIZATION
   benchmark              &b_mark;
  // Constructor

  TGITile(sc_module_name name, benchmark &_b_mark):
  b_mark(_b_mark)
  //mwmr_data_rx(sc_gen_unique_name("flit_tx"))
  {  
    // GatewayInterface pin assignments
    gwi   = new TGatewayInterface("GatewayInterface");
    gwi->clock(clock);
    gwi->photonic_clock(photonic_clock);
    gwi->reset(reset);
	// GatewayInterface data pin assignment with tile
     for(int i=0; i<(TGlobalParams::clos_waveguides); i++)
    {  
		  gwi->clos_data_rx[i](flit_rx_clos[i]); // Extension of input channels to tile
		  gwi->clos_data_tx[i](flit_tx_clos[i]); // Extension of output channels to tile
    }
    // GatewayInterface pin assignment with Hub1 and Hub2
    for(int hub_index = 0; hub_index<HUBS_PER_CLUSTER; hub_index++)
    {
    gwi->flit_rx[hub_index](flit_rx_local[DIRECTIONS][hub_index]);
    gwi->req_rx[hub_index](req_rx_local[DIRECTIONS][hub_index]);
    gwi->ack_rx[hub_index](ack_rx_local[DIRECTIONS][hub_index]);

    gwi->flit_tx[hub_index](flit_tx_local[DIRECTIONS][hub_index]);
    gwi->req_tx[hub_index](req_tx_local[DIRECTIONS][hub_index]);
    gwi->ack_tx[hub_index](ack_tx_local[DIRECTIONS][hub_index]);

    gwi->free_slots[hub_index](free_slots_local[DIRECTIONS][hub_index]);
    gwi->free_slots_neighbor[hub_index](free_slots_neighbor_local[DIRECTIONS][hub_index]);
	}
    // Processing Element pin assignments to/form a two concentrators (Hub1 and Hub2) having 4 PE's each
    for(int hub_index = 0; hub_index<HUBS_PER_CLUSTER; hub_index++)
    {
    for(int i=0; i<PE_PER_HUB; i++)
    {
	  char processing_element[30];
	  sprintf(processing_element, "ProcessingElement[%02d][%02d]", i, hub_index);
    pe[i][hub_index] = new TProcessingElement(processing_element, b_mark);
    pe[i][hub_index]->clock(clock);
    pe[i][hub_index]->reset(reset);

    pe[i][hub_index]->flit_rx(flit_rx_local[i][hub_index]);
    pe[i][hub_index]->req_rx(req_rx_local[i][hub_index]);
    pe[i][hub_index]->ack_rx(ack_rx_local[i][hub_index]);

    pe[i][hub_index]->flit_tx(flit_tx_local[i][hub_index]);
    pe[i][hub_index]->req_tx(req_tx_local[i][hub_index]);
    pe[i][hub_index]->ack_tx(ack_tx_local[i][hub_index]);

    pe[i][hub_index]->free_slots(free_slots_local[i][hub_index]);
    pe[i][hub_index]->free_slots_neighbor(free_slots_neighbor_local[i][hub_index]);
	}
	}
     // Hub1 and Hub2 pin assignments
    for (int hub_index = 0; hub_index<HUBS_PER_CLUSTER; hub_index++)
    {
		char hub_name[20];
	  sprintf(hub_name, "Hub[%02d]", hub_index);
    h[hub_index] = new THub(hub_name);
    h[hub_index]->clock(clock);
    h[hub_index]->reset(reset);
	// Hub1 and Hub2 pin assignments with pe and GWI
	for(int i=0; i<DIRECTIONS+1; i++)
    {
    h[hub_index]->flit_tx[i](flit_rx_local[i][hub_index]);
    h[hub_index]->req_tx[i](req_rx_local[i][hub_index]);
    h[hub_index]->ack_tx[i](ack_rx_local[i][hub_index]);

    h[hub_index]->flit_rx[i](flit_tx_local[i][hub_index]);
    h[hub_index]->req_rx[i](req_tx_local[i][hub_index]);
    h[hub_index]->ack_rx[i](ack_tx_local[i][hub_index]);

    h[hub_index]->free_slots[i](free_slots_neighbor_local[i][hub_index]);
    h[hub_index]->free_slots_neighbor[i](free_slots_local[i][hub_index]);
	}
	}

  }

};

#endif
