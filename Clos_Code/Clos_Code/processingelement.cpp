#include "GlobalTypeDefs.h"
#include "processingelement.h"

//---------------------------------------------------------------------------

int TProcessingElement::randInt(int min, int max)
{
	return min + (int)((double)(max-min+1) * rand()/(RAND_MAX+1.0));
}

//---------------------------------------------------------------------------

void TProcessingElement::rxProcess() // receiving process
{


	double optical_delay =0;
	if(reset.read())
	{
		ack_rx.write(0);
		current_level_rx = 0;
	}
	else
	{
		if(req_rx.read()==1-current_level_rx)
		{
			TFlit flit_tmp = flit_rx.read();

				//			TFlit stat_flit = flit_tmp;
				//			stat_flit.src_id = stat_flit.src_hub_id;
				//			stat_flit.dst_id = stat_flit.dst_hub_id;

				//Photonic Delay is calculated for the entire packet, hence only the header is given the total brunt of photonic delay
				//If the source and dstntn are the same, then there is no need to calculate the optical delay as waveguides willnot be involved
				int src_cluster = flit_tmp.src_cluster_id;
				int dst_cluster = flit_tmp.dst_cluster_id;
				if( src_cluster!=dst_cluster )//&& flit_tmp.flit_type == FLIT_TYPE_HEAD )
				{
				//	cout << "Flit source cluster:"<< src_cluster << " Flit destination:" << dst_cluster <<endl;
					//optical_delay=  PHOTONIC_DELAY;
				}
				else
				{
					//cout << "SAME CLUSTER FLIT" <<endl;
				}
				if (flit_tmp.timestamp >0)
					stats.receivedFlit(sc_time_stamp().to_double()/1000 + optical_delay, flit_tmp);
				else
					stats.receivedFlit(sc_time_stamp().to_double()/1000 - DEFAULT_RESET_TIME + optical_delay, flit_tmp);

				// 		  if(src_cluster!=dst_cluster )

				if(TGlobalParams::verbose_mode > VERBOSE_OFF)
				{
					cout << sc_simulation_time() << ": ProcessingElement[" << local_id << "] RECEIVING" << flit_tmp <<" Sent at:"<< flit_tmp.timestamp << endl;
				}
				current_level_rx = 1-current_level_rx;     // Negate the old value for Alternating Bit Protocol (ABP)
			}
			ack_rx.write(current_level_rx);

	}
}

//---------------------------------------------------------------------------

void TProcessingElement::txProcess()
{
	if(reset.read())
	{
		tx_cycle_count=0;
		req_tx.write(0);
		current_level_tx = 0;
		transmittedAtPreviousCycle = false;
	}
	else
	{
		TPacket packet;

		if (canShot(packet))
		{
			packet_queue.push(packet);

			transmittedAtPreviousCycle = true;
		}
		else
			transmittedAtPreviousCycle = false;


		if(ack_tx.read() == current_level_tx )
		{
			if(!packet_queue.empty())
			{
				TFlit flit = nextFlit();                  // Generate a new flit
				if(flit.timestamp > 0)
				{
					if(TGlobalParams::verbose_mode > VERBOSE_OFF)
					{
						cout << sc_time_stamp().to_double()/1000 << ": ProcessingElement[" << local_id << "] SENDING " << flit << endl;
					}
					flit_tx->write(flit);                     // Send the generated flit
					//	flits_generated ++;
					if(TGlobalParams::verbose_mode > VERBOSE_OFF)
					{
						cout<< sc_time_stamp().to_double()/1000  <<"localid: "<< local_id<< " Source_Id --> " <<  flit.src_id <<" Destination_Id --> " <<  flit.dst_id << endl;
					}
					current_level_tx = 1-current_level_tx;    // Negate the old value for Alternating Bit Protocol (ABP)
					req_tx.write(current_level_tx);
				}
			}
		}
	}
}

//---------------------------------------------------------------------------

