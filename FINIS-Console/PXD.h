#pragma once

#include <string>
#include <ctime> // Used for getting dateTime
#include <future>
#include <thread>
#include <atomic>
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

		static void recordFrames(int videoPeriod);
		static void saveFrames(int count, int videoPeriod, bool secondsCount);

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

		static int halfBufferSize;
		int loopCount;
		static std::string folderPath;

		static std::atomic<bool> finishedWithVideo;

		// Semaphore information
		static HANDLE ghSemaphore;

		std::string baseImagePath = "C:/FINIS/testing/Image/";
		std::string baseVideoPath = "C:/FINIS/testing/Video/";

		// Frame buffer information
		static uint32_t* frameTimestamps;

		static bool firstHandleRun;
};