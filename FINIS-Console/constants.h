#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <iomanip>
#include <sstream>
#include <string>
#include <algorithm>
#include <fstream>
// These headers are used to convert std::string to std::wstring
#include <locale>
#include <codecvt>
#include "createDirectoryRecursively.h"

#define FRAMES_IN_FOLDER 800 // How many frames to store in a folders

const std::string vimba_config_path = "C:/FINIS/configuration/vimba_config.txt";

std::string ZeroPadString(int num);
std::string ZeroPadString(int num, int length);

std::string getDateTime();

std::string createBaseSavePath();
std::string createBaseSavePath(std::string folderPath);

std::wstring s2w(std::string s);

bool copyFile(const char *SRC, const char* DEST);

#endif
