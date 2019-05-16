#include <iostream>
#include <windows.h>
#include <time.h>
#include <ctime>
#include <algorithm>
#include <thread>
#include <chrono>
#include "PXD.h"

// Static member variables defined
int PXD::halfBufferSize = 200;
std::string PXD::folderPath = "C:/Users/FINIS/Desktop/";
uint32_t* PXD::frameTimestamps = new uint32_t[PXD::halfBufferSize * 2];
std::atomic<bool> PXD::finishedWithVideo = false;
bool PXD::firstHandleRun = true;
HANDLE PXD::ghSemaphore = CreateSemaphore(NULL, 1, 1, NULL);
std::vector<ContextCamera> PXD::contextCameras = std::vector<ContextCamera>();

PXD::PXD(std::string saveLocation) : PXD(saveLocation, true)
{

}

PXD::PXD(std::string saveLocation, bool isThirtyFPS) {
	// Initialize the frame grabber states
	this->isThirtyFPS = isThirtyFPS;
	this->isSixteenBit = true;
	this->isStreaming = false;
	this->isOpen = false;

	int openError = openPXD();
	if (openError < 0) {
		printf("error: %s\n", pxd_mesgErrorCode(openError));
	}
	isOpen = (openError >= 0);
}

PXD::~PXD() {
	if (isOpen) {
		pxd_PIXCIclose();
	}

	if (frameTimestamps != nullptr) {
		delete frameTimestamps;
		frameTimestamps = nullptr;
	}
}

int PXD::snap(std::string imageName) {
	// Record single image
	enable();

	pxd_goSnap(1, 1);
	// Change sleep method?
	Sleep(50);
	int saveTiffError = pxd_saveTiff(1, (baseImagePath + imageName + ".tiff").c_str(), 1, 0,0, -1,-1, 0,0);
	if (saveTiffError < 0) {
		printf("Error saving image: %s\n", pxd_mesgErrorCode(saveTiffError));
	}
	return 1;
}

void PXD::recordFrames(int videoPeriod) {
	int i = 0;
	uint32_t time;
	bool first = true;

	pxd_goLiveSeq(1, 0, 399, 1, 0, videoPeriod);
	//while (pxd_goneLive(1, 0)) { Sleep(0); }
	while (!finishedWithVideo || i % 200 != 0) { //TODO end loop logic needs to be reworked
		// Put timestamp into buffer
		time = pxd_buffersSysTicks(1, i);
		std::cout << "Record is looping! I: " << i << "\tTime: " << time << "\tframeTimeStamps: " << frameTimestamps[i%400] << "\n";
		if (time != frameTimestamps[i%400]) {
			frameTimestamps[(i++)%400] = time;
			// Every time there is a new frame, context frames need to be saved
			for (int i = 0; i < contextCameras.size(); i++) {
				//contextCameras[i].snap();
			}
			std::cout << "Frame index: " << i << "\n";
		}
		// Check if we should start the next save cycle
		if (i % halfBufferSize == 0) {
			if (first) {
				first = false;
			}
			else {
				ReleaseSemaphore(ghSemaphore, 1, NULL);
			}
		}
	}
	std::cout << "record frames, relesaing last semaphore\n";
	ReleaseSemaphore(ghSemaphore, 1, NULL);
	std::cout << "record frames, going unlive\n";
	pxd_goUnLive(1);
	std::cout << "record frames, finished\n";
}

void PXD::saveFrames(int count, int videoPeriod, bool secondsCount) {
	int frameCount = 0;
	int firstHalf = 1;

	int folderNumber = 1;

	// Get start time
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

	// Loop until we have hit frameCount
	while ((frameCount < count && !secondsCount) || (duration < count && secondsCount)) {
		firstHalf = 1 - firstHalf;
		// Wait for semaphore
		std::cout << "Waiting for semaphore\n";
		WaitForSingleObject(ghSemaphore, INFINITE);
		std::cout << "got semaphore\n";

		// Set subfolder value
		folderNumber = 1 + frameCount / FRAMES_IN_FOLDER;
		// Attemp to create subfolder
		CreateDirectoryA((folderPath + "/" + std::to_string(folderNumber)).c_str(), NULL);

		// Save frames
		for (int i = 0; i < halfBufferSize; i++) {
			pxd_saveTiff(1, (folderPath + "/" + std::to_string(folderNumber) + "/IR_" + std::to_string(frameCount + i) + ".tiff").c_str(), (firstHalf * 200) + i, 0, 0, -1, -1, 0, 0);
		}
		// Update reference metrics
		t2 = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
		frameCount += halfBufferSize;
	}
	// Should be done recording
	std::cout << "Finishing with video, save thread\n";
	finishedWithVideo = true;
	std::cout << "frameCount: " << frameCount << "\n";
}

