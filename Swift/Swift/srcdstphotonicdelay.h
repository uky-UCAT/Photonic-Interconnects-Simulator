#include<map>
#include<iostream>
#include<iterator>
#include <string>
#include <fstream>
#include <vector>
#include <utility> // std::pair
#include <stdexcept> // std::runtime_error
#include <sstream> // std::stringstream

using namespace std;

class TSrcDstPhotonicDelay
{

 public:

	TSrcDstPhotonicDelay(std::string srcdstphotonicdelaycsvname);
	int getPhotonicDelay(std::string src,std::string dst);
	int getMaxPhotonicDelay();

 private:


  std::string srcdstphotonicdelaycsvname;
  std::string src;
  std::string dst;
  std::map<std::string,int> photonicdelaymap;
  int photonicdelay;
  int maxphotonicdelay;


};
