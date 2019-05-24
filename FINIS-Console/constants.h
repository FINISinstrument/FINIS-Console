#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <iomanip>
#include <sstream>
#include <string>

#define FRAMES_IN_FOLDER 800 // How many frames to store in a folders

static std::string ZeroPadString(int num) {
	std::ostringstream ss;
	ss << std::setw(5) << std::setfill('0') << num;
	return ss.str();
}

#endif