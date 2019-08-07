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

		void addContextCamera(ContextCamera &camera);
		void setContextCamera1(ContextCamera &camera);
		void setContextCamera2(ContextCamera &camera);
		void setIMU(IMU *imu);
	private:
		int openPXD();

		static void recordFrames(int videoPeriod);
		static void saveFrames(int count, int videoPeriod, bool secondsCount);

		static void contextOneSnapper();
		static void contextTwoSnapper();

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

		// Reference to context cameras that need to be snapped
		static std::vector<ContextCamera> contextCameras;
		static ContextCamera contextCamera_1;
		static ContextCamera contextCamera_2;

		// Semaphore information
		static HANDLE ghSemaphore;
		static HANDLE context1_semaphore;
		static HANDLE context2_semaphore;

		static std::ofstream* f_irTimestamps;
		static std::ofstream* f_context1Timestamps;
		static std::ofstream* f_context2Timestamps;
};