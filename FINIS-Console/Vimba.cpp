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
bool Vimba::startCamera(std::string configuration) {
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
		// If configuration exists, open according to config
		if (configuration.size() != 0) {
			std::fstream configFile(configuration.c_str());

			// Loop through lines of file
			std::string featureName;
			std::string featureValue;
			float featureFloatValue;

			// Read all values
			while (configFile >> featureName) {
				configFile >> featureValue;

				camera->GetFeatureByName(featureName.c_str(), feature);

				std::cout << featureName << "\t" << featureValue << "\n";

				// Try converting to float
				try {
					featureFloatValue = std::stof(featureValue);
					// Float found and converted
					feature->SetValue(featureFloatValue);
				}
				catch (const std::exception& e){
					// No conversion, is string
					feature->SetValue(featureValue.c_str());
				}
			}

			configFile.close();
		}
		// If no configuration specified, run this
		else {
			camera->GetFeatureByName("ExposureTime", feature);
			//feature->SetValue(33334.0);
			exposure = 8000.0;
			feature->SetValue(exposure);
			//camera->GetFeatureByName("AcquisitionFrameRate", feature);
			framerate = 30.0;
			//feature->SetValue(framerate);

			//camera->GetFeatureByName("TriggerSource", feature);
			//feature->SetValue("FixedRate");

			camera->GetFeatureByName("SensorGain", feature);
			feature->SetValue("Gain1");
			camera->GetFeatureByName("SensorTemperatureSetpointValue", feature);
			feature->SetValue(20);
			camera->GetFeatureByName("SensorTemperatureSetpointSelector", feature);
			feature->SetValue(1);

			// This seems to remove the vertical jump in the graph
			camera->GetFeatureByName("NUCMode", feature);
			feature->SetValue("Off");

			// Having this off made it wavy
			camera->GetFeatureByName("DPCMode", feature);
			feature->SetValue("Off");

			// having this feature off removes the main dip in the dark current vs. exposure time graph
			camera->GetFeatureByName("SensorTemperatureControlMode", feature);
			feature->SetValue("Off");

			/*
			camera->GetFeatureByName("BCMode", feature);
			feature->SetValue("Off");
			*/
		}

		camera->GetFeatureByName("AcquisitionStart", feature);
		feature->RunCommand();
	}
	else {
		std::cout << "No camera was opened. Reboot the computer\n";
	}
	cameraConnected = cameras.size() != 0;
	return cameras.size() != 0;
}

float Vimba::getExposure() {
	return exposure;
}
float Vimba::getFramerate() {
	return framerate;
}
float Vimba::getMaxFramerate() {
	return 1000000.0 / exposure;
}
float Vimba::getTemperature() {
	camera->GetFeatureByName("DeviceTemperature", feature);
	double temp;
	return feature->GetValue(temp );
}

void Vimba::updateExposure(float exposure) {
	//camera->GetFeatureByName("AcquisitionStop", feature);
	//feature->RunCommand();
	camera->GetFeatureByName("ExposureTime", feature);
	feature->SetValue(exposure);
	this->exposure = exposure;
	//camera->GetFeatureByName("AcquisitionStart", feature);
	//feature->RunCommand();
}

void Vimba::updateFramerate(float frameRate) {
	camera->GetFeatureByName("AcquisitionFrameRate", feature);
	feature->SetValue(frameRate);
	this->framerate = frameRate;
}