int PXD::video(int frameCount) {
	// Disable if live from a previous occurance or snap
	if (isStreaming) {
		pxd_goUnLive(1);
		isStreaming = false;
	}

	// Reset finished flag
	finishedWithVideo = false;

	// Clear buffer of timestamps
	for (int i = 0; i < this->halfBufferSize * 2; i++) {
		frameTimestamps[i] = 0;
	}

	// Get directory to save images to
	getDateTime();
	folderPath = baseVideoPath + dateTime;
	// Create directory
	CreateDirectoryA(folderPath.c_str(), NULL);
	std::cout << GetLastError() << "\n";

	// Update context cameras working directory
	for (int i = 0; i < contextCameras.size(); i++) {
		contextCameras[i].setFilePath(folderPath);
	}
	
	// Create semaphore object for syncronizing saving
	if (firstHandleRun) {
		CloseHandle(ghSemaphore);
		firstHandleRun = false;
	}
	ghSemaphore = CreateSemaphore(NULL, 1, 1, NULL);
	std::cout << "intital decrement\n";
	WaitForSingleObject(ghSemaphore, INFINITE);

	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

	// Spawn thread to record frames
	std::thread recordThread(recordFrames, 1);
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

	// Save images, wait for record to finish
	//std::thread saveThread(saveFrames, frameCount, 1);
	//std::thread saveThread(saveFrames, 10, 1, true); // 10 seconds to save
	std::thread saveThread(saveFrames, frameCount, 1, false); // frames to save
	std::cout << " waiting for record thread\n";
	recordThread.join();
	std::cout << " record thread joined\n";

	// Getting time to record frames
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	std::cout << "Record took " << duration << " microseconds to capture " << frameCount << " frames\n";

	// Wait for save to finish
	std::cout << " waiting for save thread\n";
	saveThread.join();
	std::cout << " save thread joined\n";

	/*
	std::cout << "Outputing times from buffer:\n";
	for (int i = 0; i < halfBufferSize * 2; i++) {
		std::cout << "Frame " << i << ": " << frameTimestamps[i] << "\n";
	}
	*/

	// Close semaphore
	CloseHandle(ghSemaphore);

	return 0;
}

void PXD::addContextCamera(ContextCamera &camera) {
	contextCameras.push_back(camera);
}

std::string PXD::getFolderPath() {
	return folderPath;
}

int PXD::openPXD() {
	int error;
	if (isThirtyFPS && isSixteenBit) {
		error = pxd_PIXCIopen("", "", "C:/Users/FINIS/source/repos/FINIS-Console/FINIS-Console/Resources/XCAPVideoSetup16Bit30Hz.fmt");
		std::cout << "30 FPS, 16 Bit\n";
	}
	else if(isThirtyFPS && !isSixteenBit) {
		error = pxd_PIXCIopen("", "", "C:/Users/FINIS/source/repos/FINIS-Console/FINIS-Console/Resources/XCAPVideoSetup14Bit30Hz.fmt");
		std::cout << "30 FPS, 14 Bit\n";
	}
	else if(!isThirtyFPS && isSixteenBit) {
		error = pxd_PIXCIopen("", "", "C:/Users/FINIS/source/repos/FINIS-Console/FINIS-Console/Resources/XCAPVideoSetup16Bit15Hz.fmt");
		std::cout << "15 FPS, 16 Bit\n";
	}
	else {
		error = pxd_PIXCIopen("", "", "C:/Users/FINIS/source/repos/FINIS-Console/FINIS-Console/Resources/XCAPVideoSetup14Bit15Hz.fmt");
		std::cout << "15 FPS, 14 Bit\n";
	}
	return error;
}

void PXD::getDateTime() {
	std::time_t t = std::time(0);
	
	// Convert to string
	// Ignoring CRT_SECURE_NO_WARNINGS
	dateTime = ctime(&t);

	// Sanitize string
	for (std::string::iterator it = dateTime.begin(); it != dateTime.end(); ++it) {
		if (*it == ' ')
			*it = '_';
		if (*it == ':')
			*it = '-';
	}
	dateTime.erase(std::remove(dateTime.begin(), dateTime.end(), '\n'), dateTime.end());
}

int PXD::enable() {
	// Check if opened
	if (!isOpen) {
		int openError = openPXD();
		if (openError < 0) {
			printf("Error on open: %s\n", pxd_mesgErrorCode(openError));
			return -1;
		}
	}

	if (!isStreaming) {
		pxd_goLive(1, 1);
		// Busy wait, needs to be changed to use futures
		while (!pxd_goneLive(1, 0)) { Sleep(0); }
		isStreaming = true;
	}
	return 0;
}

int PXD::disable() {
	// Check if already opened
	if (isOpen && isStreaming) {
		pxd_goUnLive(1);
	}
	isStreaming = false;
	return 0;
}