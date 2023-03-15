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

  sc_in<TFlit>        flit_rx[DEFAULT_MWMR_WAVEGUIDES][4];   // The input channels
 
  sc_out<TFlit>       flit_tx[DEFAULT_MWMR_WAVEGUIDES][4];   // The output channels
  /** Ports for communication with Arbiter  - START **/
  
   sc_out<TFlit>		     req_tx_arbiter_flit;   // req for arbiter 
  sc_out<bool>           req_tx_arbiter_hf;  //req for arbiter to receive head flit
  sc_in<bool>            ack_tx_arbiter_hf;  // ack of arbiter for receipt of head flit      
  
  sc_in<bool>            req_rx_arbiter_ch;//ack_tx_arbiter;    // ack from arbiter to send data
  sc_in<int>             mwmr_channel_no; // MWMR channel number from arbiter to send data
  sc_out<bool>            ack_rx_arbiter_ch;  // ack of arbiter for receipt of channel number
    
  sc_out<bool>			 req_txc_arbiter;
  sc_out<bool>			 ack_txc_arbiter;    // ack to arbiter after sending data
  sc_in<bool>			 ack_txc_gwi;    // ack from arbiter after receving data
  
  /** Ports for communication with Arbiter  - END **/
  
  
  // NoP signals inside tile
  sc_signal<TFlit>    flit_Nop_temp;   // The duplicated output channels
  
  // Signals for Processing element to Hub

  sc_signal<TFlit>    flit_rx_local;   // The input channels
  sc_signal<bool>     req_rx_local;    // The requests associated with the input channels
  sc_signal<bool>     ack_rx_local;    // The outgoing ack signals associated with the input channels

  sc_signal<TFlit>    flit_tx_local;   // The output channels
  sc_signal<bool>     req_tx_local;    // The requests associated with the output channels
  sc_signal<bool>     ack_tx_local;    // The outgoing ack signals associated with the output channels
  
  sc_signal<int>     free_slots_local;
  sc_signal<int>     free_slots_neighbor_local;
  
   // Signals for Hub to GWI

  sc_signal<TFlit>    gflit_rx_local;   // The input channels
  sc_signal<bool>     greq_rx_local;    // The requests associated with the input channels
  sc_signal<bool>     gack_rx_local;    // The outgoing ack signals associated with the input channels

  sc_signal<TFlit>    gflit_tx_local;   // The output channels
  sc_signal<bool>     greq_tx_local;    // The requests associated with the output channels
  sc_signal<bool>     gack_tx_local;    // The outgoing ack signals associated with the output channels
  
  sc_signal<int>     gfree_slots_local;
  sc_signal<int>     gfree_slots_neighbor_local;

  // Instances
  TGatewayInterface*   gwi;				// Gateway interface
  THub*                  h;             // Hub instance
  TProcessingElement* pe;              // Processing Element instance

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
	/** Pin assignment for communication with Arbiter  - START **/
	gwi->req_tx_arbiter_flit(req_tx_arbiter_flit);
	gwi->req_tx_arbiter_hf(req_tx_arbiter_hf);
	gwi->ack_tx_arbiter_hf(ack_tx_arbiter_hf);
	gwi->req_rx_arbiter_ch(req_rx_arbiter_ch);
	gwi->mwmr_channel_no(mwmr_channel_no);
	gwi-> ack_rx_arbiter_ch( ack_rx_arbiter_ch);
	gwi->req_txc_arbiter(req_txc_arbiter);
	gwi->ack_txc_arbiter(ack_txc_arbiter);
	gwi->ack_txc_gwi(ack_txc_gwi);
	/** Pin assignment for communication with Arbiter  - END **/
    for(int i=0; i<(TGlobalParams::mwmr_waveguides); i++)
    {
		for(int j=0; j<4; j++)
		{  
		  gwi->mwmr_data_rx[i][j](flit_rx[i][j]); // Extension of input channels to tile
		  //cout<<"flit_rx[i][j]"<<flit_rx[i][j]<<endl;
		  gwi->mwmr_data_tx[i][j](flit_tx[i][j]); // Extension of output channels to tile
	  }
    }
    // GatewayInterface pin assignment with hub
    gwi->flit_rx(gflit_tx_local);
    gwi->req_rx(greq_tx_local);
    gwi->ack_rx(gack_tx_local);

    gwi->flit_tx(gflit_rx_local);
    gwi->req_tx(greq_rx_local);
    gwi->ack_tx(gack_rx_local);

    gwi->free_slots(gfree_slots_neighbor_local);
    gwi->free_slots_neighbor(gfree_slots_local);

    // Processing Element pin assignments
    pe = new TProcessingElement("ProcessingElement", b_mark);
    pe->clock(clock);
    pe->reset(reset);

    pe->flit_rx(flit_rx_local);
    pe->req_rx(req_rx_local);
    pe->ack_rx(ack_rx_local);

    pe->flit_tx(flit_tx_local);
    pe->req_tx(req_tx_local);
    pe->ack_tx(ack_tx_local);

    pe->free_slots(free_slots_local);
    pe->free_slots_neighbor(free_slots_neighbor_local);
    
     // Hub pin assignments
    h = new THub("Hub");
    h->clock(clock);
    h->reset(reset);
	// Hub pin assignments with pe
    h->flit_tx(flit_rx_local);
    h->req_tx(req_rx_local);
    h->ack_tx(ack_rx_local);

    h->flit_rx(flit_tx_local);
    h->req_rx(req_tx_local);
    h->ack_rx(ack_tx_local);

    h->free_slots(free_slots_neighbor_local);
    h->free_slots_neighbor(free_slots_local);
    // Hub pin assignments with GWI
    h->gflit_rx(gflit_rx_local);
    h->greq_rx(greq_rx_local);
    h->gack_rx(gack_rx_local);

    h->gflit_tx(gflit_tx_local);
    h->greq_tx(greq_tx_local);
    h->gack_tx(gack_tx_local);

    h->gfree_slots(gfree_slots_local);
    h->gfree_slots_neighbor(gfree_slots_neighbor_local);

  }

};

#endif
