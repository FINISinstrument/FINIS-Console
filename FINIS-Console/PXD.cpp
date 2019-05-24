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
std::atomic<bool> PXD::finishedWithContext = false;
bool PXD::firstHandleRun = true;
HANDLE PXD::ghSemaphore = CreateSemaphore(NULL, 1, 1, NULL);
HANDLE PXD::context1_semaphore = CreateSemaphore(NULL, 1, 1, NULL);
HANDLE PXD::context2_semaphore = CreateSemaphore(NULL, 1, 1, NULL);
std::vector<ContextCamera> PXD::contextCameras = std::vector<ContextCamera>();
ContextCamera PXD::contextCamera_1 = ContextCamera();
ContextCamera PXD::contextCamera_2 = ContextCamera();
std::ofstream* PXD::f_irTimestamps = NULL;

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

bool PXD::parseCommand(char* command) {
	// Take a command from a script file, and do the action
	// Valid commands are
	// collect <int> frames
	// collect <int> seconds
	// capture <image name>

	char * word;
	word = strtok(command, " ");

	// Check if first word is capture
	if (word == "capture") {
		word = strtok(NULL, " ");
		snap(word);
		return true;
	}

	// Check for collection
	if (word != "collect") {
		return false;
	}

	// Get count to pass to record function
	int count = 0;
	word = strtok(NULL, " ");
	count = std::stoi(word);

	// Check whether to record seconds
	word = strtok(NULL, " ");
	bool useSeconds = strcmp(word, "seconds");
	
	video(count, useSeconds);
	
	return true;
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
	uint32_t last_time;
	bool first = true;

	pxd_goLiveSeq(1, 1, 400, 1, 0, videoPeriod);
	//while (pxd_goneLive(1, 0)) { Sleep(0); }

	last_time = 0;
	while (!finishedWithVideo) {
		int workingIndex = i % 400;
		// Put timestamp into buffer
		time = pxd_capturedSysTicks(1);

		//std::cout << "I: " << i << "\tTime: " << time << "\n";
		if (time != last_time) {
			frameTimestamps[workingIndex] = time;
			last_time = time;
			i++;

			ReleaseSemaphore(context1_semaphore, 1, NULL);
			ReleaseSemaphore(context2_semaphore, 1, NULL);

			// Check for saving from buffer
			if (i % 200 == 0) {
				ReleaseSemaphore(ghSemaphore, 1, NULL);
			}
		}
	}
	finishedWithContext = true;
	pxd_goUnLive(1);
	/*for (int i = 0; i < 400; i++) {
		std::cout << "  I: " << i << "\tTime: " << frameTimestamps[i] << "\n";
	}*/
}

void PXD::contextOneSnapper() {
	while (!finishedWithContext) {
		//std::cout << "Watigin for context 1\n";
		//WaitForSingleObject(context1_semaphore, INFINITE);
		contextCamera_1.snap();
	}
}
void PXD::contextTwoSnapper() {
	while (!finishedWithContext) {
		//WaitForSingleObject(context2_semaphore, INFINITE);
		contextCamera_2.snap();
	}
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
		WaitForSingleObject(ghSemaphore, INFINITE); // Changing to be noninfinite
		std::cout << "got semaphore\n";

		// Set subfolder value
		folderNumber = 1 + frameCount / FRAMES_IN_FOLDER;
		// Attemp to create subfolder
		CreateDirectoryA((folderPath + "/" + std::to_string(folderNumber)).c_str(), NULL);

		// Save data
		for (int i = 0; i < halfBufferSize; i++) {
			// Save frame
			pxd_saveTiff(1, (folderPath + "/" + std::to_string(folderNumber) + "/IR_" + ZeroPadString(frameCount + i + 1) + ".tiff").c_str(), (firstHalf * 200) + i + 1, 0, 0, -1, -1, 0, 0);
			// Save frame timestamp
			*f_irTimestamps << frameCount + i << "\t" << frameTimestamps[(firstHalf * 200) + i] << "\n";
		}
		// Update reference metrics
		t2 = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
		frameCount += halfBufferSize;
	}
	finishedWithVideo = true;
	// Should be done recording
	std::cout << "frameCount: " << frameCount << "\n";
}

int PXD::video(int frameCount, bool useSeconds) {
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
	contextCamera_1.setFilePath(folderPath);
	contextCamera_2.setFilePath(folderPath);
	
	// Create semaphore object for syncronizing saving
	if (firstHandleRun) {
		CloseHandle(ghSemaphore);
		CloseHandle(context1_semaphore);
		CloseHandle(context2_semaphore);
		firstHandleRun = false;
	}
	ghSemaphore = CreateSemaphore(NULL, 1, 1, NULL);
	std::cout << "intital decrement\n";
	WaitForSingleObject(ghSemaphore, INFINITE);

	context1_semaphore = CreateSemaphore(NULL, 1, 1, NULL);
	std::cout << "intital decrement\n";
	WaitForSingleObject(context1_semaphore, INFINITE);

	context2_semaphore = CreateSemaphore(NULL, 1, 1, NULL);
	std::cout << "intital decrement\n";
	WaitForSingleObject(context2_semaphore, INFINITE);

	// Create the file to store the IR camera timestamps
	f_irTimestamps = new std::ofstream((folderPath + "/ir_timestamps.txt").c_str());

	std::thread context1(contextOneSnapper);
	std::thread context2(contextTwoSnapper);

	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

	// Spawn thread to record frames
	std::thread recordThread(recordFrames, 1, useSeconds);
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
	context1.join();
	context2.join();

	/*
	std::cout << "Outputing times from buffer:\n";
	for (int i = 0; i < halfBufferSize * 2; i++) {
		std::cout << "Frame " << i << ": " << frameTimestamps[i] << "\n";
	}
	*/

	// Close semaphore
	CloseHandle(ghSemaphore);
	CloseHandle(context1_semaphore);
	CloseHandle(context2_semaphore);

	// Close files
	f_irTimestamps->close();
	delete f_irTimestamps;

	return 0;
}

void PXD::addContextCamera(ContextCamera &camera) {
	contextCameras.push_back(camera);
}
void PXD::setContextCamera1(ContextCamera &camera) {
	contextCamera_1 = camera;
}
void PXD::setContextCamera2(ContextCamera &camera) {
	contextCamera_2 = camera;
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