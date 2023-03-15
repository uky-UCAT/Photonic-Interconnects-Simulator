#include<srcdstphotonicdelay.h>

TSrcDstPhotonicDelay::TSrcDstPhotonicDelay(
		std::string srcdstphotonicdelaycsvname) {

	std::string line;

	std::ifstream photonicdelaycsv(srcdstphotonicdelaycsvname.c_str());

	std::getline(photonicdelaycsv, line);
	maxphotonicdelay =0;

	while (std::getline(photonicdelaycsv, line)) {

		vector < string > values;
		std::string value;
		std::stringstream ss(line);
		std::string srcdstcomb;
		int delay = 0;

		while (getline(ss, value, ',')) {

			values.push_back(value);
		}
		srcdstcomb = values.at(0);
		std::stringstream convert(values.back());
		convert >> delay;
		if(delay>maxphotonicdelay){

			maxphotonicdelay = delay;
		}
		photonicdelaymap.insert(pair<string, int>(srcdstcomb, delay));

	}

}
int TSrcDstPhotonicDelay::getPhotonicDelay(std::string src, std::string dst) {

	std::string srcdstkey = "(" + src + "  " + dst + ")";
	int value;
	std::map<std::string, int>::iterator pos;
	pos = photonicdelaymap.find(srcdstkey);
	if (pos == photonicdelaymap.end()) {
		cerr << "Src Destination pair is not possible " << endl;
	} else {
		//cerr<<"Key Found"<<endl;
		value = pos->second;

	}
	return value;
}
int TSrcDstPhotonicDelay::getMaxPhotonicDelay(){

	return maxphotonicdelay;
}