TFlit TProcessingElement::nextFlit()
{
	TFlit   flit;
	TPacket packet = packet_queue.front();
	flit.src_id      = packet.src_id;
	flit.dst_id      = packet.dst_id;
	flit.src_hub_id      = packet.src_hub_id;
	flit.dst_hub_id      = packet.dst_hub_id;
	flit.src_cluster_id      = packet.src_cluster_id;
	flit.dst_cluster_id      = packet.dst_cluster_id;
	flit.timestamp   = packet.timestamp;
	flit.sequence_no = packet.size - packet.flit_left;
	flit.hop_no      = 0;
	//  flit.payload     = DEFAULT_PAYLOAD;
	//cout << "packet.size == packet.flit_left" << packet.size <<"=="<< packet.flit_left << endl;
	//cout <<"Packet from :"<< flit.src_id <<endl;
	if(packet.size == packet.flit_left)
		flit.flit_type = FLIT_TYPE_HEAD;
	else if(packet.flit_left == 1)
	{
		flit.flit_type = FLIT_TYPE_TAIL;
		//cout<<"Tail flit generated"<<endl;
	}
	else
		flit.flit_type = FLIT_TYPE_BODY;

	packet.flit_left--;
	packet_queue.front().flit_left--;
	//cout << "Flits left:" << packet.flit_left << ":"<< packet_queue.front().flit_left << endl;

	if(packet.flit_left == 0)
		packet_queue.pop();
	return flit;

}

//---------------------------------------------------------------------------

bool TProcessingElement::canShot(TPacket& packet)
{
	bool   shot;
	double threshold;

	if (TGlobalParams::traffic_distribution != TRAFFIC_TABLE_BASED)
	{
		if (!transmittedAtPreviousCycle)
			threshold = TGlobalParams::packet_injection_rate;
		else
			threshold = TGlobalParams::probability_of_retransmission;
		if (TGlobalParams::real_traffic == 1)
		{
			shot = b_mark.packet_injection_response(local_id);
			if(shot==1)
			{
				//cout<< sc_time_stamp().to_double()/1000 <<"localid: "<< local_id << " shot value: "<<shot<<endl;
			}
		}
		else
		{
			shot = (((double)rand())/RAND_MAX < threshold);
		}
		if (shot)
		{
			switch(TGlobalParams::traffic_distribution)
			{
			case TRAFFIC_RANDOM:
				packet = trafficRandom();
				break;

			case TRAFFIC_TRANSPOSE1:
				packet = trafficTranspose1();
				break;

			case TRAFFIC_TRANSPOSE2:
				packet = trafficTranspose2();
				break;

			case TRAFFIC_BIT_REVERSAL:
				packet = trafficBitReversal();
				break;

			case TRAFFIC_SHUFFLE:
				packet = trafficShuffle();
				break;

			case TRAFFIC_BUTTERFLY:
				packet = trafficButterfly();
				break;
			case TRAFFIC_HOTSPOT:
				packet = hotspot();
				break;
			case TRAFFIC_BENCHMARK:
				packet = benchmarktraffic();
				break;

			default:
				assert(false);
			}

		}
	}
	//else
	//{ // Table based communication traffic
	//if (never_transmit)
	//return false;

	//double now         = sc_time_stamp().to_double()/1000;
	//bool   use_pir     = (transmittedAtPreviousCycle == false);
	//vector<pair<int,double> > dst_prob;
	//double threshold = traffic_table->getCumulativePirPor(local_id, (int)now, use_pir, dst_prob);

	//double prob = (double)rand()/RAND_MAX;
	//shot = (prob < threshold);
	//if (shot)
	//{
	//for (unsigned int i=0; i<dst_prob.size(); i++)
	//{
	//if (prob < dst_prob[i].second)
	//{
	//packet.make(local_id, dst_prob[i].first, now, getRandomSize());
	//break;
	//}
	//}
	//}
	//}
	if(TGlobalParams::verbose_mode > VERBOSE_OFF && shot ==1)
	{
		cout<< sc_time_stamp().to_double()/1000  <<"localid: "<< local_id<< " in shot packet Source_Id --> " <<  packet.src_id <<" in shot packet Destination_Id --> " <<  packet.dst_id << endl;
	}
	return shot;
}

//---------------------------------------------------------------------------

