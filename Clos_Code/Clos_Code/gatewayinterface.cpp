#include "gatewayinterface.h"



void TGatewayInterface::delayCount()
{
	if (reset.read())
	{
		cycle_count =0;
	}
	else
	{
//		cycle_count++;
		if(cycle_count > PHOTONIC_DELAY)
			cycle_count =0;
		else
			cycle_count++;
	}

}

/** This thread transfers flits to the lower electrical layer.
 *  Since the photonic communication comes to an end here , the 
 *  inter pri bit is reset here before sending to electrical router 
 **/

void TGatewayInterface::clos_to_local_rx_process() // Clock Edge
{
	if (reset.read())
	{
		for(int i=0; i< TGlobalParams::clos_waveguides; i++)
		{
			previous_transfer_photonic[i] = -10;
			previous_transfer_srcid[i]  = -6;
		}

	}
	else
	{
		for(int i=0; i< TGlobalParams::clos_waveguides; i++)
		{
			TFlit received_flit_temp = clos_data_rx[i].read();
			if(received_flit_temp.dst_cluster_id == cluster_id )//&& received_flit_temp.flit_type == FLIT_TYPE_HEAD)
			{
				if(previous_transfer_photonic[i] != received_flit_temp.timestamp && !buffer_receive.IsFull() )
				{
					if(TGlobalParams::verbose_mode > VERBOSE_OFF)
					{
						cout<< sc_time_stamp().to_double()/1000 << " gwi Source_Cluster_Id --> " <<  received_flit_temp.src_cluster_id <<" gwi Destination_Cluster_Id --> " <<  received_flit_temp.dst_cluster_id <<" Port number : " << i << endl;
					}
					previous_transfer_photonic[i] = received_flit_temp.timestamp;
					previous_transfer_srcid[i] = received_flit_temp.src_id;
					buffer_receive.Push(received_flit_temp);
					stats.power.Buffering();

				}
				else
				{
					if(previous_transfer_srcid[i] != received_flit_temp.src_id && !buffer_receive.IsFull() )
					{
						if(TGlobalParams::verbose_mode > VERBOSE_OFF)
						{
							cout<< sc_time_stamp().to_double()/1000 << " gwi Source_Cluster_Id --> " <<  received_flit_temp.src_cluster_id <<" gwi Destination_Cluster_Id --> " <<  received_flit_temp.dst_cluster_id <<" Port number : " << i << endl;
						}
						previous_transfer_photonic[i] = received_flit_temp.timestamp;
						previous_transfer_srcid[i] = received_flit_temp.src_id;
						buffer_receive.Push(received_flit_temp);
						stats.power.Buffering();
					}


				}
			}

		}
		stats.power.Leakage();
	}

}

void TGatewayInterface::clos_to_local_tx_process() // sensitive -- photonic_to_electrical_tx
{
	if(reset.read())
	{
		for (int i = 0; i<HUBS_PER_CLUSTER; i++)
		{
			req_tx[i].write(0);
			current_level_tx[i] =0;
		}
	}
	else
	{

		if ( !buffer_receive.IsEmpty() && !reset.read() )
		{
			TFlit received_flit = buffer_receive.Front();
			if (ack_tx[0].read() == current_level_tx[0] && received_flit.dst_hub_id == (cluster_id*2))
			{
				if(TGlobalParams::verbose_mode > VERBOSE_OFF)
				{
					cout << sc_time_stamp().to_double()/1000 << ": Photonic buffer["<< cluster_id <<"] Flit Source_Id --> " <<  received_flit.src_id <<" Flit Destination_Id --> " <<  received_flit.dst_id<< endl;
				}

				flit_tx[0].write(received_flit);		// passing the flit from photonic buffer to electircal domain
				current_level_tx[0] = 1-current_level_tx[0];
				req_tx[0].write(current_level_tx[0]);
				buffer_receive.Pop();
			}
			else if (ack_tx[0].read() == current_level_tx[0] && received_flit.dst_hub_id == (cluster_id*2)+1)
			{
				if(TGlobalParams::verbose_mode > VERBOSE_OFF)
				{
					cout << sc_time_stamp().to_double()/1000 << ": Photonic buffer["<< cluster_id <<"] Flit Source_Id --> " <<  received_flit.src_id <<" Flit Destination_Id --> " <<  received_flit.dst_id<< endl;
				}

				flit_tx[1].write(received_flit);		// passing the flit from photonic buffer to electircal domain
				current_level_tx[1] = 1-current_level_tx[1];
				req_tx[1].write(current_level_tx[1]);
				buffer_receive.Pop();
			}
		}
		stats.power.Leakage(); 

	}
}

