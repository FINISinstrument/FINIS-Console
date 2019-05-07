// FINIS-Console.cpp : This file contains the 'main' function. Program execution begins and ends here.

#include "pch.h"
#include <iostream>
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

// Vimba class
#include "Vimba.h"

//header files we have written
#include "IMU.h"

int main() {
	Vimba vimba = Vimba();
	vimba.startCamera();

	Shutter shutter = Shutter();

	
	//Initial Set up for the IMU
	uint32_t defaultBaudeRate = 115200;
	std::string defaultSensorPort = "COM12";
	IMU imu(defaultSensorPort, defaultBaudeRate, true);

	if (!imu.ConnectIMU()) {
		std::cout << "IMU FAILED TO CONNECT\n";
	}

	imu.getAsynchData();

	
	

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
		std::cout << "2: Get real time LLA data\n";
		std::cout << "3: Open shutter\n";
		std::cout << "4: Close shutter\n";
		std::cout << "5: Record stream\n";
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
			case 3: {
				shutter.openShutter();
				break;
			}
			case 4: {
				shutter.closeShutter();
				break;
			}
			case 5: {
				std::cout << "Enter frame count (max 400: ";
				int frameCount;
				std::cin >> frameCount;
				pxd.video(frameCount);
				
				break;
			}
			case 2: { //GET data from IMU (nonAsync) //TODO: make this Asynchronous
				//imu.getNonAsyncData();
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