TPacket TProcessingElement::trafficRandom()
{
	TPacket p;
	p.src_id = local_id;
	p.src_cluster_id = cluster_id;
	p.src_hub_id = hub_id;
	double rnd = rand()/(double)RAND_MAX;
	double range_start = 0.0;

	//cout << "\n " << sc_time_stamp().to_double()/1000 << " PE " << local_id << " rnd = " << rnd << endl;

	int max_id = (TGlobalParams::mesh_dim_x * TGlobalParams::mesh_dim_y)-1;

	// Random destination distribution
	do
	{
		p.dst_cluster_id = randInt(0, max_id);


		// check for hotspot destination
		for (uint i = 0; i<TGlobalParams::hotspots.size(); i++)
		{
			//cout << sc_time_stamp().to_double()/1000 << " PE " << local_id << " Checking node " << TGlobalParams::hotspots[i].first << " with P = " << TGlobalParams::hotspots[i].second << endl;

			if (rnd>=range_start && rnd < range_start + TGlobalParams::hotspots[i].second)
			{
				if (local_id != TGlobalParams::hotspots[i].first)
				{
					//cout << sc_time_stamp().to_double()/1000 << " PE " << local_id <<" That is ! " << endl;
					p.dst_cluster_id = TGlobalParams::hotspots[i].first;
				}
				break;
			}
			else
				range_start+=TGlobalParams::hotspots[i].second; // try next
		}
		p.dst_id =  p.dst_cluster_id*randInt(0,7);
		p.dst_hub_id =p.dst_cluster_id*randInt(0,1);
	}while(p.dst_cluster_id==p.src_cluster_id);

	//  do
	//  {
	//	  p.dst_cluster_id = randInt(0, max_id);
	//	  p.dst_id =  p.dst_cluster_id*randInt(0,7);
	//	  p.dst_hub_id =p.dst_cluster_id*randInt(0,1);
	//  }while(p.dst_cluster_id==p.src_cluster_id);


	p.timestamp = sc_time_stamp().to_double()/1000;
	p.size = p.flit_left = getRandomSize();

	//cout << "Packet Size:" <<p.size <<endl;

	return p;
}
//benchmark traffic distribution

TPacket TProcessingElement::benchmarktraffic()
{
	//cout<< "benchmark functionaccessed "<<endl;
	TPacket p;
	p.src_id = local_id;
	p.src_hub_id = hub_id;
	p.src_cluster_id = cluster_id;

	TCoord src,dst;
	if(local_id % 8 != 0)
	{
		if(cluster_id != 7)
		{
			p.dst_id = (cluster_id+1)*8;
			p.dst_hub_id = p.dst_id/4;
			p.dst_cluster_id = p.dst_id/8;
		}
		else if(cluster_id == 7)
		{
			p.dst_id = 0;
			p.dst_hub_id = p.dst_id/4;
			p.dst_cluster_id = p.dst_id/8;
		}
	}
	else if(local_id % 8 == 0)
	{
		if(local_id != 0)
		{
			int k = randInt(local_id, local_id-7);
			p.dst_id = k;
			p.dst_hub_id = p.dst_id/4;
			p.dst_cluster_id = p.dst_id/8;
		}
		else
		{
			int k = randInt(0, 63);
			p.dst_id = k;
			p.dst_hub_id = p.dst_id/4;
			p.dst_cluster_id = p.dst_id/8;
		}
	}
	p.timestamp = sc_time_stamp().to_double()/1000;
	p.size = p.flit_left = getRandomSize();
	if(TGlobalParams::verbose_mode > VERBOSE_OFF)
	{
		cout<< sc_time_stamp().to_double()/1000  <<"localid: "<< local_id<< " in packet Source_Id --> " <<  p.src_id <<" in packet Destination_Id --> " <<  p.dst_id << endl;
	}
	return p;
}
//---------------------------------------------------------------------------

TPacket TProcessingElement::trafficTranspose1()
{
	TPacket p;
	p.src_id = local_id;
	TCoord src,dst;

	// Transpose 1 destination distribution
	src.x = id2Coord(p.src_id).x;
	src.y = id2Coord(p.src_id).y;
	dst.x = TGlobalParams::mesh_dim_x-1-src.y;
	dst.y = TGlobalParams::mesh_dim_y-1-src.x;
	fixRanges(src, dst);
	p.dst_id = coord2Id(dst);

	p.timestamp = sc_time_stamp().to_double()/1000;
	p.size = p.flit_left = getRandomSize();

	return p;
}

