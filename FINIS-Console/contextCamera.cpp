#include "contextCamera.h"

ContextCamera::ContextCamera() {

}
ContextCamera::ContextCamera(int camera_id, std::string basePath, std::string camera, bool useSmallResolution)
{
	cam.open(camera_id);

	//cam.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	//cam.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
	//cam.set(cv::CAP_PROP_FRAME_WIDTH, 480);
	//cam.set(cv::CAP_PROP_FRAME_HEIGHT, 360);

    if (useSmallResolution) {
        cam.set(cv::CAP_PROP_FRAME_WIDTH, 320);
        cam.set(cv::CAP_PROP_FRAME_HEIGHT, 240);
    } else {
        cam.set(cv::CAP_PROP_FRAME_WIDTH, 480);
        cam.set(cv::CAP_PROP_FRAME_HEIGHT, 360);
    }
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
        CreateDirectoryA((filePath + "/" + cameraName).c_str(), NULL);
		CreateDirectoryA((filePath + "/" + cameraName + "/" + cameraName + "_" + ZeroPadString(folderNumber,3)).c_str(), NULL);
	}

	divisor_index++;
	if (divisor_index == divisor) {
		// Snap a picture
		cam >> frame;
		//std::cout << "Camera " << cameraName << " snapped a picture\n";
		imwrite((filePath + "/" + cameraName + "/" + cameraName + "_" + ZeroPadString(folderNumber,3) + "/" + cameraName + "_" + ZeroPadString(frameIndex++) + ".tiff").c_str(), frame, tags);
		divisor_index = 0;
	}
	return divisor_index;
}

void ContextCamera::record(HANDLE &signal_begin, std::atomic<bool> &complete) {
	// Create file to record timestamps
	std::ofstream f_timestamps = std::ofstream((filePath + "/" + cameraName + "_timestamps.txt").c_str());

	int div = 0;

	// Setup timestamp variables
	auto now = std::chrono::high_resolution_clock::now();
	auto last = now;
	auto duration = now.time_since_epoch();
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

	// Wait for the calling function to signal the semaphore to begin recording, as other cameras are being initialized
	WaitForSingleObject(signal_begin, INFINITE);

	while (!complete) {
		div = snap();
		// Save frame timestamp
		if (div == 0) {
			last = now;
			now = std::chrono::high_resolution_clock::now();
			duration = now.time_since_epoch();
			millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
			f_timestamps << millis << "\t";
			duration = now - last;
			millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
			f_timestamps << millis << "\n";
		}
	}

	// Close file
	f_timestamps.close();
}

int ContextCamera::recordFrames(int count) {
	for (int i = 0; i < count; i++) {
		snap();
	}
	return 0;
}
