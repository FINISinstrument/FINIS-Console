/*
Created by Jason Powell on 18 April 2019
Designed to connect IMU and to set up functions to pull information off of the IMU
*/

#include <iostream>
#include "IMU.h"


std::ofstream* IMU::m_file = NULL;

//constructor
IMU::IMU(std::string sensorPort, uint32_t sensorBaudeRate, bool print) {

  // BEGIN Leftover from merge
	m_print = print;
  // END

	m_sensorPort = sensorPort;
	m_sensorBaudeRate = sensorBaudeRate;
	m_bor = BinaryOutputRegister( 
		ASYNCMODE_PORT1,
		200,
		COMMONGROUP_TIMESTARTUP | COMMONGROUP_YAWPITCHROLL | COMMONGROUP_VELOCITY,	// Note use of binary OR to configure flags.
		TIMEGROUP_NONE,
		IMUGROUP_NONE,
		GPSGROUP_POSLLA,
		ATTITUDEGROUP_NONE,
		INSGROUP_NONE);
	m_print = print;
	m_filePath = "C:/FINIS/testing/IMU";
}
IMU::~IMU() {
	if (m_vs.isConnected()) {
		m_vs.disconnect();
	}
}

bool IMU::ConnectIMU() { //returns true if the sensor succesfully connects
	try {
		m_vs.connect(m_sensorPort, m_sensorBaudeRate);
		m_vs.writeBinaryOutput1(m_bor);
	}
	catch (std::exception ex) {
		std::cout << "Failed to connect IMU at " << m_sensorPort << " with BaudeRate " << m_sensorBaudeRate << std::endl;
	}
	if (m_vs.isConnected()) {
		return true;
	}
	return false;
}

void IMU::debugGetAsynchData() {
	time_t t = time(NULL);
	tm* timePtr = localtime(&t);
	std::string curDate = m_filePath + std::to_string(timePtr->tm_mon) + "-" + std::to_string(timePtr->tm_mday) + "-" + std::to_string((timePtr->tm_year) + 1900) + "-" + std::to_string(timePtr->tm_hour) + "-" + std::to_string(timePtr->tm_min) + ".txt";

	//m_file->open(curDate.c_str());
	m_file = new std::ofstream(curDate.c_str());
	if (!m_file) {
		std::cout << "file not open\n";
	}
	m_vs.writeBinaryOutput1(m_bor);
	m_vs.registerAsyncPacketReceivedHandler(NULL, asciiOrBinaryAsyncMessageReceived);

	std::cout << "Starting sleep...\n";
	Thread::sleepSec(5);

	m_vs.unregisterAsyncPacketReceivedHandler();
	m_file->close();
}

void IMU::startAsynchData() {
	time_t t = time(NULL);
	tm* timePtr = localtime(&t);

	m_file = new std::ofstream((m_filePath + "/imu_data.txt").c_str());
	if (!m_file) {
		std::cout << "file not open\n";
	}
	m_vs.writeBinaryOutput1(m_bor);
	m_vs.registerAsyncPacketReceivedHandler(NULL, asciiOrBinaryAsyncMessageReceived);
}

void IMU::stopAsynchData() {
	m_vs.unregisterAsyncPacketReceivedHandler();
	m_file->close();
}

void IMU::asynchData(HANDLE semaphore) {
	// Start recording
	startAsynchData();

	// Wait for signal to stop
	WaitForSingleObject(semaphore, INFINITE);

	// Stop recording
	stopAsynchData();
}

void asciiOrBinaryAsyncMessageReceived(void* userData, Packet& p, size_t index) {
	if (p.type() == Packet::TYPE_ASCII && p.determineAsciiAsyncType() == VNYPR) {
		vec3f ypr;
		p.parseVNYPR(&ypr);
		std::cout << "ASCII Async YPR: " << ypr << std::endl;
		return;
	}

	if (p.type() == Packet::TYPE_BINARY) {
		// First make sure we have a binary packet type we expect since there
		// are many types of binary output types that can be configured.
		if (!p.isCompatible(
			COMMONGROUP_TIMESTARTUP | COMMONGROUP_YAWPITCHROLL| COMMONGROUP_VELOCITY,
			TIMEGROUP_NONE,
			IMUGROUP_NONE,
			GPSGROUP_POSLLA,
			ATTITUDEGROUP_NONE,
			INSGROUP_NONE)) {
			std::cout << "Not the type of binary packet we are expecting." << std::endl;
			return;
		}

		// Ok, we have our expected binary output packet. Since there are many
		// ways to configure the binary data output, the burden is on the user
		// to correctly parse the binary packet. However, we can make use of
		// the parsing convenience methods provided by the Packet structure.
		// When using these convenience methods, you have to extract them in
		// the order they are organized in the binary packet per the User Manual.
		uint64_t timeStartup = p.extractUint64();
		vec3f ypr = p.extractVec3f();
		vec3f vel = p.extractVec3f();
		vec3d lla = p.extractVec3d();

		//p.extract
		*(IMU::m_file) << "Binary Async TimeStartup: " << timeStartup << std::endl;
		*(IMU::m_file) << "Binary Async YPR: " << ypr << std::endl;
		*(IMU::m_file) << "VEL X" << vel.x << std::endl;
		*(IMU::m_file) << "VEL Y" << vel.y << std::endl;
		*(IMU::m_file) << "VEL Z" << vel.z << std::endl;
		*(IMU::m_file) << "LLA X" << lla.x << std::endl;
		*(IMU::m_file) << "LLA Y" << lla.y << std::endl;
		*(IMU::m_file) << "LLA Z" << lla.z << std::endl;
		
		/*
		std::cout << "Binary Async TimeStartup: " << timeStartup << std::endl;
		std::cout << "Binary Async YPR: " << ypr << std::endl;
		std::cout << "VEL X" << vel.x << std::endl;
		std::cout << "VEL Y" << vel.y << std::endl;
		std::cout << "VEL Z" << vel.z << std::endl;
		std::cout << "LLA X" << lla.x << std::endl;
		std::cout << "LLA Y" << lla.y << std::endl;
		std::cout << "LLA Z" << lla.z << std::endl;
		*/
	}
}