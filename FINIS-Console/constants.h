#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <iomanip>
#include <sstream>
#include <string>
#include <algorithm>
// These headers are used to convert std::string to std::wstring
#include <locale>
#include <codecvt>
#include "createDirectoryRecursively.h"

#define FRAMES_IN_FOLDER 800 // How many frames to store in a folders

std::string ZeroPadString(int num);
std::string ZeroPadString(int num, int length);

std::string getDateTime();

std::string createBaseSavePath(std::string folderPath);

#endif