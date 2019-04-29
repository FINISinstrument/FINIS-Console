#pragma once

#include <string>
#include <ctime> // Used for getting dateTime
#include "XCLIB/xcliball.h"

class PXD {
	public:
		PXD(std::string saveLocation);
		PXD(std::string saveLocation, bool isThirtyFPS);

		~PXD();

		int snap(std::string imageName);
		int video(int frameCount);
	private:
		int openPXD();

		void getDateTime();

		bool recordFrames(int frameCount);
		bool saveFrames(int frameCount);

		int enable();
		int disable();

		bool isThirtyFPS;
		bool isSixteenBit;
		bool isStreaming;
		bool isOpen;
		//std::string imageName; // argument in snap function
		std::string liveName;
		std::string dateTime;
		//int frameCount;
		int frameCountRemainder;
		int imagesCaptured;

		int halfBufferSize;
		int loopCount;
		int folderIndex;
		std::string folderPath;

		std::string baseImagePath = "C:/FINIS/testing/Image/";
		std::string baseVideoPath = "C:/FINIS/testing/Video/";
};