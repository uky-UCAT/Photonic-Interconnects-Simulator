#include <iomanip>
#include "globalstatus.h"


//---------------------------------------------------------------------------
// Initialize global configuration parameters (can be overridden with command-line arguments)
int   TGlobalParams::verbose_mode = DEFAULT_VERBOSE_MODE;
int   TGlobalParams::trace_mode = DEFAULT_TRACE_MODE;
char  TGlobalParams::trace_filename[128] = DEFAULT_TRACE_FILENAME;
int   TGlobalParams::topology = MESH_2D;
int   TGlobalParams::mesh_dim_x = DEFAULT_MESH_DIM_X;
int   TGlobalParams::mesh_dim_y = DEFAULT_MESH_DIM_Y;
//int   TGlobalParams::mesh_dim_z                       = DEFAULT_MESH_DIM_Z;
int   TGlobalParams::buffer_depth = DEFAULT_BUFFER_DEPTH;
int   TGlobalParams::mwmr_waveguides = DEFAULT_MWMR_WAVEGUIDES;
int   TGlobalParams::min_packet_size = DEFAULT_MIN_PACKET_SIZE;
int   TGlobalParams::max_packet_size = DEFAULT_MAX_PACKET_SIZE;
int   TGlobalParams::routing_algorithm = DEFAULT_ROUTING_ALGORITHM;
char  TGlobalParams::routing_table_filename[128] = DEFAULT_ROUTING_TABLE_FILENAME;
int   TGlobalParams::selection_strategy = DEFAULT_SELECTION_STRATEGY;
float TGlobalParams::packet_injection_rate = DEFAULT_PACKET_INJECTION_RATE;
float TGlobalParams::probability_of_retransmission = DEFAULT_PROBABILITY_OF_RETRANSMISSION;
int   TGlobalParams::traffic_distribution = DEFAULT_TRAFFIC_DISTRIBUTION;
char  TGlobalParams::traffic_table_filename[128] = DEFAULT_TRAFFIC_TABLE_FILENAME;
int   TGlobalParams::simulation_time = DEFAULT_SIMULATION_TIME;
int   TGlobalParams::stats_warm_up_time = DEFAULT_STATS_WARM_UP_TIME;
int   TGlobalParams::rnd_generator_seed = time(NULL);
bool  TGlobalParams::detailed = DEFAULT_DETAILED;
float TGlobalParams::dyad_threshold = DEFAULT_DYAD_THRESHOLD;
unsigned int TGlobalParams::max_volume_to_be_drained = DEFAULT_MAX_VOLUME_TO_BE_DRAINED;
//int   TGlobalParams::real_traffic               	  = DEFAULT_REAL_TRAFFIC;
vector<pair<int, double> > TGlobalParams::hotspots;



//---------------------------------------------------------------------------

TGlobalStats::TGlobalStats(const TNoC *_noc) {
    noc = _noc;

#ifdef TESTING
    drained_total = 0;
#endif
}

//---------------------------------------------------------------------------

double TGlobalStats::getAverageDelay() {
    unsigned int total_packets = 0;
    double avg_delay = 0.0;

    for (int y = 0; y < TGlobalParams::mesh_dim_y; y++)
        for (int x = 0; x < TGlobalParams::mesh_dim_x; x++) {
            unsigned int received_packets = noc->git[x][y]->gwi->stats.getReceivedPackets();

            if (received_packets) {
                avg_delay += received_packets * noc->git[x][y]->gwi->stats.getAverageDelay();
                total_packets += received_packets;
            }
        }

    avg_delay /= (double) total_packets;

    return avg_delay;
}

//---------------------------------------------------------------------------

double TGlobalStats::getAverageDelay(const int src_id, const int dst_id) {
    TGITile *tile = noc->searchNode(dst_id);

    assert(tile != NULL);

    return tile->gwi->stats.getAverageDelay(src_id);
}

//---------------------------------------------------------------------------

double TGlobalStats::getMaxDelay() {
    double maxd = -1.0;

    for (int y = 0; y < TGlobalParams::mesh_dim_y; y++)
        for (int x = 0; x < TGlobalParams::mesh_dim_x; x++) {
            TCoord coord;
            coord.x = x;
            coord.y = y;
            int node_id = coord2newId(coord);
            double d = getMaxDelay(node_id);
            if (d > maxd)
                maxd = d;
        }

    return maxd;
}

//---------------------------------------------------------------------------

