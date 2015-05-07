/******************************************************************************
MG2639_GPRS.cpp
MG2639 Cellular Shield Library - GPRS Functionality Source
Jim Lindblom @ SparkFun Electronics
Original Creation Date: April 3, 2015
https://github.com/sparkfun/MG2639_Cellular_Shield

This library within SFE_MG2639_CellShield defines GPRS functions of
the MG2639. MG2639_GPRS, a friend class of MG2639_Cell, is defined with member
functions like connect(), status(), read(), write() and print().

Development environment specifics:
	IDE: Arduino 1.6.3
	Hardware Platform: Arduino Uno
	MG2639 Cellular Shield Version: 1.0

This code is beerware; if you see me (or any other SparkFun employee) at the
local, and you've found our code helpful, please buy us a round!

Distributed as-is; no warranty is given.
******************************************************************************/

#include "MG2639_GPRS.h"
#include "MG2639_AT.h"
#include <SFE_MG2639_CellShield.h>

#define WEB_RESPONSE_TIMEOUT	30000	// 30 second timeout on web response
#define IP_ADDRESS_LENGTH 15
#define MAX_DOMAIN_LENGTH 269
const char ipCharSet[] = "0123456789.";

MG2639_GPRS::MG2639_GPRS()
{
	_activeChannel = -1;
}

int MG2639_GPRS::open() // AT+ZPPPOPEN 
{
	int iRetVal;
	cell.sendATCommand(OPEN_GPRS);
	// Should respond "+ZPPPOPEN:CONNECTED\r\n\r\nOK\r\n\r\n" or
	//				  "+ZPPPOPEN:ESTABLISHED\r\n\r\nOK\r\n\r\n"
	// Bad response is "+ZPPPOPEN:FAIL\r\n\r\nERROR\r\n"
	// bad response can take ~20 seconds to occur
	iRetVal = cell.readWaitForResponses(RESPONSE_OK, RESPONSE_ERROR, WEB_RESPONSE_TIMEOUT);
	
	return iRetVal;
}

int MG2639_GPRS::close() //AT+ZPPPCLOSE
{
	int iRetVal;
	cell.sendATCommand(CLOSE_GPRS);
	// Should respond "+ZPPCLOSE:OK\r\n\r\nOK\r\n\r\n"
	iRetVal = cell.readWaitForResponses(RESPONSE_OK, RESPONSE_ERROR, WEB_RESPONSE_TIMEOUT);
	
	return iRetVal;
}

IPAddress MG2639_GPRS::localIP() // AT+ZIPGETIP
{
	int iRetVal;
	cell.sendATCommand(GET_IP);
	iRetVal = cell.readWaitForResponse(RESPONSE_OK, WEB_RESPONSE_TIMEOUT);
	if (iRetVal < 0)
	{
		return iRetVal;
	}
	
	// Response looks like: +ZIPGETIP:nnn.nnn.nnn.nnn\r\n\r\nOK\r\n\r\n
	// We need to copy the middle, IP address portion of that to ipRet
	// Find the first occurence of numbers or .'s:
	char * start;
	int len = 0;
	char tempIP[IP_ADDRESS_LENGTH];
	memset(tempIP, 0, IP_ADDRESS_LENGTH);
	start = strpbrk((const char *)cell.rxBuffer, ipCharSet);
	len = strspn(start, ipCharSet);
	// Copy the string 
	if ((len > 0) && (len <= IP_ADDRESS_LENGTH))
		strncpy(tempIP, start, len);
	
	// Little extra work to convert the "nnn.nnn.nnn.nnn" string to four
	// octet values required for the IPAdress type.
	IPAddress * ipRet;
	charToIPAddress(tempIP, *ipRet);
	return *ipRet;
}

int MG2639_GPRS::hostByName(const char * domain, IPAddress * ipRet) // AT+ZDNSGETIP
{
	int iRetVal;
	char dnsCommand[MAX_DOMAIN_LENGTH];
	memset(dnsCommand, '\0', 269);
	sprintf(dnsCommand, "%s=\"%s\"", DNS_GET_IP, domain);
	cell.sendATCommand((const char *)dnsCommand);
	iRetVal = cell.readWaitForResponse(RESPONSE_OK, WEB_RESPONSE_TIMEOUT);
	if (iRetVal < 0)
	{
		return iRetVal;
	}
	
	// Response looks like +ZDNSGETIP:nnn.nnn.nnn.nnn\r\n\r\nOK\r\n\r\n"
	// We need to copy the middle, IP address portion of that to ipRet
	// Find the first occurence of numbers or .'s:	
	char * start;
	int len = 0;	
	char tempIP[IP_ADDRESS_LENGTH];
	start = strpbrk((const char *)cell.rxBuffer, ipCharSet);
	// !!! TO DO Check if we're at the edge of the ring buffer
	// Find the length of that string:
	len = strspn(start, ipCharSet);
	// Copy the string 
	if ((len > 0) && (len <= IP_ADDRESS_LENGTH))
		strncpy(tempIP, start, len);	
	
	// Little extra work to convert the "nnn.nnn.nnn.nnn" string to four
	// octet values required for the IPAdress type.
	charToIPAddress(tempIP, *ipRet);
	
	return iRetVal;
}