void TGatewayInterface::local_to_local_tx_process() // sensitive -- photonic_to_electrical_tx
{
	if(reset.read())
	{
		for (int i = 0; i<HUBS_PER_CLUSTER; i++)
		{
			req_tx[i].write(0);
			current_level_tx_local[i] =0;
		}
	}
	else
	{
		if ( !buffer_local[0].IsEmpty()  && cycle_count == PHOTONIC_DELAY)
		{
			//cout<<"debug_test"<<endl;
			TFlit received_flit = buffer_local[0].Front();
			if (ack_tx[1].read() == current_level_tx_local[1] && received_flit.dst_hub_id == (cluster_id*2)+1)
			{
				if(TGlobalParams::verbose_mode > VERBOSE_OFF)
				{
					cout << sc_time_stamp().to_double()/1000 << ": Photonic buffer["<< cluster_id <<"] Flit Source_Id --> " <<  received_flit.src_id <<" Flit Destination_Id --> " <<  received_flit.dst_id<< endl;
				}


				current_level_tx_local[1] = 1-current_level_tx_local[1];
				req_tx[1].write(current_level_tx_local[1]);
				if(1)
				{
					flit_tx[1].write(received_flit);		// passing the flit from photonic buffer to electircal domain
					buffer_local[0].Pop();
				}
				stats.power.Arbitration();
			}
		}
		else if ( !buffer_local[1].IsEmpty() && cycle_count == PHOTONIC_DELAY)
		{
			TFlit received_flit = buffer_local[1].Front();
			if (ack_tx[0].read() == current_level_tx_local[0] && received_flit.dst_hub_id == (cluster_id*2))
			{
				if(TGlobalParams::verbose_mode > VERBOSE_OFF)
				{
					cout << sc_time_stamp().to_double()/1000 << ": Photonic buffer["<< cluster_id <<"] Flit Source_Id --> " <<  received_flit.src_id <<" Flit Destination_Id --> " <<  received_flit.dst_id<< endl;
				}

				current_level_tx_local[0] = 1-current_level_tx_local[0];
				req_tx[0].write(current_level_tx_local[0]);
				if(1)
				{
					flit_tx[0].write(received_flit);		// passing the flit from photonic buffer to electircal domain
					buffer_local[1].Pop();

				}
				stats.power.Arbitration();
			}

		}
		stats.power.Leakage();
	}
}

/** This thread recieves  flits from the lower electrical layer and stores it 
 * in the local buffer. Once a flit is pushed in the buffer ,process triggers
 * the photonic transfer process by notifying the reservation process 
 **/
void TGatewayInterface::local_to_local_rx_process() // sensitive -- photonic_to_electrical_tx
{
	if(reset.read())
	{ // Clear outputs and indexes of receiving protocol

		for (int i = 0; i < HUBS_PER_CLUSTER; i++)
		{
			ack_rx[i].write(0);
			current_level_rx[i] = 0;
		}
		//reservation_table.clear();
		routed_flits = 0;
		local_drained = 0;
	}
	else
	{

		// For each channel decide if a new flit can be accepted
		//
		// This process simply sees a flow of incoming flits. All arbitration
		// and wormhole related issues are addressed in the txProcess()

		for (int i = 0; i < HUBS_PER_CLUSTER; i++)
		{
			// To accept a new flit, the following conditions must match:
			//
			// 1) there is an incoming request
			// 2) there is a free slot in the input buffer of direction i

			if ((req_rx[i].read() == 1 - current_level_rx[i])
					&& !buffer_local[i].IsFull())
			{
				TFlit received_flit = flit_rx[i].read();
				// Store the incoming flit in to buffer
				if(received_flit.src_id != received_flit.dst_id)
				{
					buffer_local[i].Push(received_flit);

					if (TGlobalParams::verbose_mode > VERBOSE_OFF)
					{

						cout << sc_time_stamp().to_double()/1000 << ": GWI with ClusterID[" << cluster_id << "], Input[" << i << "], Received flit: " << received_flit <<" pendind flits "<<buffer_local[i].
								Size()<< endl;
					}

					// Negate the old value for Alternating Bit Protocol (ABP)
					current_level_rx[i] = 1 - current_level_rx[i];

					// Incoming flit
					stats.power.Buffering();
				}
			}
			ack_rx[i].write(current_level_rx[i]);
		}

	}
	stats.power.Leakage(); 
}
/**--------------------------------------------------------------------------
 * This module writes on crossbar in Clos Network and trainsmits data to its destination 
 * based on the destination location 
 * 
---------------------------------------------------------------------------*/
void TGatewayInterface::local_to_clos_tx_process() // sensitive - Clock
{
	if(reset.read())
	{
		//		flitsThroughGWI=0;
	}

	else if(!reset.read() && cycle_count == PHOTONIC_DELAY )
	{

		for(int i=0; i<HUBS_PER_CLUSTER; i++)
		{
			if(!buffer_local[i].IsEmpty() )
			{
				TFlit tx_flit = buffer_local[i].Front();   //sending head flit to the channel assigned by the arbiter
				if(tx_flit.src_cluster_id != tx_flit.dst_cluster_id)
				{
					clos_data_tx[tx_flit.dst_cluster_id].write(tx_flit);
					if(TGlobalParams::verbose_mode > VERBOSE_OFF )
					{
						cout<< sc_time_stamp().to_double()/1000  << "Source_Cluster_Id --> " <<  tx_flit.src_cluster_id << " Cluster id " << cluster_id <<" Destination_Cluster_Id --> " <<  tx_flit.dst_cluster_id << endl;
					}
					buffer_local[i].Pop();
					//				if(tx_flit.flit_type == FLIT_TYPE_HEAD && tx_flit.dst_cluster_id!=tx_flit.src_cluster_id)
					//				flitsThroughGWI++;
					routed_flits++;

				}
			}
		}

	}
	stats.power.Leakage();
}

//---------------------------------------------------------------------------
unsigned long TGatewayInterface::getRoutedFlits()
{ 
	return routed_flits;
}

double TGatewayInterface::getPower()
{
	return stats.power.getPower();
}
