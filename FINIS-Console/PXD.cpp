#include <iostream>
#include <windows.h>
#include <time.h>
#include <ctime>
#include <algorithm>
#include <thread>
#include <chrono>
#include "PXD.h"

PXD::PXD(std::string saveLocation) : PXD(saveLocation, true)
{

}

PXD::PXD(std::string saveLocation, bool isThirtyFPS) {
	// Initialize the frame grabber states
	this->isThirtyFPS = isThirtyFPS;
	this->isSixteenBit = true;
	this->isStreaming = false;
	this->isOpen = false;

	this->folderPath = saveLocation;

	this->halfBufferSize = 200;

	// Initialize buffer for recording system time of frames
	frameTimestamps = new uint32_t[this->halfBufferSize * 2];

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

void PXD::recordFrames(int frameCount, int videoPeriod) {
	int i = 0;
	uint32_t time;
	bool first = true;

	pxd_goLiveSeq(1, 0, 400, 1, 0, videoPeriod);
	//while (pxd_goneLive(1, 0)) { Sleep(0); }
	while (pxd_goneLive(1, 0)) {
		// Put timestamp into buffer
		time = pxd_buffersSysTicks(1, i);
		if (time > frameTimestamps[i]) {
			frameTimestamps[i++] = time;
		}
		if (i % halfBufferSize == 0) {
			if (first) {
				first = false;
			}
			else {
				ReleaseSemaphore(ghSemaphore, 1, NULL);
			}
		}
	}
	
	return true;
}
void PXD::saveFrames(int frameCount, int videoPeriod) {
	int remainingFrames = frameCount;

	// Loop until we have hit frameCount
	for (int i = 0; i < frameCount; i++) {
		// Wait for semaphore
		WaitForSingleObject(ghSemaphore, INFINITE);
		// Loop through first part of buffer
		for (int j = 0; j < halfBufferSize && j < remainingFrames; j++) {
			pxd_saveTiff(1, (folderPath + "/" + std::to_string(i * 400 + j) + ".tiff").c_str(), j, 0, 0, -1, -1, 0, 0);
		}
		remainingFrames -= halfBufferSize;
		
		// Check if not finished
		if (remainingFrames > 0) {
			// Wait for semaphore
			WaitForSingleObject(ghSemaphore, INFINITE);
			// Loop through second part of buffer
			for (int j = 0; j < halfBufferSize && j < remainingFrames; j++) {
				pxd_saveTiff(1, (folderPath + "/" + std::to_string(i * 400 + halfBufferSize + j) + ".tiff").c_str(), j, 0, 0, -1, -1, 0, 0);
			}
			remainingFrames -= halfBufferSize;
		}
	}
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
	
	// Create semaphore object for syncronizing saving
	ghSemaphore = CreateSemaphore(NULL, 1, 1, NULL);

	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

	// Spawn thread to record frames
	std::thread recordThread(recordFrames, frameCount, 1);
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

	// Save images, wait for record to finish
	std::thread saveThread(saveFrames, frameCount, 1);
	recordThread.join();

	// Getting time to record frames
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	std::cout << "Record took " << duration << " microseconds to capture " << frameCount << " frames\n";

	// Wait for save to finish
	saveThread.join();

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