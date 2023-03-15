#include "cmdlineparser.h"

void parseCmdLine(int arg_num, char *arg_vet[])
{
    //if (arg_num == 1)
	//cout <<
	    //"Running with default parameters (use '-help' option to see how to override them)"
	    //<< endl;
    //else {
	//for (int i = 1; i < arg_num; i++) {
	    //if (!strcmp(arg_vet[i], "-help")) {
		//showHelp(arg_vet[0]);
		//exit(0);
	    //} else if (!strcmp(arg_vet[i], "-verbose"))
		//NoximGlobalParams::verbose_mode = atoi(arg_vet[++i]);
	    //else if (!strcmp(arg_vet[i], "-trace")) {
		//NoximGlobalParams::trace_mode = true;
		//strcpy(NoximGlobalParams::trace_filename, arg_vet[++i]);
	    //} else if (!strcmp(arg_vet[i], "-dimx"))
		//NoximGlobalParams::mesh_dim_x = atoi(arg_vet[++i]);
	    //else if (!strcmp(arg_vet[i], "-dimy"))
		//NoximGlobalParams::mesh_dim_y = atoi(arg_vet[++i]);
	    //else if (!strcmp(arg_vet[i], "-buffer"))
		//NoximGlobalParams::buffer_depth = atoi(arg_vet[++i]);
	    //else if (!strcmp(arg_vet[i], "-size")) {
		//NoximGlobalParams::min_packet_size = atoi(arg_vet[++i]);
		//NoximGlobalParams::max_packet_size = atoi(arg_vet[++i]);
	    //} else if (!strcmp(arg_vet[i], "-routing")) {
		//char *routing = arg_vet[++i];
		//if (!strcmp(routing, "xy"))
		    //NoximGlobalParams::routing_algorithm = ROUTING_XY;
		//else if (!strcmp(routing, "yx"))
		    //NoximGlobalParams::routing_algorithm = ROUTING_YX;
		//else if (!strcmp(routing, "westfirst"))
		    //NoximGlobalParams::routing_algorithm =
			//ROUTING_WEST_FIRST;
		//else if (!strcmp(routing, "northlast"))
		    //NoximGlobalParams::routing_algorithm =
			//ROUTING_NORTH_LAST;
		//else if (!strcmp(routing, "negativefirst"))
		    //NoximGlobalParams::routing_algorithm =
			//ROUTING_NEGATIVE_FIRST;
		//else if (!strcmp(routing, "oddeven"))
		    //NoximGlobalParams::routing_algorithm =
			//ROUTING_ODD_EVEN;
		//else if (!strcmp(routing, "dyad")) {
		    //NoximGlobalParams::routing_algorithm = ROUTING_DYAD;
		    //NoximGlobalParams::dyad_threshold = atof(arg_vet[++i]);
		//} else if (!strcmp(routing, "fullyadaptive"))
		    //NoximGlobalParams::routing_algorithm =
			//ROUTING_FULLY_ADAPTIVE;
		//else if (!strcmp(routing, "table")) {
		    //NoximGlobalParams::routing_algorithm =
			//ROUTING_TABLE_BASED;
		    //strcpy(NoximGlobalParams::routing_table_filename,
			   //arg_vet[++i]);
		    //NoximGlobalParams::packet_injection_rate = 0;	// ??? why ???
		//} else
		    //NoximGlobalParams::routing_algorithm = INVALID_ROUTING;
	    //} else if (!strcmp(arg_vet[i], "-sel")) {
		//char *selection = arg_vet[++i];
		//if (!strcmp(selection, "random"))
		    //NoximGlobalParams::selection_strategy = SEL_RANDOM;
		//else if (!strcmp(selection, "bufferlevel"))
		    //NoximGlobalParams::selection_strategy =
			//SEL_BUFFER_LEVEL;
		//else if (!strcmp(selection, "nop"))
		    //NoximGlobalParams::selection_strategy = SEL_NOP;
		//else
		    //NoximGlobalParams::selection_strategy =
			//INVALID_SELECTION;
	    //} else if (!strcmp(arg_vet[i], "-pir")) {
		//NoximGlobalParams::packet_injection_rate =
		    //atof(arg_vet[++i]);
		//char *distribution = arg_vet[++i];
		//if (!strcmp(distribution, "poisson"))
		    //NoximGlobalParams::probability_of_retransmission =
			//NoximGlobalParams::packet_injection_rate;
		//else if (!strcmp(distribution, "burst")) {
		    //float burstness = atof(arg_vet[++i]);
		    //NoximGlobalParams::probability_of_retransmission =
			//NoximGlobalParams::packet_injection_rate / (1 -
								    //burstness);
		//} else if (!strcmp(distribution, "pareto")) {
		    //float Aon = atof(arg_vet[++i]);
		    //float Aoff = atof(arg_vet[++i]);
		    //float r = atof(arg_vet[++i]);
		    //NoximGlobalParams::probability_of_retransmission =
			//NoximGlobalParams::packet_injection_rate *
			//pow((1 - r), (1 / Aoff - 1 / Aon));
		//} else if (!strcmp(distribution, "custom"))
		    //NoximGlobalParams::probability_of_retransmission =
			//atof(arg_vet[++i]);
	    //} else if (!strcmp(arg_vet[i], "-traffic")) {
		//char *traffic = arg_vet[++i];
		//if (!strcmp(traffic, "random"))
		    //NoximGlobalParams::traffic_distribution =
			//TRAFFIC_RANDOM;
		//else if (!strcmp(traffic, "transpose1"))
		    //NoximGlobalParams::traffic_distribution =
			//TRAFFIC_TRANSPOSE1;
		//else if (!strcmp(traffic, "transpose2"))
		    //NoximGlobalParams::traffic_distribution =
			//TRAFFIC_TRANSPOSE2;
		//else if (!strcmp(traffic, "bitreversal"))
		    //NoximGlobalParams::traffic_distribution =
			//TRAFFIC_BIT_REVERSAL;
		//else if (!strcmp(traffic, "butterfly"))
		    //NoximGlobalParams::traffic_distribution =
			//TRAFFIC_BUTTERFLY;
		//else if (!strcmp(traffic, "shuffle"))
		    //NoximGlobalParams::traffic_distribution =
			//TRAFFIC_SHUFFLE;
		//else if (!strcmp(traffic, "table")) {
		    //NoximGlobalParams::traffic_distribution =
			//TRAFFIC_TABLE_BASED;
		    //strcpy(NoximGlobalParams::traffic_table_filename,
			   //arg_vet[++i]);
		//} else
		    //NoximGlobalParams::traffic_distribution =
			//INVALID_TRAFFIC;
	    //} else if (!strcmp(arg_vet[i], "-arbiter")) {
		//char *arbiter = arg_vet[++i];
		//if (!strcmp(arbiter, "oblivious"))
			//NoximGlobalParams::arbiter = ARBITER_OBLIVIOUS;
		//else if (!strcmp(arbiter, "fixed"))
			//NoximGlobalParams::arbiter = ARBITER_FIXED_PRIORITY;
		//else
			//NoximGlobalParams::arbiter = INVALID_ARBITER;
		//} else if (!strcmp(arg_vet[i], "-hs")) {
		//int node = atoi(arg_vet[++i]);
		//double percentage = atof(arg_vet[++i]);
		//pair < int, double >t(node, percentage);
		//NoximGlobalParams::hotspots.push_back(t);
	    //} else if (!strcmp(arg_vet[i], "-warmup"))
		//NoximGlobalParams::stats_warm_up_time = atoi(arg_vet[++i]);
	    //else if (!strcmp(arg_vet[i], "-seed"))
		//NoximGlobalParams::rnd_generator_seed = atoi(arg_vet[++i]);
	    //else if (!strcmp(arg_vet[i], "-detailed"))
		//NoximGlobalParams::detailed = true;
	    //else if (!strcmp(arg_vet[i], "-volume"))
		//NoximGlobalParams::max_volume_to_be_drained =
		    //atoi(arg_vet[++i]);
	    //else if (!strcmp(arg_vet[i], "-sim"))
		//NoximGlobalParams::simulation_time = atoi(arg_vet[++i]);
	    //else {
		//cerr << "Error: Invalid option: " << arg_vet[i] << endl;
		//exit(1);
	    //}
	//}
    //}

    //checkInputParameters();

    //// Show configuration
    //if (NoximGlobalParams::verbose_mode > VERBOSE_OFF)
	//showConfig();
}
