/******************************************************************************
MG2639_GPRS.h
MG2639 Cellular Shield Library - GPRS Functionality Header
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

#ifndef _MG2639_GPRS_H_
#define _MG2639_GPRS_H_

#include <Stream.h>
#include <IPAddress.h>

#define DEFAULT_CHANNEL 0

enum connection_status {
	GPRS_DISCONNECTED = 0,
	GPRS_ESTABLISHED
};

class MG2639_GPRS : public Stream
{
public:
	/// MG2639_GPRS() - Constructor
	/// Sets up class variables
	MG2639_GPRS();
	
	//////////////////////////////
	// GPRS Connection Commands //
	//////////////////////////////
	
	/// open() - Sends AT+ZPPPOPEN to enable GPRS
	/// Before using any TCP commands, this function must be called.
	/// This function can take a long time to complete. WEB_RESPONSE_TIMEOUT is
	/// set to 30s, which is the high end of what it might take.
	///
	/// Returns: >0 on success, <0 on fail
	int open(); 
	
	/// close() - Sends AT+ZPPPCLOSE to disable GPRS
	/// 
	/// Returns >0 on success, <0 on fail
	int close();
	
	/// status() - Checks the GPRS connection status
	/// This function returns the result of "AT+ZPPPSTATUS". If it responds
	/// "ESTABLISHED", GPRS_ESTABLISHED (1) is returned.
	/// If it responds "DISCONNECTED" 0 is returned.
	int8_t status();
	
	/// localIP() - Returns the IP address assigned to the MG2639.
	/// Sends the AT+ZIPGETIP function.
	/// An object of type IPAddress is returned. This class is included with
	/// all Arduino cores - used for WiFi and Ethernet classes.
	///
	/// Returns: >0 on success, <0 on fail
	IPAddress localIP();
	
	/// hostByName([domain], [ipRet]) - Gets the IP address of the requested
	/// remote domain.
	/// e.g.: hostByName("sparkfun.com", returnIPhere);
	///
	/// Returns: >0 on success, <0 on fail
	int hostByName(const char * domain, IPAddress * ipRet);
	
	///////////////////////
	// TCP Link Commands //
	///////////////////////
	
	/// connect([ip], [port], [channel]) - Open a TCP connection to
	/// a specified [ip] address on a specific [port].
	/// [channel] defaults to 0. If you only need one connection open at a time,
	/// this variable can be ignored.
	/// e.g.: connect("204.144.132.37", 80);
	///
	/// Returns: >0 on success, <0 on fail
	int connect(IPAddress ip, unsigned int port, uint8_t channel = DEFAULT_CHANNEL); // AT+ZIPSETUP
	
	/// connect([domain], [port], [channel]) - Open a TCP connection to
	/// a specified [domain] on a specific [port].
	/// This function calls connect(ip, port, channel) after looking up an IP.
	/// [channel] defaults to 0. If you only need one connection open at a time,
	/// this variable can be ignored.
	/// e.g.: connect("sparkfun.com", 80);
	///
	/// Returns: >0 on success, <0 on fail
	int connect(const char * domain, unsigned int port, uint8_t channel = DEFAULT_CHANNEL);
	
	/// available() - Returns number of characters available in the UART RX buffer.
    virtual int available();
	
	/// read() - Reads the first character out of the UART RX buffer. Then
	/// removes it from the buffer.
    virtual int read();
	
	/// peek() - Looks at the first character in the UART RX buffer, but
	/// the character is left at the top of the buffer.
	//! TODO: not yet implemented
    virtual int peek();
	
	//! TODO: not yet implemented
    virtual void flush();
	
	/// write(b) - Send a single byte over a TCP link
	/// This function sends the "AT+ZIPSEND" command, then the requested byte.
	/// +ZIPSEND requires a TCP channel number be sent along with the data.
	/// This function uses the channel used in the last connect() function.
	///
	/// Returns: >0 on success, <0 on fail.
	virtual size_t write(uint8_t b);
	
	/// write([buf], size) - Send a character buffer (of length [size]) over
	/// a TCP link.
	/// +ZIPSEND requires a TCP channel number be sent along with the data.
	/// This function uses the channel used in the last connect() function.
	///
	/// Returns: >0 on success, <0 on fail.
	virtual size_t write(const uint8_t *buf, size_t size);

	using Print::write;
	
private:
	// Keep track of the active channel - the last value specified in [channel]
	// of connect([ip], [port], [channel])
	int8_t _activeChannel; 
	
	// Helper function to convert a char array to IPAddress object
	bool charToIPAddress(char * ipChar, IPAddress & ipRet);
};

extern MG2639_GPRS gprs;

#endif