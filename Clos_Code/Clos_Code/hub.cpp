#include "GlobalTypeDefs.h"
#include "processingelement.h"
#include "hub.h"

//---------------------------------------------------------------------------
void THub::rxProcess()
{
	if (reset.read())
	{
		rx_cycle_count =0;
		//Clear outputs and indexes of receiving protocol
		for (int i = 0; i < DIRECTIONS+1; i++)
		{
			ack_rx[i].write(0);
			current_level_rx[i] = 0;
		}
		reservation_table.clear();
		routed_flits = 0;
		local_drained = 0;
	}
	else
	{
		// For each channel decide if a new flit can be accepted
		//
		// This process simply sees a flow of incoming flits. All arbitration
		// and wormhole related issues are addressed in the txProcess()

		for (int i = 0; i < DIRECTIONS+1; i++)
		{
			// To accept a new flit, the following conditions must match:
			//
			// 1) there is an incoming request
			// 2) there is a free slot in the input buffer of direction i

			if ((req_rx[i].read() == 1 - current_level_rx[i]) && !buffer[i].IsFull() )
			{
				rx_cycle_count=0;
				TFlit received_flit = flit_rx[i].read();
				// Store the incoming flit in to buffer
				//        if(received_flit.src_id != received_flit.dst_id)
				//        {
				buffer[i].Push(received_flit);
				//        }
				if (TGlobalParams::verbose_mode > VERBOSE_OFF)
				{

					cout << sc_time_stamp().to_double()/1000 << ": HUBID[" << hub_id << "], Input[" << i << "], Received flit: " << received_flit <<" pendind flits "<<buffer[i].
							Size()<< endl;
				}

				// Negate the old value for Alternating Bit Protocol (ABP)
				current_level_rx[i] = 1 - current_level_rx[i];

				// Incoming flit
				stats.power.Buffering();

			}
			ack_rx[i].write(current_level_rx[i]);
		}


	}
	stats.power.Leakage();
}

void THub::txProcess()
{
	if (reset.read())
	{
		tx_cycle_count=0;
		// Clear outputs and indexes of transmitting protocol
		for (int i = 0; i < DIRECTIONS+1; i++)
		{
			req_tx[i].write(0);
			current_level_tx[i] = 0;
		}
		start_from_port =0;
	}
	else
	{
		// 1st phase: Reservation
		for (int j = 0; j < (DIRECTIONS+1); j++)
		{
			int i = (start_from_port + j) % (DIRECTIONS+1);

			if (!buffer[i].IsEmpty())
			{
				TFlit flit = buffer[i].Front();

				//        if (flit.flit_type == FLIT_TYPE_HEAD)
				//        {
				// prepare data for routing
				TRouteData route_data;
				route_data.current_id = hub_id;
				route_data.src_id = flit.src_id;
				route_data.dst_id = flit.dst_id;
				route_data.dir_in = i;

				int o = route(route_data);

				stats.power.Arbitration();

				if (reservation_table.isAvailable(o))
				{
					routed_flits++;
					stats.power.Crossbar();
					reservation_table.reserve(i, o);
					if (TGlobalParams::verbose_mode > VERBOSE_OFF)
					{
						cout << sc_time_stamp().to_double() / 1000 << ": HUBTX[" << hub_id << "], Input[" << i << "] (" << buffer[i].Size() << " flits)" << ", reserved Output["<< o << "], flit: " << flit << endl;
					}
				}
				//        }
			}
		}
		start_from_port++;

		// 2nd phase: Forwarding
		for (int i = 0; i < DIRECTIONS+1; i++)
		{
			if (!buffer[i].IsEmpty())
			{
				TFlit flit = buffer[i].Front();

				int o = reservation_table.getOutputPort(i);
				if (o != NOT_RESERVED)
				{
					if (current_level_tx[o] == ack_tx[o].read())
					{
						flit_tx[o].write(flit);
						current_level_tx[o] = 1 - current_level_tx[o];
						req_tx[o].write(current_level_tx[o]);
						buffer[i].Pop();

						if (TGlobalParams::verbose_mode > VERBOSE_OFF)
						{
							cout << sc_time_stamp().to_double() / 1000 << ": Router[" << hub_id << "], Input[" << i <<"] forward to Output[" << o << "], flit: "<< flit <<" (" << buffer[i].Size() << " flits)"<< endl;
						}

						stats.power.Link(false);
						reservation_table.release(o);

					}
				}
			}
		}

	}
	stats.power.Leakage();
}

int THub::route(const TRouteData & route_data)
{
	stats.power.Routing();

	int direction = routingFunction(route_data);
	return direction;
}

int THub::routingFunction(const TRouteData & route_data)
{
	int Port_id_inter = route_data.dst_id - (4 * hub_id); // reduces destination id to local port id
	int dir_out;
	if(Port_id_inter>=0 && Port_id_inter <= 3)
	{
		dir_out = Port_id_inter;
	}
	else
	{
		dir_out = 4;
	}


	return dir_out;
}

//---------------------------------------------------------------------------

int THub::selectionRandom(const vector < int >&directions)
{
	return directions[rand() % directions.size()];
}

unsigned long THub::getRoutedFlits()
{
	return routed_flits;
}

unsigned int THub::getFlitsCount()
{
	unsigned count = 0;

	for (int i = 0; i < DIRECTIONS; i++)
		count += buffer[i].Size();

	return count;
}

double THub::getPower()
{
	return stats.power.getPower();
}




