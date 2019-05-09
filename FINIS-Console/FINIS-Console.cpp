// FINIS-Console.cpp : This file contains the 'main' function. Program execution begins and ends there.

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

// Context camera
#include "ContextCamera.h"

int main() {
	Vimba vimba = Vimba();
	vimba.startCamera();

	Shutter shutter = Shutter();
	
	//Initial Set up for the IMU
	/*
	uint32_t defaultBaudeRate = 115200;
	std::string defaultSensorPort = "COM12";
	VnSensor vs;
	vs.connect(defaultSensorPort, defaultBaudeRate);
	if (vs.isConnected()) {
		std::cout << "Connected IMU at " << defaultSensorPort << " with BaudeRate " << defaultBaudeRate << std::endl;
	}
	else {
		std::cout << "Failed to connect IMU at " << defaultSensorPort << " with BaudeRate " << defaultBaudeRate << std::endl;
	}

	//Declare what we expect to get out of the IMU (GPS DATA)
	BinaryOutputRegister bor(
		ASYNCMODE_PORT1,
		200,
		COMMONGROUP_NONE,
		TIMEGROUP_NONE,
		IMUGROUP_NONE,
		GPSGROUP_POSLLA,
		ATTITUDEGROUP_NONE,
		INSGROUP_NONE
	);

	vs.writeBinaryOutput1(bor);
	*/

	// Open the pxd opject
	PXD pxd ("C:/FINIS/testing");
	std::cout << "Opened frame grabber\n";

	ContextCamera context1("C:/FINIS/testing/context/");

	int cmd;
	bool done = false;
	while (!done) {
		std::cout << "0: End Program\n";
		std::cout << "1: Snap a photo\n";
		std::cout << "2: Get real time LLA data\n";
		std::cout << "3: Open shutter\n";
		std::cout << "4: Close shutter\n";
		std::cout << "5: Record stream (instrument)\n";
		std::cout << "6: Record stream (context)\n";
		std::cout << "7: Snap context photo\n";
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
			case 6: {
				context1.recordFrames(30);
				break;
			}
			case 7: {
				context1.snap();
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