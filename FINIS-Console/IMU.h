#ifndef IMU_H
#define IMU_H

// Include these header file to get access to VectorNav sensors.
#include "vn/sensors.h"
#include "vn/thread.h"

#include <string>

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
	
	void getAsynchData();


private:
	std::string m_sensorPort;
	uint32_t m_sensorBaudeRate;
	VnSensor vs;
	BinaryOutputRegister m_bor;
	bool m_print;
};

#endif



