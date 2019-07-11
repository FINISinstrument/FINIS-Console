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

static std::string getDateTime() {
	std::time_t t = std::time(0);

	// Convert to string
	// Ignoring CRT_SECURE_NO_WARNINGS
	std::string dateTime = ctime(&t);

	// Sanitize string
	for (std::string::iterator it = dateTime.begin(); it != dateTime.end(); ++it) {
		if (*it == ' ')
			*it = '_';
		if (*it == ':')
			*it = '-';
	}
	dateTime.erase(std::remove(dateTime.begin(), dateTime.end(), '\n'), dateTime.end());

	return dateTime;
}

#endif