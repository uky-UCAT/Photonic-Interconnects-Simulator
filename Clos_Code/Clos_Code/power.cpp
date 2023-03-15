#include <cassert>
#include "GlobalTypeDefs.h"
#include "power.h"


using namespace std;
double TPower::pwr_buffering   =  9.38275e-12;
double TPower::pwr_routing     = 0.0;
double TPower::pwr_selection   = 0.0;
double TPower::pwr_arbitration = 3.71035e-13;
double TPower::pwr_crossbar    = 1.0258e-11;
double TPower::pwr_link        = 11.0e-12;
double TPower::pwr_link_lv     = 11.0e-12;
double TPower::pwr_leakage     = 0.0001e-9/2.0;
double TPower::pwr_end2end     = 0.0;

bool   TPower::power_data_loaded = false;

TPower::TPower()
{
  pwr = 0.0;

}

void TPower::Routing()
{
  //cout << " Routing() " << endl;
  pwr += pwr_routing;
}

void TPower::Selection()
{
  //cout << " Selection() " << endl;
  pwr += pwr_selection;
}

void TPower::Buffering()
{
  //cout << " Buffering() " << endl;
  pwr += pwr_buffering;
}

void TPower::Link(bool low_voltage)
{
  //cout << " Link() " << endl;
  pwr += low_voltage ? pwr_link_lv : pwr_link;
}

void TPower::Arbitration()
{
  //cout << " Arbitration() " << endl;
  pwr += pwr_arbitration;
}

void TPower::Crossbar()
{
  //cout << " Crossbar() " << endl;
  pwr += pwr_crossbar;
}

void TPower::Leakage()
{
  //cout << " Leakage() " << endl;
  pwr += pwr_leakage;
}

void TPower::EndToEnd()
{
  //cout << " EndToEnd() " << endl;
  pwr += pwr_end2end;
}






