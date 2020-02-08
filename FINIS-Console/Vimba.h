#pragma once

// Class to handle Vimba camera helper functions, along with startup and shutdown procedures

#include "VimbaCPP/Include/VimbaCPP.h"

class Vimba {
	public:
		Vimba();
		~Vimba();

		bool startCamera();

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

		float exposure;
		float framerate;
};