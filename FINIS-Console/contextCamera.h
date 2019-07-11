#pragma once

#include "windows.h"
#include <string>
#include <direct.h>
#include <thread>
#include <atomic>
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"

#include "constants.h"

class ContextCamera {
	public:
		ContextCamera();
		ContextCamera(int camera_id, std::string basePath, std::string camera);
		~ContextCamera();

		void setFilePath(std::string path) { filePath = path; frameIndex = 0; folderNumber = 0; }
		void setCameraName(std::string name) { cameraName = name; }
		void setDivisor(int div) { divisor = div;  }

		int snap();
		int recordFrames(int count);
	private:
		cv::VideoCapture cam;
		cv::Mat frame;

		std::string filePath;
		std::string cameraName;
		int frameIndex;

		int folderNumber;

		int divisor;
		int divisor_index;

		//		{ TIFFTAG_COMPRESSION, COMPRESSION_NONE };
		std::vector<int> tags = { 259, 1 };
};
