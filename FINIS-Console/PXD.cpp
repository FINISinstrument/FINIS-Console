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
std::atomic<bool> PXD::finishedWithRecording = false;
std::atomic<bool> PXD::finishedWithSaving    = false;
std::atomic<bool> PXD::finishedWithContext   = false;
bool PXD::firstHandleRun = true;
HANDLE PXD::ghSemaphore = CreateSemaphore(NULL, 1, 1, NULL);
HANDLE PXD::context1_semaphore = CreateSemaphore(NULL, 1, 1, NULL);
HANDLE PXD::context2_semaphore = CreateSemaphore(NULL, 1, 1, NULL);
std::vector<ContextCamera> PXD::contextCameras = std::vector<ContextCamera>();
ContextCamera PXD::contextCamera_1 = ContextCamera();
ContextCamera PXD::contextCamera_2 = ContextCamera();
std::ofstream* PXD::f_irTimestamps = NULL;
std::ofstream* PXD::f_context1Timestamps = NULL;
std::ofstream* PXD::f_context2Timestamps = NULL;

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

bool PXD::parseCommand(std::string command) {
	// Take a command from a script file, and do the action
	// Valid commands are
	// collect <int> frames
	// collect <int> seconds
	// capture <image name>

	char* c_line = new char[command.size() + 1];
	std::copy(command.begin(), command.end(), c_line);
	c_line[command.size()] = '\0';

	char * word;
	word = strtok(c_line, " ");

	// Check if first word is capture
	if (strcmp(word, "capture") == 0) {
		word = strtok(NULL, " ");
		snap(word);
		return true;
	}

	// Check for collection
	if (strcmp(word, "collect") != 0) {
		return false;
	}

	// Get count to pass to record function
	int count = 0;
	word = strtok(NULL, " ");
	count = std::stoi(word);

	// Check whether to record seconds
	word = strtok(NULL, " ");
	bool useSeconds = (strcmp(word, "seconds") == 0);
	
	std::cout << "count: " << count << "\tseconds: " << useSeconds << "\n";

	video(count, useSeconds, false);
	
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

	pxd_goLiveSeq(1, 1, 401, 1, 0, videoPeriod);
	//while (pxd_goneLive(1, 0)) { Sleep(0); }

	last_time = 0;
	while (!finishedWithRecording || (i % 200 != 0)) {
		int workingIndex = i % 400;
		// Put timestamp into buffer
		time = pxd_capturedSysTicks(1);

		//std::cout << "I: " << i << "\tTime: " << time << "\n";
		if (time != last_time) {
			frameTimestamps[workingIndex] = time;
			last_time = time;
			i++;

			// Check for saving from buffer
			if (i % 200 == 0) {
				ReleaseSemaphore(ghSemaphore, 1, NULL);
				//finishedWithSaving = true;
			}

			ReleaseSemaphore(context1_semaphore, 1, NULL);
			ReleaseSemaphore(context2_semaphore, 1, NULL);
		}
	}
	finishedWithSaving = true;
	finishedWithContext = true;

	ReleaseSemaphore(context1_semaphore, 1, NULL);
	ReleaseSemaphore(context2_semaphore, 1, NULL);

	pxd_goUnLive(1);
	/*for (int i = 0; i < 400; i++) {
		std::cout << "  I: " << i << "\tTime: " << frameTimestamps[i] << "\n";
	}*/
}

