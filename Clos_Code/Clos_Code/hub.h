#ifndef __HUB_H__
#define __HUB_H__

//---------------------------------------------------------------------------

#include <cassert>
#include <queue>
#include "GlobalTypeDefs.h"
#include "reservationtable.h"
#include "buffer.h"
#include "status.h"

using namespace std;

//---------------------------------------------------------------------------

SC_MODULE(THub)
{

  // I/O Ports

  sc_in_clk            clock;        // The input clock for the HUB
  sc_in<bool>          reset;        // The reset signal for the HUB

  sc_in<TFlit>         flit_rx[DIRECTIONS+1];      // The input channel from PE
  sc_in<bool>          req_rx[DIRECTIONS+1];       // The request associated with the input channel to PE
  sc_out<bool>         ack_rx[DIRECTIONS+1];       // The outgoing ack signal associated with the input channel to PE

  sc_out<TFlit>        flit_tx[DIRECTIONS+1];      // The output channel to PE
  sc_out<bool>         req_tx[DIRECTIONS+1];       // The request associated with the output channel to PE
  sc_in<bool>          ack_tx[DIRECTIONS+1];       // The outgoing ack signal associated with the output channel to PE

  sc_out<int>         free_slots[DIRECTIONS+1];
  sc_in<int>          free_slots_neighbor[DIRECTIONS+1];
  
  int hub_id;		                // Unique hub ID
  int cluster_id;		                // Unique hub ID
  
  int tx_cycle_count;
  int rx_cycle_count;

  TBuffer buffer[DIRECTIONS+1];	        // Buffer for each input channel 
  bool current_level_rx[DIRECTIONS+1];	// Current level for Alternating Bit Protocol (ABP)
  bool current_level_tx[DIRECTIONS+1];	// Current level for Alternating Bit Protocol (ABP)
  int routing_type;		                // Type of routing algorithm
  int selection_type;
  TStats             stats;                           // Statistics
  reservationtable reservation_table;	// Switch reservation table
  int start_from_port;	                // Port from which to start the reservation cycle
  unsigned long routed_flits;

  // Functions
  void rxProcess();
  void txProcess();
  unsigned long getRoutedFlits();	// Returns the number of routed flits 
  unsigned int getFlitsCount();	// Returns the number of flits into the router
  double getPower();		        // Returns the total power dissipated by the router

   SC_CTOR(THub)
  {
    SC_METHOD(rxProcess);
    sensitive << reset;
    sensitive << clock.pos();
    SC_METHOD(txProcess);
    sensitive << reset;
    sensitive << clock.pos();
   }
   private:

    // performs actual routing + selection
    int route(const TRouteData & route_data);

    // wrappers
    int routingFunction(const TRouteData & route_data);

    // selection strategies
    int selectionRandom(const vector <int> & directions);


  public:

    unsigned int local_drained;

 };

#endif
