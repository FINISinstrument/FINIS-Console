#ifndef IMU_H
#define IMU_H

// Include these header file to get access to VectorNav sensors.
#include "vn/sensors.h"
#include "vn/thread.h"

#include <fstream>
#include <string>
#include <algorithm>
#include <ctime>

using namespace vn::math;
using namespace vn::sensors;
using namespace vn::protocol::uart;
using namespace vn::xplat;

void asciiOrBinaryAsyncMessageReceived(void* userData, Packet& p, size_t index);

class IMU {
public:
	//constructor/destructor
	IMU(std::string sensorPort, uint32_t sensorBaudeRate, bool print);
	~IMU();
	
	bool ConnectIMU();
	void debugGetAsynchData();
	void startAsynchData();
	void stopAsynchData();
	void setFilePath(std::string filePath) {
		m_filePath = filePath;
	};

private:
	std::string m_sensorPort;
	uint32_t m_sensorBaudeRate;
	VnSensor m_vs;
	BinaryOutputRegister m_bor;
	bool m_print;
	static std::ofstream* m_file;
	friend void asciiOrBinaryAsyncMessageReceived(void* userData, Packet& p, size_t index);
	std::string m_filePath;
};

#endif



