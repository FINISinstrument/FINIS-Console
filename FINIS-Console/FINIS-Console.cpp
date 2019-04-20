// ConsoleTesting.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <string>
#include <time.h>
#include <memory>

#include <windows.h>
// PIXCI header
#include "XCLIB/xcliball.h"
// Vimba header
#include "VimbaCPP/Include/VimbaCPP.h"


//header files we have written
#include "IMU.h"

int main() {


	AVT::VmbAPI::VimbaSystem &sys = AVT::VmbAPI::VimbaSystem::GetInstance();
	AVT::VmbAPI::CameraPtrVector cameras;
	AVT::VmbAPI::CameraPtr camera;
	AVT::VmbAPI::FeaturePtr feature;

	sys.Shutdown();

	sys.Startup();
	sys.GetCameras(cameras);
	std::cout << cameras.size() << "\n";
	camera = cameras[0];

	sys.OpenCameraByID("DEV_64AA2C448F1F2349", VmbAccessModeFull, camera);
	camera->GetFeatureByName("ExposureTime", feature);
	feature->SetValue(60000.0);
	camera->GetFeatureByName("AcquisitionFrameRate", feature);
	feature->SetValue(15.0);
	camera->GetFeatureByName("SensorGain", feature);
	feature->SetValue("Gain1");
	camera->GetFeatureByName("SensorTemperatureSetpointValue", feature);
	feature->SetValue(20);
	camera->GetFeatureByName("SensorTemperatureSetpointSelector", feature);
	feature->SetValue(1);
	camera->GetFeatureByName("AcquisitionStart", feature);
	feature->RunCommand();

	/*
	//Initial Set up for the IMU
	uint32_t defaultBaudeRate = 115200;
	std::string defaultSensorPort = "COM12";
	//TODO: Somehow I am not declaring my IMU correctly :/ 
	//IMU imu = IMU(defaultSensorPort, defaultBaudeRate);
	//imu.ConnectIMU();
	*/

	// Open the pxd opject
	pxd_PIXCIopen("", "", "C:/Users/FINIS/source/repos/ConsoleTesting/ConsoleTesting/Resources/XCAPVideoSetup16Bit15Hz.fmt");
	Sleep(1000);
	std::cout << "Opened frame grabber\n";

	// Go live
	pxd_goLive(1, 1);
	Sleep(1000);
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
			pxd_saveTiff(1, "C:/Users/FINIS/Desktop/image.tiff", 1, 0, 0, -1, -1, 0, 0);
			Sleep(100);
			std::cout << "Save image\n";

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

			break;
		}
		}

		// Disconnect the IMU
		//imu.~IMU();

		// Close the pxd object
		pxd_PIXCIclose();
		std::cout << "Hello World!\n";

		camera->Close();
		sys.Shutdown();
		std::cout << "Shutdown\n";
	}
}