double TGlobalStats::getMaxDelay(const int node_id) {
    TCoord coord = newid2Coord(node_id);

    unsigned int received_packets = noc->git[coord.x][coord.y]->gwi->stats.getReceivedPackets();

    if (received_packets)
        return noc->git[coord.x][coord.y]->gwi->stats.getMaxDelay();
    else
        return -1.0;
}

//---------------------------------------------------------------------------

double TGlobalStats::getMaxDelay(const int src_id, const int dst_id) {
    TGITile *tile = noc->searchNode(dst_id);

    assert(tile != NULL);

    return tile->gwi->stats.getMaxDelay(src_id);
}

//---------------------------------------------------------------------------

vector<vector<double> > TGlobalStats::getMaxDelayMtx() {
    vector<vector<double> > mtx;

    mtx.resize(TGlobalParams::mesh_dim_y);
    for (int y = 0; y < TGlobalParams::mesh_dim_y; y++)
        mtx[y].resize(TGlobalParams::mesh_dim_x);

    for (int y = 0; y < TGlobalParams::mesh_dim_y; y++)
        for (int x = 0; x < TGlobalParams::mesh_dim_x; x++) {
            TCoord coord;
            coord.x = x;
            coord.y = y;
            int id = coord2Id(coord);
            mtx[y][x] = getMaxDelay(id);
        }

    return mtx;
}

//---------------------------------------------------------------------------

double TGlobalStats::getAverageThroughput(const int src_id, const int dst_id) {
    TGITile *tile = noc->searchNode(dst_id);

    assert(tile != NULL);

    return tile->gwi->stats.getAverageThroughput(src_id);
}

//---------------------------------------------------------------------------

double TGlobalStats::getAverageThroughput() {
    unsigned int total_comms = 0;
    double avg_throughput = 0.0;

    for (int y = 0; y < TGlobalParams::mesh_dim_y; y++)
        for (int x = 0; x < TGlobalParams::mesh_dim_x; x++) {
            unsigned int ncomms = noc->git[x][y]->gwi->stats.getTotalCommunications();
            //cout<<"["<<x<<"] "<<"[" << y <<"] "<< "local id: "<< noc->git[x][y]->gwi->local_id_new <<" Total Communication to this tile :"<< ncomms <<" Average TP: "<< noc->git[x][y]->gwi->stats.getAverageThroughput()<<endl;
            if (ncomms) {
                avg_throughput += ncomms * noc->git[x][y]->gwi->stats.getAverageThroughput();
                total_comms += ncomms;
            }
        }

    avg_throughput /= (double) total_comms;
    //cout << "AVG TP: "<< avg_throughput<<" Total Communications: "<< total_comms<< endl;

    return avg_throughput;
}

//---------------------------------------------------------------------------

unsigned int TGlobalStats::getReceivedPackets() {
    unsigned int n = 0;

    for (int y = 0; y < TGlobalParams::mesh_dim_y; y++)
        for (int x = 0; x < TGlobalParams::mesh_dim_x; x++)
            n += noc->git[x][y]->gwi->stats.getReceivedPackets();

    return n;
}

//---------------------------------------------------------------------------

unsigned int TGlobalStats::getReceivedFlits() {
    unsigned int n = 0;

    for (int y = 0; y < TGlobalParams::mesh_dim_y; y++)
        for (int x = 0; x < TGlobalParams::mesh_dim_x; x++) {
            n += noc->git[x][y]->gwi->stats.getReceivedFlits();
#ifdef TESTING
            drained_total+= noc->gitt[x][y]->gwi->local_drained;
#endif
        }

    return n;
}

//---------------------------------------------------------------------------

double TGlobalStats::getThroughput() {
    int total_cycles = TGlobalParams::simulation_time -
                       TGlobalParams::stats_warm_up_time;


    //  int number_of_ip = TGlobalParams::mesh_dim_x * TGlobalParams::mesh_dim_y;
    //  return (double)getReceivedFlits()/(double)(total_cycles * number_of_ip);

    unsigned int n = 0;
    unsigned int trf = 0;
    for (int y = 0; y < TGlobalParams::mesh_dim_y; y++)
        for (int x = 0; x < TGlobalParams::mesh_dim_x; x++) {
            unsigned int rf = noc->git[x][y]->gwi->stats.getReceivedFlits();

            if (rf != 0)
                n++;

            trf += rf;
        }
    return (double) trf / (double) (total_cycles * n);

}

//---------------------------------------------------------------------------

