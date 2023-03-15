#ifndef __NOC_H__
#define __NOC_H__

//---------------------------------------------------------------------------

#include <systemc.h>
#include "gitile.h"
#include "arbiter.h"
#include "gatewayinterface.h"
#include <vector>
#include <map>
#include <algorithm>

SC_MODULE(TNoC)
{

  // I/O Ports

  sc_in_clk        clock;        // The input clock for the NoC
  sc_in_clk        photonic_clock;        // The input clock for the NoC
  sc_in<bool>      reset;        // The reset signal for the NoC

  // Signals for crossbar
  sc_signal<TFlit>     cross_bar[MAX_STATIC_DIM * MAX_STATIC_DIM][4];
  
  sc_signal<TFlit>     cross_bar_additional;
  
  // Signals for Arbiter
  
  sc_signal<TFlit>		req_tx_arbiter_flit_signal[MAX_STATIC_DIM][MAX_STATIC_DIM];
  sc_signal<bool>		req_tx_arbiter_hf_signal[MAX_STATIC_DIM][MAX_STATIC_DIM];
  sc_signal<bool>		ack_tx_arbiter_hf_signal[MAX_STATIC_DIM][MAX_STATIC_DIM];
  
  sc_signal<bool>		req_rx_arbiter_ch_signal[MAX_STATIC_DIM][MAX_STATIC_DIM];
  sc_signal<int>		mwmr_channel_no_signal[MAX_STATIC_DIM][MAX_STATIC_DIM];
  sc_signal<bool>		ack_rx_arbiter_ch_signal[MAX_STATIC_DIM][MAX_STATIC_DIM];
  
  sc_signal<bool>		req_txc_arbiter_signal[MAX_STATIC_DIM][MAX_STATIC_DIM];
  sc_signal<bool>		ack_txc_arbiter_signal[MAX_STATIC_DIM][MAX_STATIC_DIM];
  sc_signal<bool>		ack_txc_gwi_signal[MAX_STATIC_DIM][MAX_STATIC_DIM];
  
  // Matrix of tiles

  TGITile*            git[DEFAULT_MESH_DIM_X][DEFAULT_MESH_DIM_Y];
  TGlobalarbiter*	  garbiter;
  // BENCHMARK INTIALIZATION
   benchmark              &b_mark;
  // Constructor

  TNoC(sc_module_name name, benchmark &_b_mark):
  b_mark(_b_mark)
  {
    // Build the Mesh
    buildCrossbar();
  }
  
// Support methods
  TGITile* searchNode(const int id) const;
  
  private:
  void buildCrossbar();
};

//---------------------------------------------------------------------------

#endif

