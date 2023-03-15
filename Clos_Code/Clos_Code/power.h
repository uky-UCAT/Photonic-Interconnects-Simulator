#ifndef __POWER_H__
#define __POWER_H__

class TPower
{

 public:

  TPower();

  void Buffering();
    void Routing();
    void Selection();
    void Arbitration();
    void Crossbar();
    void Link(bool low_voltage);
    void EndToEnd();
    void Leakage();

  double getPower() {
	return pwr;
    } 

    double getPwrRouting() {
	return pwr_routing;
    }
    
    double getPwrSelection() {
	return pwr_selection;
    }
    
    double getPwrBuffering() {
	return pwr_buffering;
    }
    
    double getArbitration() {
	return pwr_arbitration;
    }

    double getCrossbar() {
	return pwr_crossbar;
    }

    double getLeakage() {
	return pwr_leakage;
    }

    double getPwrLink(bool low_voltage) {
      if (low_voltage)
	return pwr_link_lv;
      else
	return pwr_link;
    }

    double getPwrEndToEnd() {
      return pwr_end2end;
    }

  private:
    
    static double pwr_buffering;
    static double pwr_routing;
    static double pwr_selection;
    static double pwr_arbitration;
    static double pwr_crossbar;
    static double pwr_link;
    static double pwr_link_lv;
    static double pwr_leakage;
    static double pwr_end2end;

    static bool   power_data_loaded;

    double pwr;
};

// ---------------------------------------------------------------------------

#endif
