#include "noc.h"

//---------------------------------------------------------------------------


void TNoC::buildCrossbar()
{
  // Create the grid as a matrix of tiles
  for(int i=0; i<TGlobalParams::mesh_dim_x; i++)
    {
      for(int j=0; j<TGlobalParams::mesh_dim_y; j++)
	{
	  // Create the single Tile with a proper name
	  char tile_name[20];
	  sprintf(tile_name, "Tile[%02d][%02d]", i, j);
	  git[i][j] = new TGITile(tile_name, b_mark);
	  
	  git[i][j]->gwi->cluster_id = i * TGlobalParams::mesh_dim_y + j;
	  //cout<< "Cluster: "<< tile_name <<" Component wise details in Cluster: "<< i * TGlobalParams::mesh_dim_y + j<<endl;
	  // Map clock and reset
	  git[i][j]->clock(clock);
	  git[i][j]->photonic_clock(photonic_clock);
	  git[i][j]->reset(reset);

	  //Tell to the PEs and Hubs its coordinates and its hub ids;
	  for(int hub_index = 0; hub_index<HUBS_PER_CLUSTER; hub_index++)
	  {
	  for(int c =0; c<PE_PER_HUB ; c++)
	  {
	  git[i][j]->pe[c][hub_index]->local_id = 8*(i * TGlobalParams::mesh_dim_y + j)+c+PE_PER_HUB*hub_index;
	  git[i][j]->pe[c][hub_index]->hub_id = HUBS_PER_CLUSTER*(i * TGlobalParams::mesh_dim_y + j)+ hub_index;
	  git[i][j]->pe[c][hub_index]->cluster_id = i * TGlobalParams::mesh_dim_y + j;
	  //cout << "pe local id[" <<i<<"]["<<j<<"]["<<hub_index<<"]["<<c<<"]: "<< (i * TGlobalParams::mesh_dim_y + j)*8+c+4*hub_index  << endl;
	  //cout << "pe hub id[" <<i<<"]["<<j<<"]["<<hub_index<<"]["<<c<<"]: "  << 2*(i * TGlobalParams::mesh_dim_y + j)+ hub_index << endl;
	  //cout << "pe cluster id[" <<i<<"]["<<j<<"]["<<hub_index<<"]["<<c<<"]: "  << i * TGlobalParams::mesh_dim_y + j << endl;
      }
      git[i][j]->h[hub_index]->hub_id = HUBS_PER_CLUSTER*(i * TGlobalParams::mesh_dim_y + j)+ hub_index;
      git[i][j]->h[hub_index]->cluster_id = i * TGlobalParams::mesh_dim_y + j;
      //cout << "Concentrator hub id[" <<i<<"]["<<j<<"]["<<hub_index<<"]: "  << 2*(i * TGlobalParams::mesh_dim_y + j)+ hub_index << endl;
	  //cout << "Concentrator cluster id[" <<i<<"]["<<j<<"]["<<hub_index<<"]: "  << i * TGlobalParams::mesh_dim_y + j << endl;
	  }
	  	
	   //Map Tx signals and Rx Signals
	  int cluster_id = i * TGlobalParams::mesh_dim_y + j;
	  //cout<< " cluster_id: "<< cluster_id<<endl;
	   for(int k=0; k<(TGlobalParams::clos_waveguides); k++) 
		{
			if (k == cluster_id)
			{
			git[i][j]->flit_tx_clos[k](cross_bar_additional_output);  // channel assignment for sending data 
			//cout << "Tile " <<i<<j<< " Output connect to dummy output channel: "<< k << endl;
			git[i][j]->flit_rx_clos[k](cross_bar_additional_input); // channel assignment for receveing data 
			//cout << "Tile " <<i<<j<< " Input connect to dummy input channel: "<< k << endl;			
			}
			else
			{
			git[i][j]->flit_tx_clos[k](cross_bar[cluster_id][k]);  // channel assignment for sending data 
			//cout << "Tile " <<i<<j<< " Output connection: "<< k <<" -> "<<" ["<<cluster_id<<"]["<<k<<"]"<< endl;
			git[i][j]->flit_rx_clos[k](cross_bar[k][cluster_id]); // channel assignment for receveing data 
			//cout << "Tile " <<i<<j<< " Input connection: "<< k <<" -> "<<" ["<<k<<"]["<<cluster_id<<"]"<< endl;
			}
		}

	}
    }

}

//---------------------------------------------------------------------------
TGITile* TNoC::searchNode(const int id) const
{
//  for (int i=0; i<TGlobalParams::mesh_dim_x; i++)
//    for (int j=0; j<TGlobalParams::mesh_dim_y; j++)
//      if (git[i][j]->gwi->cluster_id == id)
//	return git[i][j];
//
//  return false;
}
