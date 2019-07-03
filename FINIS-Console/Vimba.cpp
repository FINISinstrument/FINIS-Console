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
	std::cout << "Closing camera...";
	camera->Close();
	sys.Shutdown();
	std::cout << "done\n";
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
	std::cout << cameras.size() << "\n";
	camera = cameras[0];

	// Set features for the camera
	sys.OpenCameraByID("DEV_64AA2C448F1F2349", VmbAccessModeFull, camera);
	camera->GetFeatureByName("ExposureTime", feature);
	//feature->SetValue(30000.0);
	//feature->SetValue(33330.0);
	//feature->SetValue(33334.0);
	feature->SetValue(8000.0);
	camera->GetFeatureByName("AcquisitionFrameRate", feature);
	//feature->SetValue(30.0);
	feature->SetValue(30.0);
	camera->GetFeatureByName("SensorGain", feature);
	feature->SetValue("Gain1");
	camera->GetFeatureByName("SensorTemperatureSetpointValue", feature);
	feature->SetValue(20);
	camera->GetFeatureByName("SensorTemperatureSetpointSelector", feature);
	feature->SetValue(1);
	camera->GetFeatureByName("AcquisitionStart", feature);
	feature->RunCommand();

	return true;
}