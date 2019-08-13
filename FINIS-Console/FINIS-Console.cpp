// FINIS-Console.cpp : This file contains the 'main' function. Program execution begins and ends here.

#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <memory>
#include <stdlib.h>

#include <windows.h>

// For profiling
#include <chrono>

// Shutter header
#include "Shutter.h"
// PIXCI header
#include "XCLIB/xcliball.h"
// PXD
#include "PXD.h"

// Context camera
#include "contextCamera.h"

// Vimba class
#include "Vimba.h"

//header files we have written
#include "IMU.h"

void parseScript(std::string scriptPath, Vimba* vimba, Shutter* shutter, IMU* imu, PXD* pxd) {
	// Open the script file
	std::ifstream script(scriptPath);
	if (!script.is_open()) {
		return;
	}

	// Loop through lines of file and execute commands
	std::string line;
	while (getline(script, line)) {
		char* c_line = new char[line.size() + 1];
		line.copy(c_line, line.size() + 1);
		c_line[line.size()] = '\0';

		c_line = strtok(c_line, " ");

		if ( strcmp(c_line, "shutter")==0 ) {
			shutter->parseCommand(line);
		}
		else if ( strcmp(c_line, "capture") == 0 || strcmp(c_line, "collect") == 0 ) {
			pxd->parseCommand(line);
		}
		else if (strcmp(c_line, "sleep") == 0) {
			c_line = strtok(NULL, " ");
			int count = std::stoi(c_line);
			Sleep(1000 * count);
		}

		/*
		std::cout << "C_Line: *" << c_line << "*\n";
		if (c_line != NULL) {
			std::cout << "Pre delete\n";
			delete c_line;
			std::cout << "Post delete\n";
		}
		std::cout << "Next command\n";
		*/
	}

	script.close();
}

void createLogFile(std::string basePath, Vimba* const vimba, int timeRecording, bool useSeconds) {
	// Create log file
	std::ofstream logFile = std::ofstream((basePath + "/trial_data.txt").c_str());

	// Create temporary variables
	std::string user_str;
	int user_int;

	// Get user input for different values
	std::cout << "Enter location: ";
	std::cin >> user_str;
	logFile << "Location: " << user_str << "\n";

	std::cout << "Enter speed: ";
	std::cin >> user_str;
	logFile << "Speed:    " << user_str << "\n";

	std::cout << "Enter distance to scene: ";
	std::cin >> user_str;
	logFile << "Distance: " << user_str << "\n";

	std::cout << "Enter trial number: ";
	std::cin >> user_str;
	logFile << "Trial No: " << user_str << "\n";

	logFile << "Time recording: " << timeRecording << "\n";
	logFile << "Using seconds:  " << (useSeconds ? "TRUE" : "FALSE") << "\n";

	logFile << "Exposure: " << vimba->getExposure() << "\n";
	logFile << "Frame Rate: " << vimba->getFramerate() << "\n";

	// Close log file
	logFile.close();

	// Wait for user input
	std::cout << "Press enter to begin...";
	std::cin.ignore();
	std::cin.ignore();
}

/* Function to handle recording of data */
void recordData(ContextCamera* context1, ContextCamera* context2, PXD* pxd, IMU* imu, Vimba* vimba, int duration, bool useSeconds, bool useLogFile) {
	// Determine folder that will be saved to
	std::string baseSavePath = createBaseSavePath("C:/FINIS/testing/Video");

	// Create and write log file if desired
	if (useLogFile) {
		createLogFile(baseSavePath, vimba, duration, useSeconds);
	}

	std::atomic<bool> contextComplete = false;

	// Initialize semaphores and threads
	std::vector<std::thread> threads;
	HANDLE semaphore_context1 = CreateSemaphore(NULL, 0, 1, NULL);
	HANDLE semaphore_context2 = CreateSemaphore(NULL, 0, 1, NULL);
	HANDLE semaphore_imu      = CreateSemaphore(NULL, 0, 1, NULL);

	// Spawn thread for context (context1), if enabled
	if (context1 != nullptr) {
		context1->setFilePath(baseSavePath);
		threads.push_back(std::thread(&ContextCamera::record, context1, std::ref(semaphore_context1), std::ref(contextComplete)));
	}

	// Spawn thread for aBand (context2)
	if (context2 != nullptr) {
		context2->setFilePath(baseSavePath);
		threads.push_back(std::thread(&ContextCamera::record, context2, std::ref(semaphore_context2), std::ref(contextComplete)));
	}
	
	// Spawn thread for IMU
	if (imu != nullptr) {
		imu->setFilePath(baseSavePath);
		threads.push_back(std::thread(&IMU::asynchData, imu, semaphore_imu));
	}

	// Spawn thread for PXD
	pxd->setFilePath(baseSavePath);
	std::thread thread_pxd(&PXD::video, pxd, duration, useSeconds, false);

	// Signal everything to begin
	ReleaseSemaphore(semaphore_context1, 1, NULL);
	ReleaseSemaphore(semaphore_context2, 1, NULL);

	// Wait for PXD to finish
	thread_pxd.join();

	// Signal everything to stop
	ReleaseSemaphore(semaphore_imu, 1, NULL);
	contextComplete = true;

	// Join threads
	for (unsigned int i = 0; i < threads.size(); i++) {
		threads[i].join();
	}

	// Close semaphores
	CloseHandle(semaphore_context1);
	CloseHandle(semaphore_context2);
	CloseHandle(semaphore_imu);
}

