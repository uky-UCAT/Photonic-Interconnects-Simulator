#include "gatewayinterface.h"

/** This thread transfers flits to the lower electrical layer.
 *  Since the photonic communication comes to an end here , the 
 *  inter pri bit is reset here before sending to electrical router 
**/

void TGatewayInterface::delayCount()
{
	if (reset.read())
	{
		cycle_count =0;
	}
	else
	{
		cout<<"MAX_PHOTONIC_DELAY  :"<<MAX_PHOTONIC_DELAY<<endl;

				if (cycle_count > MAX_PHOTONIC_DELAY)
					cycle_count = 0;
				else
					cycle_count++;
	}

}

void TGatewayInterface::Optical_rxProcess() // Clock Edge
{
    if (reset.read())
    {
        for (int i = 0; i < TGlobalParams::mwmr_waveguides; i++) {
            for (int j = 0; j < 4; j++) {
                previous_transfer_photonic[i][j] = -10;
            }
        }

    }
    else
	{
        for (int i = 0; i < TGlobalParams::mwmr_waveguides; i++) {
            for (int j = 0; j < 4; j++) {
                TFlit received_flit_temp = mwmr_data_rx[i][j].read();
                if (received_flit_temp.dst_id == local_id_new && received_flit_temp.flit_type == FLIT_TYPE_HEAD
                		&& received_flit_temp.dst_id != received_flit_temp.src_id) {
                    if (previous_transfer_photonic[i][j] != received_flit_temp.timestamp && !buffer_receive.IsFull()) {
                        if (TGlobalParams::verbose_mode > VERBOSE_OFF) {
                            cout << sc_time_stamp().to_double() / 1000 << " gwi Source_Id --> " << received_flit_temp.src_id << " gwi Destination_Id --> " << received_flit_temp.dst_id << " Channel number : " << i << " Sub Channel Number: " << j << endl;
                        }
                        previous_transfer_photonic[i][j] = received_flit_temp.timestamp;
                        int src_cluster = received_flit_temp.src_id / 8;
                        int dst_cluster = received_flit_temp.dst_id / 8;
                        int slot_gap = dst_cluster - src_cluster;

                        buffer_receive.Push(received_flit_temp);
//  					  double optical_delay;
//  					  if(slot_gap==0)
//  					  {
//  						  optical_delay = 0.2;
//  					  }
//  					  if(slot_gap>0)
//  					  {
//  						  optical_delay = slot_gap*0.2;
//  					  }
//  					  if(slot_gap<0)
//  					  {
//  						  optical_delay = (8+slot_gap)*0.2;
//  					  }
//  					  //cout<< "optical_delay " << optical_delay<< endl;
//  					  TFlit stat_flit = received_flit_temp;
//
//  					  stats.receivedFlit((sc_time_stamp().to_double()/1000+ optical_delay*PHOTO_DEL_CYCLES), stat_flit);

  					stats.receivedFlit((sc_time_stamp().to_double()/1000), received_flit_temp);
                    }
                }
            }

        }
    }

}

void TGatewayInterface::txProcess() // sensitive -- photonic_to_electrical_tx
{
    if (reset.read()) {
        req_tx.write(0);
        current_level_tx = 0;
    }
    else 
    {

        if ( !buffer_receive.IsEmpty() && !reset.read() && ack_tx.read() == current_level_tx  )
        {
        TFlit received_flit = buffer_receive.Front();
cout << "src" << received_flit.src_id << "dst" << received_flit.dst_id << endl;
			cout<< "cycle count"<<cycle_count<<endl;

	if(cycle_count==PHOTONIC_DELAY){

	
        if(TGlobalParams::verbose_mode > VERBOSE_OFF)
        {
        cout << sc_time_stamp().to_double()/1000 << ": Photonic buffer["<< local_id_new <<"] Flit Source_Id --> " <<  received_flit.src_id <<" Flit Destination_Id --> " <<  received_flit.dst_id<< endl;
        }

        flit_tx.write(received_flit);		// passing the flit from photonic buffer to electircal domain
//        int src_cluster = received_flit.src_id/8;
//        int dst_cluster = received_flit.dst_id/8;
//        int slot_gap = dst_cluster-src_cluster;
//        double optical_delay;
//        if(slot_gap==0)
//        {
//        optical_delay = 0.2;
//        }
//        if(slot_gap>0)
//        {
//        optical_delay = slot_gap*0.2;
//        }
//        else
//        {
//        optical_delay = (8+slot_gap)*0.2;
//        }
//        //cout<< "optical_delay " << optical_delay<< endl;
//        stats.receivedFlit((sc_time_stamp().to_double()/1000+ PHOTO_DEL_CYCLES), received_flit);
        current_level_tx = 1-current_level_tx;
        req_tx.write(current_level_tx);
        buffer_receive.Pop();
        }
}
    }


}

