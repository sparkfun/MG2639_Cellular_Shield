/******************************************************************************
MG2639_SMS.cpp
MG2639 Cellular Shield Library - SMS Functionality Source
Jim Lindblom @ SparkFun Electronics
Original Creation Date: April 3, 2015
https://github.com/sparkfun/MG2639_Cellular_Shield

This library within SFE_MG2639_CellShield defines SMS functions of
the MG2639. MG2639_SMS, a friend class of MG2639_Cell, is defined with member
functions like read(), send(), deleteMessage(), and setMode().

Development environment specifics:
	IDE: Arduino 1.6.3
	Hardware Platform: Arduino Uno
	MG2639 Cellular Shield Version: 1.0

This code is beerware; if you see me (or any other SparkFun employee) at the
local, and you've found our code helpful, please buy us a round!

Distributed as-is; no warranty is given.
******************************************************************************/

#include "MG2639_SMS.h"
#include "MG2639_AT.h"
#include <SFE_MG2639_CellShield.h>
#include <Arduino.h>

// Message sends are completed by sending a CTRL+Z (0x1A) byte
#define CTRL_Z 0x1A
// Maximum time it should take an SMS command to complete
#define SMS_COMMAND_TIMEOUT 10000 
#define CHAR_RECV_TIME 5 //! TODO: Should be dependent on baud

MG2639_SMS::MG2639_SMS()
{
	memset(_msgIndex, 0, MESSAGE_INDEX_MAX);
	memset(_destPhone, 0, MAX_PHONE_NUMBER_SIZE);
	messageOverrun = false;
}

int8_t MG2639_SMS::setMode(sms_mode mode)
{
	int8_t iRetVal;
	char tempCmd[10];
	
	memset(tempCmd, 0, 10);
	
	sprintf(tempCmd, "%s=%d", SMS_MODE, mode);
	cell.sendATCommand((const char *)tempCmd);	
	
	iRetVal = cell.readWaitForResponse(RESPONSE_OK, COMMAND_RESPONSE_TIME);
	return iRetVal;
}

int8_t MG2639_SMS::start(const char * phoneNumber)
{
	// Send message: AT+CMGS="13316538879"<CR>MESSAGE_GOES_HERE<CTRL+Z>OK
	// Max phone number digits is 15, add 2 for quote, 5 for command, 1 for equal
	char tempCmd[24];
	sprintf(tempCmd, "%s=\"%s\"", SMS_SEND, phoneNumber);
	cell.sendATCommand((const char *)tempCmd);
}

int8_t MG2639_SMS::send()
{
	int iRetVal;
	cell.printChar(CTRL_Z);
	
	iRetVal = cell.readWaitForResponse(RESPONSE_OK, SMS_COMMAND_TIMEOUT);
	return iRetVal;
}

// Get index of available READ, UNREAD, or ALL messages
// Returns index of first applicable message
int MG2639_SMS::available(sms_status status)
{
	int iRetVal;
	char tempCmd[21];
	char tempIndex[10];
	uint8_t index = 0;
	char c = 0;
	int response = 1;
	int msgIndex = 0;
	
	memset(tempCmd, 0, 21);
	memset(tempIndex, 0, 10);
	
	switch (status)
	{
	case REC_UNREAD:
		sprintf(tempCmd, "%s=\"REC UNREAD\"", SMS_LIST);
		break;
	case REC_READ:
		sprintf(tempCmd, "%s=\"REC READ\"", SMS_LIST);
		break;
	case REC_ALL:
		sprintf(tempCmd, "%s=\"ALL\"", SMS_LIST);
		break;
	default:
		return -1;
	}
	cell.clearBuffer();
	cell.sendATCommand((const char *)tempCmd);
	while (response > 0)
	{
		response = cell.readWaitForResponses("+CMGL: ", RESPONSE_OK, COMMAND_RESPONSE_TIME);
		if (response > 0)
		{	// Else if we got a "+CMGL: ", get the message number.
			delay(CHAR_RECV_TIME);
			while (c != ',')
			{	//! Put a timeout in here
				c = cell.uartRead();
				if ((c >= '0') && (c <= '9'))
					tempIndex[index++] = c;
			}
			msgIndex = atoi(tempIndex);
			_msgIndex[msgIndex>>3] |= 1<<(msgIndex % 8);
		}	
	}
	if (response == ERROR_FAIL_RESPONSE)
	{	// If we read "OK" (not necessarily a fail, but that's what'll be returned)
		// Return the first available UNREAD message
		for (int i=0; i<(MESSAGE_INDEX_MAX<<3); i++)
		{
			if (_msgIndex[i>>3]&(1<<(i%8)))
				return i;
		}
		return 0;
	}
	else
	{
		return ERROR_TIMEOUT;
	}
}

