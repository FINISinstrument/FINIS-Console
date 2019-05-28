// FINIS-Console.cpp : This file contains the 'main' function. Program execution begins and ends here.

#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <memory>

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
		delete c_line;
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
	

	// Open the pxd opject
	/*
	std::cout << "Opening frame grabber...\n";
	int openError = pxd_PIXCIopen("", "", "C:/Users/FINIS/source/repos/FINIS-Console/FINIS-Console/Resources/XCAPVideoSetup16Bit30Hz.fmt");
	if (openError < 0) {
		printf("error: %s\n", pxd_mesgErrorCode(openError));
	}
	Sleep(1000);
	*/
	PXD pxd ("C:/FINIS/testing");
	std::cout << "Opened frame grabber\n";

	// Open context cameras
	ContextCamera context1(0, "C:/FINIS/testing", "context1");
	ContextCamera context2(1, "C:/FINIS/testing", "context2");

	// Add context cameras to PXD object
	pxd.setContextCamera1(context1);
	pxd.setContextCamera2(context2);

	/*
	// Go live
	pxd_goLive(1, 1);
	while (!pxd_goneLive(1,0)) { Sleep(0); }
	std::cout << "Gone live\n";
	*/

	int cmd;
	bool done = false;
	while (!done) {
		std::cout << "0: End Program\n";
		std::cout << "1: Snap a photo\n";
		//LLA: Longitude, Latitude, Altitude
		std::cout << "2: Get real time LLA data\n";
		std::cout << "3: Open shutter\n";
		std::cout << "4: Close shutter\n";
		std::cout << "5: Record stream\n";
		std::cout << "6: Run script\n";
		std::cout << "Enter a cmd:";
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
				imu.getAsynchData();
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
				std::cout << "Enter frame count: ";
				int frameCount;
				std::cin >> frameCount;
				pxd.video(frameCount, true);
				
				break;
			}
			case 6: {
				//std::cout << "Enter path to script: ";
				std::string path;
				//std::cin >> path;
				path = "C:/FINIS/testing/scripts/script1.txt";
				parseScript(path, &vimba, &shutter, &imu, &pxd);
				break;
			}
			default: { //print out all the options for keystrokes
				std::cout << "Not a valid argument type\n";

				break;
			}
		}

		// Disconnect the IMU
		//imu.~IMU();
	} // End while
	// Close the pxd object
	//pxd_PIXCIclose();
	std::cout << "Hello World!\n";

}