int main() {
	Vimba vimba = Vimba();
	vimba.startCamera();

	Shutter shutter = Shutter();
	
	//Initial Set up for the IMU
	uint32_t defaultBaudeRate = 115200;
	std::string defaultSensorPort = "COM50";
	//If true, IMU data will print to the screen
	bool print = true;
	IMU imu(defaultSensorPort, defaultBaudeRate, print);
	imu.ConnectIMU();
	
	PXD pxd ("C:/FINIS/testing", false);
	std::cout << "Opened frame grabber\n";

	// Open context cameras
	ContextCamera context1(0, "C:/FINIS/testing", "context1", true);
	ContextCamera context2(1, "C:/FINIS/testing", "context2", true);

	int cmd;
	bool done = false;
	while (!done) {
		std::cout << "\n\n\n\n\n\n\n\n\n";
		std::cout << "  0: End Program\n";
		std::cout << "  1: Snap a photo (IR camera)\n";
		//LLA: Longitude, Latitude, Altitude
		std::cout << "  2: Get real time LLA data (IMU)\n";
		std::cout << "  3: Open shutter\n";
		std::cout << "  4: Close shutter\n";
		std::cout << "  5: Change exposure (IR)\n";
		std::cout << "  6: Full recording (IR, 2 context, IMU), timed with frames\n";
		std::cout << "  7: Full recording (IR, 2 context, IMU), timed with seconds\n";
		std::cout << "  8: Vehicle run (IR, IMU), timed with seconds\n";
		std::cout << "  9: Flat field test (IR)\n";
		std::cout << " 10: Single calibration run (IR)\n";
		std::cout << " 11: Refactored test\n";
		std::cout << "Enter a cmd:";;
		std::cin >> cmd;
		std::cout << "\n";

		switch (cmd) {
			case 0: { //end the program
				done = true;
				break;
			}
			case 1: {// Snap a picture!
				pxd.snap("snapper");
				break;
			}
			case 2: {
				imu.setFilePath("C:/FINIS/testing/");
				imu.debugGetAsynchData();
				break;
			}
			case 3: {
				shutter.openShutter();
				break;
			}
			case 4: {
				shutter.closeShutter();
				break;
			}
			case 5: {
				std::cout << "Current exposure is " << vimba.getExposure() << "\n";
				float exposure;
				std::cout << "Enter new exposure (between 0 and 34000): ";
				std::cin >> exposure;
				vimba.updateExposure(exposure);

				break;
			}
			case 6: {
				// Full recording.
				// Use frames, do not create log file
				std::cout << "Enter frame count: ";
				int duration;
				std::cin >> duration;

				recordData(&context1, &context2, &pxd, &imu, &vimba, duration, false, false);

				break;
			}
			case 7: {
				// Full recording.
				// Use seconds, and create log file
				std::cout << "Enter time to record (in seconds): ";
				int duration;
				std::cin >> duration;

				recordData(&context1, &context2, &pxd, &imu, &vimba, duration, true, true);

				break;
			}
			/*
			case 6: {
				//std::cout << "Enter path to script: ";
				std::string path;
				//std::cin >> path;
				path = "C:/FINIS/testing/scripts/script1.txt";
				parseScript(path, &vimba, &shutter, &imu, &pxd);
				break;
			}
			*/
			case 8: {
				// Only IR and IMU
				// Use seconds, and create log file
				std::cout << "Enter time to record (in seconds): ";
				int duration;
				std::cin >> duration;

				recordData(nullptr, nullptr, &pxd, &imu, &vimba, duration, true, true);
				
				break;
			}
			case 9: {
				std::cin.ignore();
				std::string gasses[2] = { "Methane", "Nitrogen" };
				std::string cellLengths[3] = { "10.16cm", "25cm", "50cm" };

				// Loop through gasses
				for (int i = 0; i < 2; i++) {
					std::string gasPath = "C:/FINIS/calibration/" + gasses[i] + "/";
					// Attemp to create subfolder
					CreateDirectoryA((gasPath).c_str(), NULL);
					// Loop through test cells
					for (int j = 0; j < 3; j++) {
						std::string cellPath = gasPath + cellLengths[j] + "/";
						// Attemp to create subfolder
						CreateDirectoryA((cellPath).c_str(), NULL);
						std::cout << "BasePath: *" << cellPath << "*\n";
						float minimum = 1000; // Measured in microseconds
						float maximum = 33000; // Measured in microseconds
						float stepSize = 1000; // Meausred in microseconds
						float frameCount = 10; // Frames to capture per run
						pxd_goUnLive(1);

						// Wait for user input to change cells
						std::cout << "Beginning calibration of " << gasses[i] << " in " << cellLengths[j] << " cell. Press Enter to begin calibration...";
						std::cin.ignore();

						for (float i = minimum; i <= maximum; i += stepSize) {
							std::cout << "Aquiring at " << i << " microsecond exposure\n";
							// Change exposure
							vimba.updateExposure(i);
							// Take image
							std::cout << "Starting capture\n";
							pxd_goLiveSeq(1, 1, 401, 1, frameCount, 1);
							// Wait for completion
							while (pxd_goneLive(1, 0)) { Sleep(0); }
							// Save images
							std::cout << "Saving images\n";
							for (int j = 1; j <= frameCount; j++) {
								pxd_saveTiff(1, (cellPath + ZeroPadString(i, 5) + "_" + ZeroPadString(j, 2) + ".tiff").c_str(), j, 0, 0, -1, -1, 0, 0);
							}
						}
					}
				}
				pxd_goLive(1, 1);
				break;
			}
			case 10: {
				std::cin.ignore();
				
				std::string path = "C:/FINIS/calibration/ExposureTest/";
				// Attemp to create subfolder
				CreateDirectoryA((path).c_str(), NULL);
				float minimum = 1000; // Measured in microseconds
				float maximum = 33000; // Measured in microseconds
				float stepSize = 1000; // Meausred in microseconds
				float frameCount = 10; // Frames to capture per run
				pxd_goUnLive(1);

				for (float i = minimum; i <= maximum; i += stepSize) {
					std::cout << "Aquiring at " << i << " microsecond exposure\n";
					// Change exposure
					vimba.updateExposure(i);
					// Take image
					std::cout << "Starting capture\n";
					pxd_goLiveSeq(1, 1, 401, 1, frameCount, 1);
					// Wait for completion
					while (pxd_goneLive(1, 0)) { Sleep(0); }
					// Save images
					std::cout << "Saving images\n";
					for (int j = 1; j <= frameCount; j++) {
						pxd_saveTiff(1, (path + ZeroPadString(i, 5) + "_" + ZeroPadString(j, 2) + ".tiff").c_str(), j, 0, 0, -1, -1, 0, 0);
					}
				}
				
				pxd_goLive(1, 1);
				break;
			}
			case 11: {
				std::cout << "Enter seconds to capture: ";
				int frameCount;
				std::cin >> frameCount;

				recordData(&context1, &context2, &pxd, &imu, &vimba, frameCount, true, false);
				//recordData(nullptr, nullptr, &pxd, nullptr, frameCount, true);
				break;
			}
			default: { //print out all the options for keystrokes
				std::cout << "Not a valid argument type\n";

				break;
			}
		}
	} // End while
	std::cout << "Hello World!\n";

}
