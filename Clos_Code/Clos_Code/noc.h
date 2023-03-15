#ifndef __NOC_H__
#define __NOC_H__

//---------------------------------------------------------------------------

#include <systemc.h>
#include "gitile.h"
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
  sc_signal<TFlit>     cross_bar[MAX_STATIC_DIM][MAX_STATIC_DIM];
  
  sc_signal<TFlit>     cross_bar_additional_input;
  sc_signal<TFlit>     cross_bar_additional_output;
 
  
  // Matrix of tiles

  TGITile*            git[DEFAULT_MESH_DIM_X][DEFAULT_MESH_DIM_Y]; // Generating a matrix of 2X4 tiles

  // BENCHMARK INTIALIZATION
   benchmark              &b_mark;
  // Constructor

  TNoC(sc_module_name name, benchmark &_b_mark):
  b_mark(_b_mark)
  {

    // Build the clos network
    buildCrossbar();
  }
  // Support methods
  TGITile* searchNode(const int id) const;
  
  private:
  void buildCrossbar();
};

//---------------------------------------------------------------------------

#endif

