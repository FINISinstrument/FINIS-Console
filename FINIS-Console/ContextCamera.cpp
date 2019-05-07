#include "ContextCamera.h"

ContextCamera::ContextCamera(std::string basePath)
{
	cam(0);

	cam.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	cam.set(cv::CAP_PROP_FRAME_HEIGHT, 470);
	cam.set(cv::CAP_PROP_FPS, 30);

	filePath = basePath;
	frameIndex = 0;
}

ContextCamera::~ContextCamera() {
	cam.release();
}

int ContextCamera::snap() {
	cam >> frame;
	imwrite( (filePath + std::to_string(frameIndex++) + ".tiff").c_str() , frame);
	return 0;
}

int ContextCamera::recordFrames(int count) {
	for (int i = 0; i < count; i++) {
		snap();
	}
	return 0;
}