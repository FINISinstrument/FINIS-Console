#include "pch.h"
#include <iostream>
#include "Shutter.h"

Shutter::Shutter() {
	portNumber = 7;
	maxPortNumber = 12;

	portConnected = false;
	shutterOpen = false;
}

Shutter::~Shutter() {
	disconnectPort();
}

void Shutter::openShutter() {
	// If shutter open, don't open again
	if (shutterOpen) {
		return;
	}
	// If port not connected, attempt to connect
	// If we can't open the port, fail
	if (!connectPort()) {
		return;
	}

	// Attempt to write to buffer
	cmdBuffer[0] = 0x0D;
	if (!WriteFile(hComPort, cmdBuffer, 1, &numBytesWritten, NULL)) {
		disconnectPort();
		return;
	}

	// Copy command into buffer
	strcpy_s(cmdBuffer, "relay on 0");
	// Append 0x0D to emulate ENTER key
	cmdBuffer[10] = 0x0D;

	WriteFile(hComPort, cmdBuffer, 11, &numBytesWritten, NULL);

	Sleep(200);

	cmdBuffer[0] = 0x0D;
	WriteFile(hComPort, cmdBuffer, 1, &numBytesWritten, NULL);

	// Copy command into buffer
	strcpy_s(cmdBuffer, "relay off 0");
	// Append 0x0D to emulate ENTER key
	cmdBuffer[11] = 0x0D;

	WriteFile(hComPort, cmdBuffer, 12, &numBytesWritten, NULL);

	shutterOpen = true;
}

void Shutter::closeShutter() {
	// If shutter is closed, don't close again
	if (!shutterOpen) {
		return;
	}
	// If port not connected, attempt to connect
	// If we can't open the port, fail
	if (!connectPort()) {
		return;
	}

	// Attempt to write to buffer
	cmdBuffer[0] = 0x0D;
	if (!WriteFile(hComPort, cmdBuffer, 1, &numBytesWritten, NULL)) {
		disconnectPort();
		return;
	}

	// Copy command into buffer
	strcpy_s(cmdBuffer, "relay on 1");
	// Append 0x0D to emulate ENTER key
	cmdBuffer[10] = 0x0D;

	WriteFile(hComPort, cmdBuffer, 11, &numBytesWritten, NULL);

	Sleep(200);

	cmdBuffer[0] = 0x0D;
	WriteFile(hComPort, cmdBuffer, 1, &numBytesWritten, NULL);

	// Copy command into buffer
	strcpy_s(cmdBuffer, "relay off 1");
	// Append 0x0D to emulate ENTER key
	cmdBuffer[11] = 0x0D;

	WriteFile(hComPort, cmdBuffer, 12, &numBytesWritten, NULL);

	shutterOpen = false;
}

bool Shutter::isOpen() {
	return shutterOpen;
}

bool Shutter::connectPort() {
	// Check if already open
	if (portConnected) {
		return portConnected;
	}
	// Open handle to COM port.
	// Handle is needed to send commands and receive results
	wchar_t PortName[14];
	do {
		wsprintf(PortName, L"\\\\.\\COM%d", portNumber);
		hComPort = CreateFile(PortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
		portNumber++;
	} while (hComPort == INVALID_HANDLE_VALUE && portNumber < maxPortNumber);

	// If unable to open any handles, error
	if (hComPort == INVALID_HANDLE_VALUE) {
		std::cout << "Error: Unable to open port to shutter\n";
		portConnected = false;
	}
	else {
		// If no error, port is open
		portConnected = true;
	}

	return portConnected;
}

void Shutter::disconnectPort() {
	// Close handle to COM port if it is open
	if (!portConnected) {
		return;
	}
	// CloseHandle returns a nonzero value if successful
	portConnected = !CloseHandle(hComPort);
}