size_t MG2639_SMS::write(uint8_t b)
{
	return write(&b, 1);
}

size_t MG2639_SMS::write(uint8_t *buf, size_t size)
{
	cell.printString((char *)buf, size);
}

int MG2639_SMS::pollAvailable()
{
	char temp[10]; // Should be more than enough. Could maybe drop this to 5.
	int msgIndex;
	uint8_t index = 0;
	char c = 0;
	bool found = false;
	
	memset(temp, 0, 10);
	
	// When SMS comes in, UART interrupts with: '+CMTI: "SM", <msg id>\r\n'
	// SoftwareSerial doesn't have on-receive interrupt hooks, so this
	// won't be 100% functional.
	cell.clearBuffer();
	while (cell.dataAvailable())
	{
		delay(CHAR_RECV_TIME); // Delay long enough to receive another character ~4-5ms @ 2400bps
		cell.readByteToBuffer();
		if ((cell.searchBuffer("+CMTI: \"SM\", ")) > 0)
		{
			found = true;
			break;
		}
	}
	// Have to clear buffer, otherwise this will continue to return true.
	cell.clearBuffer();
	if (found)
	{
		while ((c != '\r') && (cell.dataAvailable()) && (index < 10))
		{
			c = cell.uartRead();
			temp[index++] = c;
		}
		msgIndex = atoi(temp);
		_msgIndex[msgIndex>>3] |= 1<<(msgIndex % 8);
		//_smsStatus |= (1<<msgIndex);
		
		return msgIndex;
	}
	else
	{
		//for (int i=0; i<64; i++)
		for (int i=0; i<(MESSAGE_INDEX_MAX<<3); i++)
		{
			//if (_smsStatus & (1<<i))
			if (_msgIndex[i>>3]&(1<<(i%8)))
				return i;
		}
		return 0;
	}
}

int8_t MG2639_SMS::read(uint8_t msgIndex)
{
	int8_t iRetVal;
	char tempCmd[10];
	char c = 0;
	int i =0;
	memset(tempCmd, 0, 10);
	sprintf(tempCmd, "%s=%d", SMS_READ, msgIndex);
	cell.clearBuffer();
	cell.sendATCommand((const char *)tempCmd);
	
	// Example response: 
	// +CMGR: "REC READ","1xxxnnnzzzz","","2014/10/12 21:54:25-24"\r\n
    // Hey hey hey\r\n\r\n
	//
	// OK

	// Look for "," which comes right before the phone number
	iRetVal = cell.readWaitForResponses(",", RESPONSE_ERROR, COMMAND_RESPONSE_TIME);
	
	if (iRetVal > 0)
	{
		memset(_lastNumber, 0, MAX_PHONE_NUMBER_SIZE);
		memset(_lastDate, 0, MAX_DATE_SIZE);
		memset(_lastSMSData, 0, SMS_DATA_SIZE);
		
		cell.readBetween('\"', '\"', _lastNumber, 1000);
		while (cell.dataAvailable() < 4) 
			;
		for (int i=0; i<4; i++) // Read 4 characters ,"", (static)
			cell.uartRead();
		cell.readBetween('\"', '\"', _lastDate, 1000);
		while (cell.dataAvailable() < 2) 
			;
		for (int i=0; i<2; i++)
			cell.uartRead(); // Read the "\r\n"
		if (cell.readUntil(_lastSMSData, '\r', SMS_DATA_SIZE - 1, 1000) == ERROR_OVERRUN_PREVENT)
			messageOverrun = true;
		else
			messageOverrun = false;
	}
	else
	{
		return iRetVal;
	}
	
	_msgIndex[msgIndex>>3] &= ~(1<<(msgIndex%8));
	//_smsStatus &= ~(1<<msgIndex);
		
	return 1;
}

char * MG2639_SMS::getSender()
{
	return _lastNumber;
}

char * MG2639_SMS::getDate()
{
	return _lastDate;
}

char * MG2639_SMS::getMessage()
{
	return _lastSMSData;
}

int8_t MG2639_SMS::deleteMessage(uint8_t msgIndex)
{
	char tempCmd[11];
	int8_t iRetVal;
	
	sprintf(tempCmd, "%s=%d", SMS_DELETE, msgIndex);
	cell.sendATCommand((const char *)tempCmd);
	
	iRetVal = cell.readWaitForResponse(RESPONSE_OK, COMMAND_RESPONSE_TIME);
	return iRetVal;
}

MG2639_SMS sms;