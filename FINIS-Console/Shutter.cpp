#include "pch.h"
#include <iostream>
#include "Shutter.h"
#include "stdafx.h"
#include "windows.h"
#include "string.h"
#include <string>
#include "stdio.h"
#include <iostream>
#include <chrono>


Shutter::Shutter() {
	portNumber = 7;
	maxPortNumber = 30;
}

Shutter::~Shutter() {
}

void Shutter::parseCommand(std::string command) {
	// Take a command from a script file, and do the action
	// Valid commands are
	// shutter open
	// shutter close

	char* c_line = new char[command.size() + 1];
	std::copy(command.begin(), command.end(), c_line);
	c_line[command.size()] = '\0';

	int offset = 0;
	char * word;
	word = strtok(c_line, " ");
	
	// Get word of command
	word = strtok(NULL, " ");
	if (strcmp(word, "open") == 0) {
		openShutter();
	} else {
		closeShutter();
	}
}

int Shutter::openShutter() {
	HANDLE hComPort;
	char cmdBuffer[32];
	char responseBuffer[32];
	DWORD numBytesWritten;
	DWORD numBytesRead;

	printf("Starting open\n");

	/*
	Open a handle to the COM port. We need the handle to send commands and
	receive results.
	*/
	wchar_t PortName[14];
	wchar_t i = 7;

	wsprintf(PortName, L"\\\\.\\COM%d", 8);
	hComPort = CreateFile(PortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);

	do
	{
		CloseHandle(hComPort);
		printf("Attempting %d\n", (char)i);
		if (i == 30) {
			printf("Relay board not connected\n");
			return -1;
		}
		wsprintf(PortName, L"\\\\.\\COM%d", i);
		i++;
		hComPort = CreateFile(PortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
	} while (hComPort == INVALID_HANDLE_VALUE);
	//printf("%d", i - 1);

	cmdBuffer[0] = 0x0D;

	if (!WriteFile(hComPort, cmdBuffer, 1, &numBytesWritten, NULL))
	{
		CloseHandle(hComPort);
		printf("Error: Unable to write to the specified port\n");
		getchar();
		return 1;
	}
	printf("A\n");

	/* Copy the command to the command buffer */
	strcpy(cmdBuffer, "relay on 0");

	/* Append 0x0D to emulate ENTER key */
	cmdBuffer[10] = 0x0D;

	/* Write the command to the relay module. Total 11 bytes including 0x0D  */
	printf("B\n");
	if (!WriteFile(hComPort, cmdBuffer, 11, &numBytesWritten, NULL))
	{
		CloseHandle(hComPort);
		printf("Error: Unable to write to the specified port\n");
		getchar();
		return 1;
	}
	printf("C\n");
	Sleep(200);

	cmdBuffer[0] = 0x0D;

	if (!WriteFile(hComPort, cmdBuffer, 1, &numBytesWritten, NULL))
	{
		CloseHandle(hComPort);
		printf("Error: Unable to write to the specified port\n");
		getchar();
		return 1;
	}
	printf("D\n");

	/* Copy the command to the command buffer */
	strcpy(cmdBuffer, "relay off 0");

	/* Append 0x0D to emulate ENTER key */
	cmdBuffer[11] = 0x0D;

	/* Write the command to the relay module. Total 11 bytes including 0x0D  */
	printf("E\n");
	if (!WriteFile(hComPort, cmdBuffer, 12, &numBytesWritten, NULL))
	{
		CloseHandle(hComPort);
		printf("Error: Unable to write to the specified port\n");
		getchar();
		return 1;
	}

	printf("F\n");
	if (hComPort == INVALID_HANDLE_VALUE)
	{
		printf("Error: Unable to open the specified port\n");
		getchar();
		return 1;
	}
	printf("G\n");
	CloseHandle(hComPort);

	return 0;
}

int Shutter::closeShutter() {
	HANDLE hComPort;
	char cmdBuffer[32];
	DWORD numBytesWritten;


	/*
	Open a handle to the COM port. We need the handle to send commands and
	receive results.
	*/

	wchar_t PortName[14];
	wchar_t i = 7;

	do
	{
		if (i == 30) {
			printf("Relay board not connected\n");
			return -1;
		}
		wsprintf(PortName, L"\\\\.\\COM%d", i);
		i++;
		hComPort = CreateFile(PortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
	} while (hComPort == INVALID_HANDLE_VALUE);
	//printf("%d", i - 1);


	cmdBuffer[0] = 0x0D;

	if (!WriteFile(hComPort, cmdBuffer, 1, &numBytesWritten, NULL))
	{
		CloseHandle(hComPort);
		printf("Error: Unable to write to the specified port\n");
		getchar();
		return 1;
	}

	/* Copy the command to the command buffer */
	strcpy(cmdBuffer, "relay on 1");

	/* Append 0x0D to emulate ENTER key */
	cmdBuffer[10] = 0x0D;

	/* Write the command to the relay module. Total 11 bytes including 0x0D  */

	if (!WriteFile(hComPort, cmdBuffer, 11, &numBytesWritten, NULL))
	{
		CloseHandle(hComPort);
		printf("Error: Unable to write to the specified port\n");
		getchar();
		return 1;
	}

	Sleep(200);


	cmdBuffer[0] = 0x0D;

	if (!WriteFile(hComPort, cmdBuffer, 1, &numBytesWritten, NULL))
	{
		CloseHandle(hComPort);
		printf("Error: Unable to write to the specified port\n");
		getchar();
		return 1;
	}

	/* Copy the command to the command buffer */
	strcpy(cmdBuffer, "relay off 1");

	/* Append 0x0D to emulate ENTER key */
	cmdBuffer[11] = 0x0D;

	/* Write the command to the relay module. Total 11 bytes including 0x0D  */

	if (!WriteFile(hComPort, cmdBuffer, 12, &numBytesWritten, NULL))
	{
		CloseHandle(hComPort);
		printf("Error: Unable to write to the specified port\n");
		getchar();
		return 1;
	}

	CloseHandle(hComPort);

	return 0;
}