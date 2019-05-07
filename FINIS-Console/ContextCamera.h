#pragma once

#include <string>
#include <direct.h>
#include "opencv2/opencv.hpp"

class ContextCamera {
	public:
		ContextCamera(std::string basePath);
		~ContextCamera();

		int snap();
		int recordFrames(int count);
	private:
		cv::VideoCapture cam;
		cv::Mat frame;

		std::string filePath;
		int frameIndex;
};