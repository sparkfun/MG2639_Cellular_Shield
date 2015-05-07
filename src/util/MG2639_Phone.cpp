/******************************************************************************
MG2639_Phone.cpp
MG2639 Cellular Shield Library - Phone Functionality Source
Jim Lindblom @ SparkFun Electronics
Original Creation Date: April 3, 2015
https://github.com/sparkfun/MG2639_Cellular_Shield

This library within SFE_MG2639_CellShield defines phone-call functions of
the MG2639. MG2639_Phone a friend class of MG2639_Cell is defined, with member
functions like dial(), hangUp(), answer(), and callerID().

Development environment specifics:
	IDE: Arduino 1.6.3
	Hardware Platform: Arduino Uno
	MG2639 Cellular Shield Version: 1.0

This code is beerware; if you see me (or any other SparkFun employee) at the
local, and you've found our code helpful, please buy us a round!

Distributed as-is; no warranty is given.
******************************************************************************/

#include "MG2639_Phone.h"
#include "MG2639_AT.h"
#include <SFE_MG2639_CellShield.h>

#define CELL_RING	A0	// Cellular module's RING output goes to Arduino's A0
#define CELL_RING_THRESHOLD 100

MG2639_Phone::MG2639_Phone()
{
	pinMode(CELL_RING, INPUT); // Set CELL_RING pin as an input
}

// Check if a call is coming in.
// Couple ways to do this: check if the RING pin is low (0V).
// or check if "RING" was received on serial port.
bool MG2639_Phone::available()
{
	// The CELL_RING pin will pull low if a call is coming in.
	// Otherwise it'll idle at around 2.8V.
	// Use an analog read because the idle voltage is in a grey logic area.
	int ringer = analogRead(CELL_RING);
	if (ringer < CELL_RING_THRESHOLD)
		return true;
	else
		return false;
}

int8_t MG2639_Phone::status()
{
	int8_t iRetVal;
	// AT+CLCC
	// If it just returns OK, no calls active. Otherwise, response is:
	// +CLCC: <caller id>,<direction>,<status>,<mode>,<multiparty>,<number>,<type>
	// Originated Dialing e.g.:	+CLCC: 1,0,2,0,0,"12345678901",129\r\n\r\nOK\r\n\r\n
	// Originated Ringing e.g.:	+CLCC: 1,0,3,0,0,"12345678901",129\r\n\r\nOK\r\n\r\n
	// Incoming:				+CLCC: 1,1,4,0,0,"2345678901",129\r\n\r\nOK\r\n\r\n
	// Active e.g.:				+CLCC: 1,0,0,0,0,"12345678901",129\r\n\r\nOK\r\n\r\n
	cell.sendATCommand(CHECK_STATUS);
	// Check for response "OK" is a "fail" -- there is no active call, incoming our outgoing.
	// Good response will start with "+CLCC"
	iRetVal = cell.readWaitForResponses(CHECK_STATUS, RESPONSE_OK, COMMAND_RESPONSE_TIME);
	if (iRetVal <= 0)
	{
		return iRetVal;
	}
	// Now wait for an OK, following the +CLCC info string
	iRetVal = cell.readWaitForResponse(RESPONSE_OK, COMMAND_RESPONSE_TIME);
	if (iRetVal <= 0)
	{	// This may return a timeout error:
		return iRetVal;
	}
	
	char * ptr;
	char * start;
	char * end;
	// Find the first instance of the comma:
	ptr = strchr((const char *)cell.rxBuffer, ',');
	if (ptr == NULL)
		return ERROR_FAIL_RESPONSE;
	start = strchr(ptr + 1, ',');
	if (start == NULL)
		return ERROR_FAIL_RESPONSE;
	end = strchr(start + 1, ',');
	if (end == NULL)
		return ERROR_FAIL_RESPONSE;
	
	return start[1] - '0';
}

int8_t MG2639_Phone::callerID(char * phoneNumber)
{
	int8_t iRetVal = status();
	if (iRetVal >= 0)
	{
		// If status() is active call, incoming, or outgoing, the response will
		// look like: "+CLCC: 1,0,2,0,0,"12345678901",129\r\n\r\nOK\r\n\r\n"
		char * ptr;
		char * start;
		char * end;
		// Find the first instance of the ":
		start = strchr((const char *)cell.rxBuffer, '\"');
		if (start == NULL)
			return ERROR_FAIL_RESPONSE;
		end = strchr(start + 1, '\"');
		if (end == NULL)
			return ERROR_FAIL_RESPONSE;
		
		strncpy(phoneNumber, start + 1, (end - start - 1));
		
		return SUCCESS_OK;
		
	}
	else
	{
		return iRetVal;
	}
}

int8_t MG2639_Phone::answer()
{
	int8_t iRetVal;
	
	cell.sendATCommand(ANSWER); // Send "ATA"
	
	// "OK" response on successful pick up.
	iRetVal = cell.readWaitForResponse(RESPONSE_OK, COMMAND_RESPONSE_TIME);
	return iRetVal;
}

int8_t MG2639_Phone::hangUp()
{
	int8_t iRetVal;
	
	cell.sendATCommand(HANG_UP); // Send "ATH"
	
	// "OK" response on successful hang up.
	iRetVal = cell.readWaitForResponse(RESPONSE_OK, COMMAND_RESPONSE_TIME);
	return iRetVal;
}


int8_t MG2639_Phone::dial(char * phoneNumber)
{	
	int8_t iRetVal;
	char temp[20];
	memset(temp, 0, 20);
	
	// Send something like: "ATD13024540756;"
	sprintf(temp, "%s%s;", DIAL, phoneNumber);
	cell.sendATCommand(temp);
	
	// Successful response is "OK"
	iRetVal = cell.readWaitForResponse(RESPONSE_OK, COMMAND_RESPONSE_TIME);
	return iRetVal;
}

int8_t MG2639_Phone::dialLast()
{	
	int8_t iRetVal;
	cell.sendATCommand(DIAL_LAST); // Send "ATDL"
	iRetVal = cell.readWaitForResponse(RESPONSE_OK, COMMAND_RESPONSE_TIME);
	return iRetVal;
}

int8_t MG2639_Phone::setAudioChannel(audio_channel channel)
{
	int8_t iRetVal;
	char temp[10];
	memset(temp, 0, 10);
	
	// Send a commmand like: "AT+SPEAKER=0"
	sprintf(temp, "%s=%d", SPEAKER_SELECT, channel);
	cell.sendATCommand(temp);
	iRetVal = cell.readWaitForResponse(RESPONSE_OK, COMMAND_RESPONSE_TIME);
	return iRetVal;
}

MG2639_Phone phone;