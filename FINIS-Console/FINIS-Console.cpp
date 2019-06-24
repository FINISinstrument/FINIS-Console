// FINIS-Console.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "pch.h"
#include <iostream>
#include <string>
#include <time.h>
#include <memory>

#include <iomanip>
#include <sstream>

#include <windows.h>

// Shutter header
#include "Shutter.h"
// PIXCI header
#include "XCLIB/xcliball.h"

// Vimba class
#include "Vimba.h"

//header files we have written
#include "IMU.h"

std::string ZeroPadString(int num, int size) {
	std::ostringstream ss;
	ss << std::setw(size) << std::setfill('0') << num;
	return ss.str();
}

int main() {
	Vimba vimba = Vimba();
	vimba.startCamera();

	Shutter shutter = Shutter();

	/*
	//Initial Set up for the IMU
	uint32_t defaultBaudeRate = 115200;
	std::string defaultSensorPort = "COM12";
	//TODO: Somehow I am not declaring my IMU correctly :/ 
	//IMU imu = IMU(defaultSensorPort, defaultBaudeRate);
	//imu.ConnectIMU();
	*/

	// Open the pxd opject
	std::cout << "Opening frame grabber...\n";
	int openError = pxd_PIXCIopen("", "", "C:/Users/FINIS/source/repos/FINIS-Console/FINIS-Console/Resources/XCAPVideoSetup16Bit30Hz.fmt");
	if (openError < 0) {
		printf("error: %s\n", pxd_mesgErrorCode(openError));
	}
	Sleep(1000);
	std::cout << "Opened frame grabber\n";

	// Go live
	pxd_goLive(1, 1);
	while (!pxd_goneLive(1,0)) { Sleep(0); }
	std::cout << "Gone live\n";

	int cmd;
	bool done = false;
	while (!done) {
		std::cout << "Enter a cmd:";
		std::cin >> cmd;

		switch (cmd) {
			case 0: { //end the program
				done = true;
				break;
			}
			case 1: {// Snap a picture!
				pxd_goSnap(1, 1);
				Sleep(50);
				std::cout << "Snap picture\n";
				// save image
				int saveTiffError = pxd_saveTiff(1, "C:/Users/FINIS/Desktop/image.tiff", 1, 0, 0, -1, -1, 0, 0);
				if (saveTiffError < 0) {
					printf("error: %s\n", pxd_mesgErrorCode(saveTiffError));
				}
				Sleep(100);
				std::cout << "\nSave image\n";


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
				std::cin.ignore();
				std::string gasses [2] = {"Methane", "Nitrogen"};
				std::string cellLengths[3] = {"10.16cm", "25cm", "50cm"};

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
			case 2: { //GET data from IMU (nonAsync) //TODO: make this Asynchronous
				//imu.getNonAsyncData();
			}
			default: { //print out all the options for keystrokes
				std::cout << "Not a valid argument type\n";
				std::cout << "0: End Program\n";
				std::cout << "1: Snap a photo\n";
				std::cout << "2: Get real time LLA data\n";
				std::cout << "3: Open shutter\n";
				std::cout << "4: Close shutter\n";
				std::cout << "5: Calibration test\n";

				break;
			}
		}

		// Disconnect the IMU
		//imu.~IMU();
	} // End while
	// Close the pxd object
	pxd_PIXCIclose();
	std::cout << "Hello World!\n";

}