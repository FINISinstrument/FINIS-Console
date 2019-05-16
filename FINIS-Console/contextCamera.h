#pragma once

#include "windows.h"
#include <string>
#include <direct.h>
#include <thread>
#include <atomic>
#include "opencv2/opencv.hpp"

#include "constants.h"

class ContextCamera {
public:
	ContextCamera();
	ContextCamera(std::string basePath, std::string camera);
	~ContextCamera();

	void setFilePath(std::string path) { filePath = path; }
	void setCameraName(std::string name) { cameraName = name; }

	int snap();
	int recordFrames(int count);
private:
	cv::VideoCapture cam;
	cv::Mat frame;

	std::string filePath;
	std::string cameraName;
	int frameIndex;

	int folderNumber;
};