void PXD::contextOneSnapper() {
	
	auto now = std::chrono::high_resolution_clock::now();
	auto last = now;
	auto duration = now.time_since_epoch();
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

	int div = 0;

	WaitForSingleObject(context1_semaphore, INFINITE);
	while (!finishedWithContext) {
		div = contextCamera_1.snap();
		// Save frame timestamp
		if (div == 0) {
			last = now;
			now = std::chrono::high_resolution_clock::now();
			duration = now.time_since_epoch();
			millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
			*f_context1Timestamps << millis << "\t";
			duration = now - last;
			millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
			*f_context1Timestamps << millis << "\n";
		}

		WaitForSingleObject(context1_semaphore, INFINITE);
	}
	
}
void PXD::contextTwoSnapper() {
	auto now = std::chrono::high_resolution_clock::now();
	auto last = now;
	auto duration = now.time_since_epoch();	
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

	int div = 0;

	WaitForSingleObject(context2_semaphore, INFINITE);
	while (!finishedWithContext) {
		div = contextCamera_2.snap();
		// Save frame timestamp
		if (div == 0) {
			last = now;
			now = std::chrono::high_resolution_clock::now();
			duration = now.time_since_epoch();
			millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
			*f_context2Timestamps << millis << "\t";
			duration = now - last;
			millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
			*f_context2Timestamps << millis << "\n";
		}

		WaitForSingleObject(context2_semaphore, INFINITE);
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
	//while ((frameCount < count && !secondsCount) || (duration < count && secondsCount)) {
	//while (!finishedWithVideo) {
	while (!finishedWithSaving) {
		firstHalf = 1 - firstHalf;
		// Wait for semaphore
		std::cout << "Waiting for semaphore\n";
		WaitForSingleObject(ghSemaphore, INFINITE); // Changing to be noninfinite
		std::cout << "got semaphore\n";

		// Update reference metrics
		t2 = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count();
		if ((frameCount+200 >= count && !secondsCount) || (duration >= count && secondsCount)) {
			finishedWithRecording = true;
		}

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
		// Update frame count
		frameCount += halfBufferSize;
	}
	// Should be done recording
	std::cout << "frameCount: " << frameCount << "\n";
}

int PXD::video(int frameCount, bool useSeconds, bool enableContext) {
	// Disable if live from a previous occurance or snap
	if (isStreaming) {
		pxd_goUnLive(1);
		isStreaming = false;
	}

	// Reset finished flag
	finishedWithRecording = false;
	finishedWithSaving = false;
	finishedWithContext = false;

	// Clear buffer of timestamps
	for (int i = 0; i < this->halfBufferSize * 2; i++) {
		frameTimestamps[i] = 0;
	}

	// Don't need to create folder, it will be created from FINIS-Console
	// TODO this functionality will need to be refactored during the script testing

	// Check if context cameras are enabled
	if (enableContext) {
		// Update context cameras working directory
		for (int i = 0; i < contextCameras.size(); i++) {
			contextCameras[i].setFilePath(folderPath);
		}
		contextCamera_1.setFilePath(folderPath);
		contextCamera_2.setFilePath(folderPath);
		std::cout << "Folder path: *" << folderPath << "*\n";
		contextCamera_1.setDivisor(1);
		contextCamera_2.setDivisor(1);
	}
	else {
		finishedWithContext = true;
	}
	
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

	std::thread context1(contextOneSnapper);
	std::thread context2(contextTwoSnapper);

	// Create the file to store the IR camera timestamps
	f_irTimestamps = new std::ofstream((folderPath + "/ir_timestamps.txt").c_str());
	if (enableContext) {
		f_context1Timestamps = new std::ofstream((folderPath + "/context1_timestamps.txt").c_str());
		f_context2Timestamps = new std::ofstream((folderPath + "/context2_timestamps.txt").c_str());
	}

	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

	// Spawn thread to record frames
	std::thread recordThread(recordFrames, 1);

	// Save images, wait for record to finish
	std::thread saveThread(saveFrames, frameCount, 1, useSeconds); // frames to save
	std::cout << " waiting for record thread\n";
	recordThread.join();
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
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
	if (enableContext) {
		f_context1Timestamps->close();
		f_context2Timestamps->close();
		delete f_context1Timestamps;
		delete f_context2Timestamps;
	}

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

std::string PXD::createFolder() {
	// Get directory to save images to
	dateTime = getDateTime();
	folderPath = baseVideoPath + dateTime;
	// Create directory
	CreateDirectoryA(folderPath.c_str(), NULL);
	std::cout << GetLastError() << "\n";

	return folderPath;
}

std::string PXD::getFolderPath() {
	return folderPath;
}

int PXD::openPXD() {
	return pxd_PIXCIopen("", "", "C:/Users/FINIS/source/repos/FINIS-Console/FINIS-Console/Resources/XCAPVideoSetup16Bit30Hz.fmt");
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