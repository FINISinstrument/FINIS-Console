#pragma once

// Class to handle Vimba camera helper functions, along with startup and shutdown procedures

#include "VimbaCPP/Include/VimbaCPP.h"

class Vimba {
	public:
		Vimba();
		~Vimba();

		bool startCamera();

		double getExposure();
		double getFramerate();
		double getMaxFramerate();

		void updateExposure(double exposureTime);
		void updateFramerate(double frameRate);
	private:
		AVT::VmbAPI::VimbaSystem &sys;
		AVT::VmbAPI::CameraPtrVector cameras;
		AVT::VmbAPI::CameraPtr camera;
		AVT::VmbAPI::FeaturePtr feature;
		bool cameraConnected;

		double exposure;
		double framerate;
};