#pragma once

// Class to control the Shutter
#include <stdio.h>
#include <tchar.h>
#include "windows.h"
#include "string.h"

class Shutter {
	public:
		Shutter();
		~Shutter();

		void parseCommand(char* command);

		void openShutter();
		void closeShutter();

		bool isOpen();

	private:
		bool portConnected;
		bool shutterOpen;
	
		bool connectPort();
		void disconnectPort();

		HANDLE hComPort;
		char cmdBuffer[32];
		char responseBuffer[32];
		DWORD numBytesWritten;
		DWORD numBytesRead;

		int portNumber;
		int maxPortNumber;
};
