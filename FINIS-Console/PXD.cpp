#include <iostream>
#include <windows.h>
#include <time.h>
#include <ctime>
#include <algorithm>
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
}

int PXD::snap(std::string imageName) {
	// Record single image
	enable();

	pxd_goSnap(1, 1);
	// Change sleep method?
	Sleep(50);
	int saveTiffError = pxd_saveTiff(1, (folderPath + "/" + imageName + ".tiff").c_str(), 1, 0,0, -1,-1, 0,0);
	if (saveTiffError < 0) {
		printf("Error saving image: %s\n", pxd_mesgErrorCode(saveTiffError));
	}
	return 1;
}

bool PXD::recordFrames(int frameCount) {
	// Take images
	pxd_goLiveSeq(1, 1, 401, 1, frameCount, 1);
	while (pxd_goneLive(1, 0)) { Sleep(0); }
	return true;
}
bool PXD::saveFrames(int frameCount) {
	for (int i = 0; i < frameCount; i++) {
		pxd_saveTiff(1, (folderPath + "/" + std::to_string(i) + ".tiff").c_str(), i, 0, 0, -1, -1, 0, 0);
	}
	return true;
}

int PXD::video(int frameCount) {
	// Disable if live from a previous occurance or snap
	if (isStreaming) {
		pxd_goUnLive(1);
		isStreaming = false;
	}

	// Get directory to save images to
	getDateTime();
	folderPath = baseVideoPath + dateTime;
	// Create directory
	CreateDirectoryA(folderPath.c_str(), NULL);
	std::cout << GetLastError() << "\n";
	
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	// Record frames
	recordFrames(frameCount % 400);
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	std::cout << "Record took " << duration << " microseconds to capture " << frameCount << " frames\n";

	// Save images
	saveFrames(frameCount % 400);


	return 0;
	/*
	// TODO: Finish reimplementing more than 400 frames. For testing purposes, max capture of 400
	// Start recording
	int offset;
	for (int i = 0; i < frameCount / halfBufferSize; i++) {
		// Record in first half
		if (i % 2 == 0) {
			pxd_goLiveSeq(1, 1, 1 + (i % 2)*halfBufferSize, 1, halfBufferSize, 1);
		}
		else {
			pxd_goLiveSeq(1, 1, 1 + (i % 2)*halfBufferSize, 1, halfBufferSize, 1);
		}
	}
	*/
}

int PXD::openPXD() {
	int error;
	if (isThirtyFPS && isSixteenBit) {
		error = pxd_PIXCIopen("", "", "C:/Users/FINIS/source/repos/FINIS-Console/FINIS-Console/Resources/XCAPVideoSetup16Bit30Hz.fmt");
	}
	else if(isThirtyFPS && !isSixteenBit) {
		error = pxd_PIXCIopen("", "", "C:/Users/FINIS/source/repos/FINIS-Console/FINIS-Console/Resources/XCAPVideoSetup14Bit30Hz.fmt");
	}
	else if(!isThirtyFPS && isSixteenBit) {
		error = pxd_PIXCIopen("", "", "C:/Users/FINIS/source/repos/FINIS-Console/FINIS-Console/Resources/XCAPVideoSetup16Bit15Hz.fmt");
	}
	else {
		error = pxd_PIXCIopen("", "", "C:/Users/FINIS/source/repos/FINIS-Console/FINIS-Console/Resources/XCAPVideoSetup14Bit15Hz.fmt");
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