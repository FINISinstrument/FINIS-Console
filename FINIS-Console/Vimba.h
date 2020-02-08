#pragma once

// Class to handle Vimba camera helper functions, along with startup and shutdown procedures

#include "VimbaCPP/Include/VimbaCPP.h"

class Vimba {
	public:
		Vimba();
		~Vimba();

		bool startCamera(std::string configuration);

		float getExposure();
		float getFramerate();
		float getMaxFramerate();
		float getTemperature();

		void updateExposure(float exposureTime);
		void updateFramerate(float frameRate);
	private:
		AVT::VmbAPI::VimbaSystem &sys;
		AVT::VmbAPI::CameraPtrVector cameras;
		AVT::VmbAPI::CameraPtr camera;
		AVT::VmbAPI::FeaturePtr feature;
		bool cameraConnected;

		const std::string configurationPath = "C:/FINIS/configuration";

		float exposure;
		float framerate;
};