int MG2639_GPRS::connect(const char * domain, unsigned int port, uint8_t channel)
{
	IPAddress destIP;
	hostByName(domain, &destIP);
	connect(destIP, port, channel);
}

int MG2639_GPRS::connect(IPAddress ip, unsigned int port, uint8_t channel)
{
	int iRetVal;
	// Maximum is 15 for IP + 5 for port + 1 for channel + 12 for cmd, = and ,'s
	char ipSetupCmd[33];
	memset(ipSetupCmd, '\0', 33);
	sprintf(ipSetupCmd, "%s=%d,%d.%d.%d.%d,%d", TCP_SETUP, channel, ip[0], ip[1], ip[2], ip[3], port);
	//sprintf(ipSetupCmd, "%s=%i,%s,%i", TCP_SETUP, channel, ip, port);
	cell.sendATCommand((const char *)ipSetupCmd);
	
	iRetVal = cell.readWaitForResponse(RESPONSE_OK, WEB_RESPONSE_TIMEOUT);
	if (iRetVal < 0)	// If nothing was received return timeout error
	{
		return iRetVal;
	}
	
	_activeChannel = channel;
	
	return iRetVal;	
}

int8_t MG2639_GPRS::status()
{
	int iRetVal;
	cell.sendATCommand(TCP_STATUS);
	iRetVal = cell.readWaitForResponses("ESTABLISHED", "DISCONNECTED", WEB_RESPONSE_TIMEOUT);
	
	if (iRetVal > 0)
		return GPRS_ESTABLISHED;
	else if (iRetVal == ERROR_FAIL_RESPONSE)
		return GPRS_DISCONNECTED;
	
	return iRetVal;	
}

int MG2639_GPRS::available()
{
	// Should check if we're connected & within a +ZIPRECV
	return cell.dataAvailable();
}

int MG2639_GPRS::read()
{
	if (!available())
		return -1;
	return cell.uartRead();
}

int MG2639_GPRS::peek()
{
}

void MG2639_GPRS::flush()
{
}

size_t MG2639_GPRS::write(uint8_t b)
{
	return write(&b, 1);
}

size_t MG2639_GPRS::write(const uint8_t *buf, size_t size)
{
	int iRetVal;
	// Maximum is 10 for command ',' and '=', 5 for port, 4 for length
	char sendCmd[19];
	memset(sendCmd, '\0', 19);
	sprintf(sendCmd, "%s=%i,%i", TCP_SEND, _activeChannel, size);
	cell.sendATCommand((const char *)sendCmd);
	iRetVal = cell.readWaitForResponse(">", WEB_RESPONSE_TIMEOUT);
	if (iRetVal <= 0)
		return -1;
	
	cell.clearSerial();		// Clear out the serial rx buffer
	cell.printString((const char *)buf);	// Send the data string to cell module
	iRetVal = cell.readWaitForResponse("+ZIPSEND: OK", WEB_RESPONSE_TIMEOUT);
	if (iRetVal <= 0)
		return -1;
	
	return size;
}

bool MG2639_GPRS::charToIPAddress(char * ipChar, IPAddress & ipRet)
{
	int a, b, c, d;
	int scan = sscanf(ipChar, "%d.%d.%d.%d", &a, &b, &c, &d);
	if (scan == 4)
	{
		ipRet = {a, b, c, d};
		return 1;
	}
	
	return 0;
}

/*
	//! Clean the rest of this function up 
	uint8_t connection = 1;
	int httpStatus = 0;
	unsigned long timeIn = millis();
	int received = 0;
	
	cell.clearBuffer();
	while (connection && (timeIn + WEB_RESPONSE_TIMEOUT > millis()))
	{
		if (cell.dataAvailable())
		{
			// !!! All of these strstr's (in this library in general) need to be
			// replaced with a function that can search from the end of the 
			// buffer back to the beginning.
			// Fix the searchBuffer function below!
			received += cell.readByteToBuffer();
			// Check for the HTTP Status Response:
			if ((httpStatus == 0) && (strstr((const char *)cell.rxBuffer, "200 OK")))
			{
				Serial.println("Received 200 OK");
				httpStatus = 1;	// Received 200 OK
			}
			else if ((httpStatus == 0) && (strstr((const char *)cell.rxBuffer, "400 Bad Request")))
			{
				Serial.println("Received 400 Bad Request");
				httpStatus = -1;	// Received 400 Bad Request
			}
			if (strstr((const char *)cell.rxBuffer, "+ZIPCLOSE"))
			{
				Serial.println("Closing the connection");
				connection = 0;
			}
		}
	}
	
	return received;
*/

MG2639_GPRS gprs;