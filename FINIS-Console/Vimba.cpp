#include <iostream>

#include "Vimba.h"

/**
 * Constructor
 * Initialize startup behavior, but do not open the camera
 */
Vimba::Vimba() :
	sys(AVT::VmbAPI::VimbaSystem::GetInstance())
{

}

/**
 * Destructor
 * Close camera before shutdown
 */
Vimba::~Vimba() {
	if (cameraConnected) {
		std::cout << "Closing camera...";
		camera->Close();
		sys.Shutdown();
		std::cout << "done\n";
	}
}

/**
 * Start the camera
 * This function can be called asyncronously so that other startup behavior can occur
 * Returns TRUE upon successful completion
 */
bool Vimba::startCamera() {
	// Shutdown system in case it was not properly closed
	sys.Shutdown();

	// Start system and get first camera (this instrament only has one camera attached)
	sys.Startup();
	sys.GetCameras(cameras);
	std::cout << "Found " << cameras.size() << " IR camera(s)\n";
	if (cameras.size() > 0) {
		camera = cameras[0];

		// Set features for the camera
		sys.OpenCameraByID("DEV_64AA2C448F1F2349", VmbAccessModeFull, camera);
		camera->GetFeatureByName("ExposureTime", feature);
		//feature->SetValue(33334.0);
		exposure = 8000.0;
		feature->SetValue(exposure);
		camera->GetFeatureByName("AcquisitionFrameRate", feature);
		framerate = 30.0;
		feature->SetValue(framerate);

		camera->GetFeatureByName("TriggerSource", feature);
		feature->SetValue("FixedRate");

		camera->GetFeatureByName("SensorGain", feature);
		feature->SetValue("Gain1");
		camera->GetFeatureByName("SensorTemperatureSetpointValue", feature);
		feature->SetValue(20);
		camera->GetFeatureByName("SensorTemperatureSetpointSelector", feature);
		feature->SetValue(1);
		camera->GetFeatureByName("AcquisitionStart", feature);
		feature->RunCommand();
	}
	else {
		std::cout << "No camera was opened. Reboot the computer\n";
	}
	cameraConnected = cameras.size() != 0;
	return cameras.size() != 0;
}

double Vimba::getExposure() {
	return exposure;
}
double Vimba::getFramerate() {
	return framerate;
}
double Vimba::getMaxFramerate() {
	return 1000000.0 / exposure;
}

void Vimba::updateExposure(double exposure) {
	//camera->GetFeatureByName("AcquisitionStop", feature);
	//feature->RunCommand();
	camera->GetFeatureByName("ExposureTime", feature);
	feature->SetValue(exposure);
	this->exposure = exposure;
	//camera->GetFeatureByName("AcquisitionStart", feature);
	//feature->RunCommand();
}

void Vimba::updateFramerate(double frameRate) {
	camera->GetFeatureByName("AcquisitionFrameRate", feature);
	feature->SetValue(frameRate);
	this->framerate = frameRate;
}