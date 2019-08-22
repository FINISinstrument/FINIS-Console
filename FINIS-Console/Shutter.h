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

		void parseCommand(std::string command);

		int openShutter();
		int closeShutter();

	private:
		int portNumber;
		int maxPortNumber;
};
