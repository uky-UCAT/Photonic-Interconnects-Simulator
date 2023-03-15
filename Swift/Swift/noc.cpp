#include "noc.h"

//---------------------------------------------------------------------------


void TNoC::buildCrossbar()
{
	// Arbiter pin assignments
    garbiter   = new TGlobalarbiter("Globalarbiter");
    // Map clock and reset
	  garbiter->clock(clock);
	  garbiter->photonic_clock(photonic_clock);
	  garbiter->reset(reset);
  // Create the grid as a matrix of tiles
  for(int i=0; i<TGlobalParams::mesh_dim_x; i++)
    {
      for(int j=0; j<TGlobalParams::mesh_dim_y; j++)
	{
	  // Create the single Tile with a proper name
	  char tile_name[20];
	  sprintf(tile_name, "Tile[%02d][%02d]", i, j);
	  git[i][j] = new TGITile(tile_name, b_mark);

	  // Tell to the PE its coordinates
	  git[i][j]->pe->local_id = i * TGlobalParams::mesh_dim_x + j;
	  //cout << "pe local id : " <<i<<j<< git[i][j]->pe->local_id << endl;

      git[i][j]->gwi->local_id_new = i * TGlobalParams::mesh_dim_x + j;
	  // Map clock and reset
	  git[i][j]->clock(clock);
	  git[i][j]->photonic_clock(photonic_clock);
	  git[i][j]->reset(reset);
	  
	  // Map Arbiter signals
	  git[i][j]->req_tx_arbiter_flit(req_tx_arbiter_flit_signal[i][j]);
	  git[i][j]->req_tx_arbiter_hf(req_tx_arbiter_hf_signal[i][j]);
	  git[i][j]->ack_tx_arbiter_hf(ack_tx_arbiter_hf_signal[i][j]);
	  
	  git[i][j]->req_rx_arbiter_ch(req_rx_arbiter_ch_signal[i][j]);
	  git[i][j]->mwmr_channel_no(mwmr_channel_no_signal[i][j]);
	  git[i][j]->ack_rx_arbiter_ch(ack_rx_arbiter_ch_signal[i][j]);
	  
	  git[i][j]->req_txc_arbiter(req_txc_arbiter_signal[i][j]);
	  git[i][j]->ack_txc_arbiter(ack_txc_arbiter_signal[i][j]);
	  git[i][j]->ack_txc_gwi(ack_txc_gwi_signal[i][j]);
	  	
	  // Map Tx signals and Rx Signals
	   for(int k=0; k<(TGlobalParams::mwmr_waveguides); k++) 
		{
			for(int l=0; l<4; l++)
			{  
			git[i][j]->flit_tx[k][l](cross_bar[k][l]);  // channel assignment for sending data 
			//cout << "Tile" <<i<<j<< "Output connect to channel"<< k << endl;
			git[i][j]->flit_rx[k][l](cross_bar[k][l]); // channel assignment for receveing data 
			//cout << "Tile" <<i<<j<< "Input connect to channel"<< k << endl;
		   }
		}

	  // connect arbiter with tiles
	  
	  garbiter->req_tx_arbiter_flit[i][j](req_tx_arbiter_flit_signal[i][j]);
	   garbiter->req_tx_arbiter_hf[i][j](req_tx_arbiter_hf_signal[i][j]);
	   garbiter->ack_tx_arbiter_hf[i][j](ack_tx_arbiter_hf_signal[i][j]);
	   
	   garbiter->req_rx_arbiter_ch[i][j](req_rx_arbiter_ch_signal[i][j]);
	   garbiter->mwmr_channel_no[i][j](mwmr_channel_no_signal[i][j]);
	   garbiter->ack_rx_arbiter_ch[i][j](ack_rx_arbiter_ch_signal[i][j]);
	   
	   garbiter->req_txc_arbiter[i][j](req_txc_arbiter_signal[i][j]);
	   garbiter->ack_txc_arbiter[i][j](ack_txc_arbiter_signal[i][j]);
	   garbiter->ack_txc_gwi[i][j](ack_txc_gwi_signal[i][j]);

	}
    }

}

TGITile* TNoC::searchNode(const int id) const
{
  for (int i=0; i<TGlobalParams::mesh_dim_x; i++)
    for (int j=0; j<TGlobalParams::mesh_dim_y; j++)
      if (git[i][j]->gwi->local_id == id)
	return git[i][j];

}

//---------------------------------------------------------------------------

