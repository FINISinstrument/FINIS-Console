#include "contextCamera.h"

ContextCamera::ContextCamera() {

}
ContextCamera::ContextCamera(int camera_id, std::string basePath, std::string camera)
{
	cam.open(camera_id);

	//cam.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	//cam.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
	cam.set(cv::CAP_PROP_FRAME_WIDTH, 320);
	cam.set(cv::CAP_PROP_FRAME_HEIGHT, 240);
	//cam.set(cv::CAP_PROP_FRAME_WIDTH, 160);
	//cam.set(cv::CAP_PROP_FRAME_HEIGHT, 120);
	cam.set(cv::CAP_PROP_FPS, 30);

	filePath = basePath;
	frameIndex = 0;
	folderNumber = 0;
	divisor = 1;
	divisor_index = 0;

	cameraName = camera;
	//semaphore = CreateSemaphore(NULL, 1, 1, NULL);
}

ContextCamera::~ContextCamera() {
	cam.release();
}

int ContextCamera::snap() {
	// Check if new folder needs to be created
	int temp = 1 + frameIndex / FRAMES_IN_FOLDER;
	if (temp != folderNumber) {
		folderNumber = temp;
		CreateDirectoryA((filePath + "/" + std::to_string(folderNumber)).c_str(), NULL);
	}

	divisor_index++;
	if (divisor_index == divisor) {
		// Snap a picture
		cam >> frame;
		//std::cout << "Camera " << cameraName << " snapped a picture\n";
		imwrite((filePath + "/" + std::to_string(folderNumber) + "/" + cameraName + "_" + ZeroPadString(frameIndex++) + ".tiff").c_str(), frame);
		divisor_index = 0;
	}
	return divisor_index;
}

int ContextCamera::recordFrames(int count) {
	for (int i = 0; i < count; i++) {
		snap();
	}
	return 0;
}