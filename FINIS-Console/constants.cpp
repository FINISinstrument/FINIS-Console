#include "constants.h"

std::string ZeroPadString(int num) {
	std::ostringstream ss;
	ss << std::setw(5) << std::setfill('0') << num;
	return ss.str();
}

std::string ZeroPadString(int num, int length) {
	std::ostringstream ss;
	ss << std::setw(length) << std::setfill('0') << num;
	return ss.str();
}

std::string getDateTime() {
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

std::string createBaseSavePath() {
	return createBaseSavePath("C:/FINIS/testing/Video");
}
std::string createBaseSavePath(std::string folderPath) {
	// Get directory to save images to
	std::string dateTime = getDateTime();
	std::string path = folderPath + "/" + dateTime;

	// Conversion to wstring for directory creation
	/*
	std::wstring w_path;
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::string narrow = converter.to_bytes(w_path);
	std::wstring wide = converter.from_bytes(path);
	*/

	createDirectoryRecursively(s2w(path));
	//folderPath = baseVideoPath + dateTime;
	// Create directory
	//CreateDirectoryA(folderPath.c_str(), NULL);
	//std::cout << GetLastError() << "\n";

	return path;
}

std::wstring s2w(std::string s) {
	std::wstring w;
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::string narrow = converter.to_bytes(w);
	std::wstring wide = converter.from_bytes(s);

	return wide;
}

bool copyFile(const char *SRC, const char* DEST)
{
	std::ifstream src(SRC);
	std::ofstream dest(DEST);
	dest << src.rdbuf();
	return src && dest;
}