vector<vector<unsigned long> > TGlobalStats::getRoutedFlitsMtx() {

    vector<vector<unsigned long> > mtx;

    mtx.resize(TGlobalParams::mesh_dim_y);
    for (int y = 0; y < TGlobalParams::mesh_dim_y; y++)
        mtx[y].resize(TGlobalParams::mesh_dim_x);

    for (int y = 0; y < TGlobalParams::mesh_dim_y; y++)
        for (int x = 0; x < TGlobalParams::mesh_dim_x; x++)
            mtx[y][x] = noc->git[x][y]->gwi->getRoutedFlits();

    return mtx;
}

//---------------------------------------------------------------------------

double TGlobalStats::getPower() {
    double power = 26.0;

//    for(int y=0; y<TGlobalParams::mesh_dim_y; y++)
//    for(int x=0; x<TGlobalParams::mesh_dim_x; x++)
//    power += noc->git[x][y]->gwi->getPower();

    return power;
}

//---------------------------------------------------------------------------
double  TGlobalStats::getEnergyPerBit()
{
	double avgPow = getTxRxPower();//+ LaserPow + MR Heating + Electrical_Concentrator

	return (avgPow*NOC_PHOTONIC_CLK*pow(10,-9)/(getAverageThroughput() * 512));
}

//----------------------------------------------------------------------------

double  TGlobalStats::getTxRxPower()
{
//	int flitsize = 64;
//	double flitsPerCycle = getAverageThroughput();//getThroughput()*8*TGlobalParams::mesh_dim_y*TGlobalParams::mesh_dim_x; //flits/cycle
//	double flitsPerSec = flitsPerCycle/(NOC_PHOTONIC_CLK*pow(10,-9));
//	double bitsPerSec = flitsPerSec * flitsize;
//	double txRxpower = bitsPerSec * MOD_ENRG_PERBIT;

	double txRxpower = MOD_ENRG_PERBIT*getReceivedFlits() * 64/((DEFAULT_SIMULATION_TIME-DEFAULT_STATS_WARM_UP_TIME)*NOC_PHOTONIC_CLK*pow(10,-9));

	return txRxpower;
}

//---------------------------------------------------------------------------

void TGlobalStats::showStats(std::ostream &out, bool detailed) {
//    out << "% Traffic Pattern: " << DEFAULT_TRAFFIC_DISTRIBUTION << endl;
//    out << "% Number of MWMR Waveguides: " << DEFAULT_MWMR_WAVEGUIDES << endl;
    //out << "% Packet Injection rate: " << DEFAULT_PACKET_INJECTION_RATE  << endl;
    out << "% Total received packets: " << getReceivedPackets() << endl;
//    out << "% Total received flits: " << getReceivedFlits() * 8 << endl;
    out << "% Global average delay (cycles): " << getAverageDelay() << endl;
    out << "% Global average throughput (flits/cycle): " << getAverageThroughput() * 8 << endl;
//    out << "% Throughput (flits/cycle/IP): " << getThroughput() * 8 << endl;
    out << "% Energy per bit (J/bit):" << getEnergyPerBit() << endl;
    out << "% Average power dissipation (W):" << getTxRxPower() << endl;
    out << "% Max delay (cycles): " << getMaxDelay() << endl;
//    out << "% Total energy (J): " << getPower() << endl;

    if (detailed) {
        out << endl << "detailed = [" << endl;
        for (int y = 0; y < TGlobalParams::mesh_dim_y; y++)
            for (int x = 0; x < TGlobalParams::mesh_dim_x; x++)
                noc->git[x][y]->gwi->stats.showStats(y * TGlobalParams::mesh_dim_x + x,
                                                     out,
                                                     true);
        out << "];" << endl;

        // show MaxDelay matrix
        vector<vector<double> > md_mtx = getMaxDelayMtx();

        out << endl << "max_delay = [" << endl;
        for (unsigned int y = 0; y < md_mtx.size(); y++) {
            out << "   ";
            for (unsigned int x = 0; x < md_mtx[y].size(); x++)
                out << setw(6) << md_mtx[y][x];
            out << endl;
        }
        out << "];" << endl;

        // show RoutedFlits matrix
        vector<vector<unsigned long> > rf_mtx = getRoutedFlitsMtx();

        out << endl << "routed_flits = [" << endl;
        for (unsigned int y = 0; y < rf_mtx.size(); y++) {
            out << "   ";
            for (unsigned int x = 0; x < rf_mtx[y].size(); x++)
                out << setw(10) << rf_mtx[y][x];
            out << endl;
        }
        out << "];" << endl;
    }
}

//---------------------------------------------------------------------------

