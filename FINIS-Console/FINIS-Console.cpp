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

std::string ZeroPadString(int num, int size) {
	std::ostringstream ss;
	ss << std::setw(size) << std::setfill('0') << num;
	return ss.str();
}

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
	ContextCamera context1(0, "C:/FINIS/testing", "context1");
	ContextCamera context2(1, "C:/FINIS/testing", "context2");

	// Add context cameras to PXD object
	pxd.setContextCamera1(context1);
	pxd.setContextCamera2(context2);

	int cmd;
	bool done = false;
	while (!done) {
		std::cout << "0: End Program\n";
		std::cout << "1: Snap a photo (IR camera)\n";
		//LLA: Longitude, Latitude, Altitude
		std::cout << "2: Get real time LLA data (IMU)\n";
		std::cout << "3: Open shutter\n";
		std::cout << "4: Close shutter\n";
		std::cout << "5: Change exposure (IR)\n";
		std::cout << "6: Change framerate (IR)\n";
		std::cout << "7: Full recording (IR, 2 context, IMU), timed with frames\n";
		std::cout << "8: Full recording (IR, 2 context, IMU), timed with seconds\n";
		std::cout << "9: Vehicle run (IR, IMU), timed with seconds\n";
		std::cout << "10: Flat field test (IR)\n";
		std::cout << "Enter a cmd:";;
		std::cin >> cmd;

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
				double exposure;
				std::cout << "Enter new exposure (between 0 and 34000): ";
				std::cin >> exposure;
				vimba.updateExposure(exposure);
			}
			case 6: {
				std::cout << "Current framerate is " << vimba.getFramerate() << "\n";
				std::cout << "Max framerate is " << vimba.getMaxFramerate() << "\n";
				double framerate;
				std::cout << "Enter new framerate: ";
				std::cin >> framerate;
				vimba.updateFramerate(framerate);
			}
			case 7: {
				std::cout << "Enter frame count: ";
				int frameCount;
				std::cin >> frameCount;

				std::string filePath = pxd.createFolder();
				imu.setFilePath(filePath);
				imu.startAsynchData();
				pxd.video(frameCount, false, true);
				imu.stopAsynchData();
				
				break;
			}
			case 8: {
				std::string location;
				std::string speed;
				std::string distance;
				std::string trial;
				int timeToRecord;

				std::cout << "Enter location: ";
				std::cin >> location;
				std::cout << "Enter speed: ";
				std::cin >> speed;
				std::cout << "Enter distance to scene: ";
				std::cin >> distance;
				std::cout << "Enter trial number: ";
				std::cin >> trial;
				std::cout << "Enter time to record (in seconds): ";
				std::cin >> timeToRecord;

				std::string filePath = pxd.createFolder();
				imu.setFilePath(filePath);

				std::cout << "Press enter to begin...";
				std::cin.ignore();
				std::cin.ignore();

				imu.startAsynchData();
				pxd.video(timeToRecord, true, true);
				imu.stopAsynchData();

				// Create file for logging the trial data to
				std::ofstream loggerFile = std::ofstream((filePath + "/trial_data.txt").c_str());
				loggerFile << "Location: " << location << "\n";
				loggerFile << "Speed:    " << speed << "\n";
				loggerFile << "Distance: " << distance << "\n";
				loggerFile << "Trial No: " << trial << "\n";
				loggerFile << "Time recording: " << timeToRecord << "\n";

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
			case 9: {
				std::string location;
				std::string speed;
				std::string distance;
				std::string trial;
				int timeToRecord;

				std::cout << "Enter location: ";
				std::cin >> location;
				std::cout << "Enter speed: ";
				std::cin >> speed;
				std::cout << "Enter distance to scene: ";
				std::cin >> distance;
				std::cout << "Enter trial number: ";
				std::cin >> trial;
				std::cout << "Enter time to record (in seconds): ";
				std::cin >> timeToRecord;

				std::string filePath = pxd.createFolder();
				imu.setFilePath(filePath);

				std::cout << "Press enter to begin...";
				std::cin.ignore();
				std::cin.ignore();

				imu.startAsynchData();
				pxd.video(timeToRecord, true, false);
				imu.stopAsynchData();

				// Create file for logging the trial data to
				std::ofstream loggerFile = std::ofstream((filePath + "/trial_data.txt").c_str());
				loggerFile << "Location: " << location << "\n";
				loggerFile << "Speed:    " << speed << "\n";
				loggerFile << "Distance: " << distance << "\n";
				loggerFile << "Trial No: " << trial << "\n";
				loggerFile << "Time recording: " << timeToRecord << "\n";

				break;
			}
			case 10: {
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
						int minimum = 1000; // Measured in microseconds
						int maximum = 33000; // Measured in microseconds
						int stepSize = 1000; // Meausred in microseconds
						int frameCount = 10; // Frames to capture per run
						pxd_goUnLive(1);

						// Wait for user input to change cells
						std::cout << "Beginning calibration of " << gasses[i] << " in " << cellLengths[j] << " cell. Press Enter to begin calibration...";
						std::cin.ignore();

						for (int i = minimum; i <= maximum; i += stepSize) {
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
			default: { //print out all the options for keystrokes
				std::cout << "Not a valid argument type\n";

				break;
			}
		}
	} // End while
	std::cout << "Hello World!\n";

}