TPacket TProcessingElement::hotspot()
{
	TPacket p;
	p.src_id = local_id;
	if (p.src_id != 1)
	{
		p.dst_id = 1;
	}
	else
	{
		p.dst_id = 5;
	}

	p.timestamp = sc_time_stamp().to_double()/1000;
	p.size = p.flit_left = getRandomSize();

	return p;
}

//---------------------------------------------------------------------------

TPacket TProcessingElement::trafficTranspose2() // Tpacket is th data type
{
	TPacket p;
	p.src_id = local_id;
	TCoord src,dst;

	// Transpose 2 destination distribution
	src.x = id2Coord(p.src_id).x;
	src.y = id2Coord(p.src_id).y;
	dst.x = src.y;
	dst.y = src.x;
	fixRanges(src, dst);
	p.dst_id = coord2Id(dst);

	p.timestamp = sc_time_stamp().to_double()/1000;
	p.size = p.flit_left = getRandomSize();

	return p;
}

//---------------------------------------------------------------------------

void TProcessingElement::setBit(int &x, int w, int v)
{
	int mask = 1 << w;

	if (v == 1)
		x = x | mask;
	else if (v == 0)
		x = x & ~mask;
	else
		assert(false);
}

//---------------------------------------------------------------------------

int TProcessingElement::getBit(int x, int w)
{
	return (x >> w) & 1;
}

//---------------------------------------------------------------------------

inline double TProcessingElement::log2ceil(double x)
{
	return ceil(log(x)/log(2.0));
}

//---------------------------------------------------------------------------

TPacket TProcessingElement::trafficBitReversal()
{

	int nbits = (int)log2ceil((double)(TGlobalParams::mesh_dim_x*TGlobalParams::mesh_dim_y));
	int dnode = 0;
	for (int i=0; i<nbits; i++)
		setBit(dnode, i, getBit(local_id, nbits-i-1));

	TPacket p;
	p.src_id = local_id;
	p.dst_id = dnode;

	p.timestamp = sc_time_stamp().to_double()/1000;
	p.size = p.flit_left = getRandomSize();

	return p;
}

//---------------------------------------------------------------------------

TPacket TProcessingElement::trafficShuffle()
{

	int nbits = (int)log2ceil((double)(TGlobalParams::mesh_dim_x*TGlobalParams::mesh_dim_y));
	int dnode = 0;
	for (int i=0; i<nbits-1; i++)
		setBit(dnode, i+1, getBit(local_id, i));
	setBit(dnode, 0, getBit(local_id, nbits-1));

	TPacket p;
	p.src_id = local_id;
	p.dst_id = dnode;

	p.timestamp = sc_time_stamp().to_double()/1000;
	p.size = p.flit_left = getRandomSize();

	return p;
}

//---------------------------------------------------------------------------

TPacket TProcessingElement::trafficButterfly()
{

	int nbits = (int)log2ceil((double)(TGlobalParams::mesh_dim_x*TGlobalParams::mesh_dim_y));
	int dnode = 0;
	for (int i=1; i<nbits-1; i++)
		setBit(dnode, i, getBit(local_id, i));
	setBit(dnode, 0, getBit(local_id, nbits-1));
	setBit(dnode, nbits-1, getBit(local_id, 0));

	TPacket p;
	p.src_id = local_id;
	p.dst_id = dnode;

	p.timestamp = sc_time_stamp().to_double()/1000;
	p.size = p.flit_left = getRandomSize();

	return p;
}

//---------------------------------------------------------------------------

void TProcessingElement::fixRanges(const TCoord src, TCoord& dst)
{
	// Fix ranges
	if(dst.x<0) dst.x=0;
	if(dst.y<0) dst.y=0;
	if(dst.x>=TGlobalParams::mesh_dim_x) dst.x=TGlobalParams::mesh_dim_x-1;
	if(dst.y>=TGlobalParams::mesh_dim_y) dst.y=TGlobalParams::mesh_dim_y-1;
}

//---------------------------------------------------------------------------

int TProcessingElement::getRandomSize()
{
	return randInt(TGlobalParams::min_packet_size,TGlobalParams::max_packet_size);
}

double TProcessingElement::getFlitsGenerated()
{
	return flits_generated;
}

//---------------------------------------------------------------------------
