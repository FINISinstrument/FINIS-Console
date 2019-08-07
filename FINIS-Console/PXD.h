#pragma once

#include <string>
#include <fstream>
#include <ctime> // Used for getting dateTime
#include <future>
#include <thread>
#include <atomic>
#include <vector>
#include "XCLIB/xcliball.h"

#include "contextCamera.h"
#include "IMU.h"

#include "constants.h"

class PXD {
	public:
		PXD(std::string saveLocation);
		PXD(std::string saveLocation, bool isThirtyFPS);

		~PXD();

		bool parseCommand(std::string command);

		int snap(std::string imageName);
		int video(int frameCount, bool useSeconds, bool enableContext);

		void setFilePath(std::string path) {
			folderPath = path;
		};
	private:
		int openPXD();

		static void recordFrames(int videoPeriod);
		static void saveFrames(int count, int videoPeriod, bool secondsCount);

		int enable();
		int disable();

		bool isThirtyFPS;
		bool isSixteenBit;
		bool isStreaming;
		bool isOpen;
		std::string liveName;
		std::string dateTime;
		int imagesCaptured;

		static int halfBufferSize;
		int loopCount;
		static std::string folderPath;

		static std::atomic<bool> finishedWithRecording;
		static std::atomic<bool> finishedWithSaving;
		static std::atomic<bool> finishedWithContext;

		std::string baseImagePath = "C:/FINIS/testing/Image";
		std::string baseVideoPath = "C:/FINIS/testing/Video";

		// Frame buffer information
		static uint32_t* frameTimestamps;

		static bool firstHandleRun;

		// Semaphore information
		static HANDLE ghSemaphore;

		static std::ofstream* f_irTimestamps;
};