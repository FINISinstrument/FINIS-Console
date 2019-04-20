/*
Created by Jason Powell on 18 April 2019
Designed to connect IMU and to set up functions to pull information off of the IMU
*/

#include <iostream>

#include "IMU.h"


//constructor
IMU::IMU(std::string sensorPort, uint32_t sensorBaudeRate) {
	m_sensorPort = sensorPort;
	m_sensorBaudeRate = sensorBaudeRate;
	m_bor = BinaryOutputRegister( //TODO: decide what controls we want to give to the user over what is outputted
		ASYNCMODE_PORT1,
		200,
		COMMONGROUP_TIMESTARTUP | COMMONGROUP_YAWPITCHROLL,	// Note use of binary OR to configure flags.
		TIMEGROUP_NONE,
		IMUGROUP_NONE,
		GPSGROUP_POSLLA,
		ATTITUDEGROUP_NONE,
		INSGROUP_NONE);
}
IMU::~IMU() {
	if (vs.isConnected()) {
		vs.disconnect();
	}
}

bool IMU::ConnectIMU() { //returns true if the sensor succesfully connects
	vs.connect(m_sensorPort, m_sensorBaudeRate);
	if (vs.isConnected()) {
		std::cout << "IMU successfully connected on " << m_sensorPort << " With Baudrate: " << m_sensorBaudeRate << std::endl;
		return true;
	}
	std::cout << "IMU failed to connect on " << m_sensorPort << " With Baudrate: " << m_sensorBaudeRate << std::endl;
	return false;
}

void IMU::asciiOrBinaryAsyncMessageReceived(void * userData, Packet & p, size_t index) {
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
			COMMONGROUP_TIMESTARTUP | COMMONGROUP_YAWPITCHROLL,
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
		//p.extract
		std::cout << "Binary Async TimeStartup: " << timeStartup << std::endl;
		std::cout << "Binary Async YPR: " << ypr << std::endl;
	}

}

void IMU::getAsynchData(bool print) {
	vs.writeBinaryOutput1(m_bor);
	//TODO: figure out why this line doesn't work
	//vs.registerAsyncPacketReceivedHandler(NULL, asciiOrBinaryAsyncMessageReceived);

	std::cout << "Starting sleep...\n";
	Thread::sleepSec(5);

	vs.unregisterAsyncPacketReceivedHandler();
}

void IMU::getNonAsyncData(){
	GpsSolutionLlaRegister gps = vs.readGpsSolutionLla();
	vec3f ypr = vs.readYawPitchRoll();
	std::cout << "Current YPR: " << ypr << std::endl;
	std::cout << "Current GPS Time: " << gps.time << std::endl;
	std::cout << "Current GPS lla: " << gps.lla << std::endl;
	std::cout << "Current GPS fix: " << gps.gpsFix << std::endl;
	std::cout << "Current GPS nedAcc: " << gps.nedAcc << std::endl;
	std::cout << "Current GPS week: " << gps.week << std::endl;
	std::cout << "Current GPS speedAcc: " << gps.speedAcc << std::endl;
	std::cout << "Number of Satallites: " << +gps.numSats << std::endl;
}