/** This thread recieves  flits from the lower electrical layer and stores it
 * in the local buffer. Once a flit is pushed in the buffer ,process triggers
 * the photonic transfer process by notifying the reservation process
**/
void TGatewayInterface::rxProcess() // sensitive -- photonic_to_electrical_tx
{
    if (reset.read()) { // Clear outputs and indexes of receiving protocol
        ack_rx.write(0);
        current_level_rx = 0;
        routed_flits = 0;
        local_drained = 0;
    } else {

        if ((req_rx.read() == 1 - current_level_rx) && !buffer_local.IsFull()) {
            TFlit received_flit = flit_rx.read();

            if (TGlobalParams::verbose_mode > VERBOSE_OFF) {
                cout << sc_time_stamp().to_double() / 1000 << ": GWI[" << local_id_new << "], Input, Received flit: " << received_flit << endl;
            }

            // Store the incoming flit in the circular buffer
            buffer_local.Push(received_flit);

            // Negate the old value for Alternating Bit Protocol (ABP)
            current_level_rx = 1 - current_level_rx;
        }
        ack_rx.write(current_level_rx);

    }


}

/**--------------------------------------------------------------------------
 * This module writes on crossbar in MWSR fashion and trainsmits data to its destination 
 * based on the decision taken by arbiter 
 * 
---------------------------------------------------------------------------*/
void TGatewayInterface::mwmr_tx_data() // sensitive - Clock
{
    if (reset.read())
    {
        arbiter_current_level_rx_ch = 0;
        ack_rx_arbiter_ch.write(0);
    }

    else if (!reset.read())
    {
        if (!buffer_local.IsEmpty() && req_rx_arbiter_ch.read() == 1 - arbiter_current_level_rx_ch )
        {
	 
            TFlit tx_flit = buffer_local.Front(); 
cout << "src" << tx_flit.src_id << "dst" << tx_flit.dst_id << endl;
		cout<< "cycle count"<<cycle_count<<endl;


	if(cycle_count==PHOTONIC_DELAY){
		
	
  //sending head flit to the channel assigned by the arbiter
            int count = local_id_new / 16;
            mwmr_data_tx[mwmr_channel_no.read()][count].write(tx_flit);

            if (tx_flit.flit_type == FLIT_TYPE_HEAD) {
                arbiter_current_level_rx_ch = 1 - arbiter_current_level_rx_ch;
                if (TGlobalParams::verbose_mode > VERBOSE_OFF) {
                    cout << sc_time_stamp().to_double() / 1000 << ": GWI: Source_Id --> " << tx_flit.src_id << " Destination_Id --> " << tx_flit.dst_id << " channel number : " << mwmr_channel_no.read() << "Sub channel number : " << count << endl;
                }

            }
            buffer_local.Pop();
}
        }
        ack_rx_arbiter_ch.write(arbiter_current_level_rx_ch);
    }
}

///**--------------------------------------------------------------------------
//* This module requests arbiter to grant access to arbiter by sending its head flit
//*
//---------------------------------------------------------------------------*/
void TGatewayInterface::arbiter_req() // sensitive - Clock
{
    if (reset.read()) {
        req_tx_arbiter_hf.write(0);
        arbiter_current_level_tx_req = 0;
        previous_transfer = -10;

    } else {
        if (!reset.read() && ack_tx_arbiter_hf.read() == arbiter_current_level_tx_req && !buffer_local.IsEmpty()) {
            if (previous_transfer != buffer_local.Front().timestamp) {
                TFlit flit = buffer_local.Front();
                // sending head flit to arbiter
                req_tx_arbiter_flit.write(flit);
                previous_transfer = flit.timestamp;
                if (TGlobalParams::verbose_mode > VERBOSE_OFF) {
                    cout << sc_time_stamp().to_double() / 1000 << "From Request Source_Id --> " << flit.src_id << " Head_Destination_Id --> " << flit.dst_id << endl;
                }

                arbiter_current_level_tx_req = 1 - arbiter_current_level_tx_req;
                req_tx_arbiter_hf.write(arbiter_current_level_tx_req);
            }


        }
    }
}

void TGatewayInterface::bufferMonitor() {
    //if (reset.read())
    //{
    //free_slots.write(buffer_local.GetMaxBufferSize());
    //}
    //else
    //{
    //// update current input buffers level to Hub
    //free_slots.write(buffer_local.getCurrentFreeSlots());

    //}
}

//---------------------------------------------------------------------------
unsigned long TGatewayInterface::getRoutedFlits() {
    //return routed_flits;
}
