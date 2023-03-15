#include "GlobalTypeDefs.h"
#include "processingelement.h"
#include "hub.h"

//---------------------------------------------------------------------------

void THub::proc_hub()
{	// passing incomming data from PE to GWI
   TFlit received_flit = flit_rx.read();
   if(TGlobalParams::verbose_mode > VERBOSE_OFF)
	   {
         cout<< sc_time_stamp().to_double()/1000  << "Source_Id --> " <<  received_flit.src_id <<" Destination_Id --> " <<  received_flit.dst_id << endl; 	   
	   }
   gflit_tx.write(flit_rx.read());
   greq_tx.write(req_rx.read());
   ack_rx.write(gack_tx.read());
   // passing incomming data from GWI to PE
   flit_tx.write(gflit_rx.read());
   req_tx.write(greq_rx.read());
   gack_rx.write(ack_tx.read());
   // passing free slots values
   free_slots.write(gfree_slots_neighbor.read());
   gfree_slots.write(free_slots_neighbor.read());

}
