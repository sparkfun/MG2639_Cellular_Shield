/******************************************************************************
SFE_MG2639_CellShield.h
MG2639 Cellular Shield Library Main Header File
Jim Lindblom @ SparkFun Electronics
Original Creation Date: April 3, 2015
https://github.com/sparkfun/MG2639_Cellular_Shield

This library implements all functions of the main MG2639_Cell class. Its main
goal is to handle low-level communication with the MG2639 cellular module. 

Development environment specifics:
	IDE: Arduino 1.6.3
	Hardware Platform: Arduino Uno
	MG2639 Cellular Shield Version: 1.0

This code is beerware; if you see me (or any other SparkFun employee) at the
local, and you've found our code helpful, please buy us a round!

Distributed as-is; no warranty is given.
******************************************************************************/

#ifndef _SFE_MG2639_CELLSHIELD_H_
#define _SFE_MG2639_CELLSHIELD_H_

#include <SoftwareSerial.h> // SoftwareSerial used to communicate with shield
#include <inttypes.h>
#include "util/MG2639_SMS.h"	// SMS (text messaging) functions (send, read, etc.)
#include "util/MG2639_GPRS.h" // GPRS functions (TCP connect, send, etc.)
#include "util/MG2639_Phone.h" // Phone call functions (answer, dial, hangup, etc.)

////////////////////////
// Memory Allocations //
////////////////////////
#define RX_BUFFER_LENGTH 64 // Number of bytes in the SS receive buffer

/////////////////////
// Pin Definitions //
/////////////////////
#define CELL_SW_RX	3	// Cellular module UART0 RXI goes to Arduino pin 3
#define CELL_SW_TX	2	// Cellular module UART0 TXO goes to Arduino pin 2
#define CELL_ON_OFF	7	// PWRKEY_N on cell module goes to Arduino pin 7

////////////////////////////////
// SoftwareSerial Definitions //
////////////////////////////////
// TARGET_BAUD_RATE sets the desired communication rate between Arduino and
// MG2639. 9600 is a safe rate -- higher bauds are much less reliable.
#define TARGET_BAUD_RATE 9600

////////////////////////////
// Timing Characteristics //
////////////////////////////
// All constants in this section defined in milliseconds
#define POWER_PULSE_DURATION	3000  // 2-5s pulse required to turn on/off
#define MODULE_OFF_TIME			10000 // Time the module takes to turn off
#define COMMAND_RESPONSE_TIME	500  // Command response timeout on UART
#define MODULE_WARM_UP_TIME		3000  // Time between on and ready

//////////////////////////
// Response Error Codes //
//////////////////////////
enum cmd_response {
	ERROR_OVERRUN_PREVENT = -4, 
	ERROR_UNKNOWN_RESPONSE = -3, // Unknown response
	ERROR_FAIL_RESPONSE = -2, // An identified error response (e.g. "ERROR")
	ERROR_TIMEOUT = -1, // No response received withing specified timeout
	SUCCESS_OK = 1 // Good response.
};

class MG2639_Cell
{
public:
	////////////////////////////////////
	// Constructor and Initialization //
	////////////////////////////////////
	
	/// MG2639_Cell() - Constructor
	/// - Initializes rxBuffer and uart0.
	MG2639_Cell();
	
	/// begin([baud]) - Cell shield initialization at specified baud rate
	///
	/// Attempt to initialize the cellular shield at a specific baud rate.
	/// Autobaud will still be attempted. If the set baud rate is different
	/// from the [baud] parameter, it will be changed.
	/// On a successful exit the cell module's echo behavior will be
	/// turned off and the baud rate will be set to TARGET_BAUD_RATE.
	///
	/// Returns: 0 if communication fails, 1 on success.
	uint8_t begin(unsigned long baud);
	
	/// begin() - Cell shield initialization (Autobaud & TARGET_BAUD_RATE)
	///
	/// With no parameters, begin() will not assume anything about the cell
	/// shield's baud rate. A simple autobaud will run to try to discover the
	/// module. On a successful exit the cell module's echo behavior will be
	/// turned off and the baud rate will be set to TARGET_BAUD_RATE.
	///
	/// Returns: 0 if communication fails, 1 on success.
	uint8_t begin();
	
	///////////////////////
	// Baud Rate Control //
	///////////////////////
	
	/// changeBaud([from], [to]) - Change the baud rate from [from] baud 
	/// rate to [to].
	/// Ex: changeBaud(115200, 9600) // Change baud rate from 115200 to 9600
	///
	/// Returns: cmd_response error on fail, >1 on success.
	int changeBaud(unsigned long from, unsigned long to);
	
	/// autoBaud() - Discover module's baud rate - Attempt to discover the 
	/// module's set baud rate from a list of pre-defined values.
	/// Ex: unsigned long currentBaud = autoBaud();
	///
	/// Returns: 0 on fail, discovered baud rate on success
	unsigned long autoBaud();
	
	/////////////////////////
	// Information Queries //
	/////////////////////////
	
	/// checkSIM() - Check if a SIM card is present
	/// Sends the AT*TSIMINS command to the MG2639
	///
	/// Returns; true if SIM card is present, false if not
	bool checkSIM();
	
	/// getInformation([infoRet]) - Get manufacturer, hardware, software info
	///
	/// Sends the ATI command to the MG2639.
	/// On successful return, infoRet will contain the information string.
	/// Return: <0 for fail, >0 for success
	int8_t getInformation(char * infoRet);
	
	/// getIMI([cimiRet]) - Get International Mobile Identification #
	///
	/// Sends the AT+CIMI command to the MG2639
	/// On successful return, [imiRet] will contain the results of query.
	/// Return: <0 for fail, >0 for success
	int8_t getIMI(char * imiRet);
	
	/// getICCID([iccidRet]) - Get ICCID in SIM card
	///
	/// Sends the AT+ZGETICCID command to the MG2639
	/// On successful return, [iccidRet] will contain results of query.
	/// Return: <0 for fail, >0 for success
	int8_t getICCID(char * iccidRet);
	
	/// getPhoneNumber([phoneRet]) - Get phone number of SIM card
	///
	/// Sends the AT+CNUM command to the MG2639
	/// On successful return, [phoneRet] will contain the owners phone number
	/// stored in the SIM card.
	/// Return: <0 for fail, >0 for success
	int8_t getPhoneNumber(char * phoneRet);
	
	/// getIMEI([imeiRet]) - Get the IMEI of the MG2639 module.
	///
	/// Sends the AT+GSN command.
	/// On successful return, [imeiRet] will contain the 15-digit IMEI of the
	/// MG2639.
	/// Return: <0 for fail, >0 for success
	int8_t getIMEI(char * imeiRet);
	
	//////////////////////////////
	// Friend Class Definitions //
	//////////////////////////////
	// The GPRS, SMS, and Phone functions in this library are split into
	// separate classes. See the other source files of this library for
	// documentation on those classes.
	friend class MG2639_GPRS;
	friend class MG2639_SMS;
	friend class MG2639_Phone;

private:
	// SoftwareSerial is used to communicate with the shield's UART0.
	SoftwareSerial uart0;
	
	// Characters received on the software serial uart are stored in rxBuffer.
	// rxBuffer is a circular buffer with no overwrite protection.
	//! TODO: These are also stored in SoftwareSerial buffer (?). Find a way to
	//! share those buffers so we're not wasting twice as much SRAM.
	unsigned char rxBuffer[RX_BUFFER_LENGTH];
	unsigned int bufferHead; // Holds position of latest byte placed in buffer.
	
	//////////////////////////////
	// Initialization Functions //
	//////////////////////////////
	
	/// initializePins() - Sets up direction and initial state of shield pins
	void initializePins();
	
	/// initializeUART([baud]) - Starts the SoftwareSerial uart at [baud]
	void initializeUART(long baud);
	
	/// powerPulse() - Sends a power pulse signal
	/// Depending on the state of the MG2639, this will either turn the module
	/// on or off.
	void powerPulse();
	
	/// bruteForceBaudChange([from], [to], [tries])
	/// LEGACY: This function is no longer required with the release of Arduino 1.6.1
	/// Previously: SoftwareSerial was very unreliable at 115200. 1 of 100 character writes
	/// may be correct. This function tried to send 100's of baud change commands in the
	/// hope that one of them was clear.
	uint8_t bruteForceBaudChange(unsigned long from, unsigned long to, unsigned int tries);
	
	//////////////////////////////////
	// Command and Response Drivers //
	//////////////////////////////////
	
	/// sendATCommand([command]) - Send an AT command to the module. This 
	/// function takes a command WITHOUT the preceding "AT". It will add 
	/// the "AT" in the beginning and '\r' at the end.
	/// Ex: sendATCommand("E0"); // Send ATE0\r to turn echo off
	void sendATCommand(const char * command);
	
	/// readBetween([begin], [end], [rsp], [timeout]) - Read directly from the
	/// UART. Throw away characters before [begin], then store all characters
	/// between that and [end] into the [rsp] array.
	/// If [begin] is not found within [timeout] ms, the function exits.
	/// Returns: 0 on fail, 1 on success
	int8_t readBetween(char begin, char end, char * rsp, unsigned int timeout);
	
	/// readWaitForResponse([goodRsp], [timeout]) - Read from UART and store in 
	/// rxBuffer until [goodRsp] string is received.
	/// If [goodRsp] is not received within [timeout] ms, the function exits.
	/// Returns: ERROR_TIMEOUT if a timeout occurs, >1 if it finds [goodRsp]
	int readWaitForResponse(const char * goodRsp, unsigned int timeout);
	
	/// readWaitForResponses([goodRsp], [failRsp], [timout] - Read directly
	/// from UART until EITHER [goodRsp] or [failRSP] are found.
	/// This function allows you to check for either a success string 
	/// (e.g. "OK") or a fail string (e.g. "ERROR").
	/// Returns: 
	///  - ERROR_TIMEOUT (-1) if a timeout
	///  - ERROR_FAIL_RESPONSE (-2) if [failRsp] was found
	///  - ERROR_UNKNOWN_RESPONSE (-3) if response wasn't [failRsp] or [goodRsp]
	///  - >1 if [goodRsp] was found.
	int readWaitForResponses(const char * goodRsp, const char * failRsp, unsigned int timeout);
	
	/// readUntil([dest], [end], [maxChars], [tiemout]) - This is a multipurpose
	/// function that reads character from the UART to a character array until
	/// either:
	///  - [timeout] - a timeout it reached. [timeout] value is in ms.
	///  - [maxChars] - A set number of characters have been read.
	///  - [end] - An [end] character is read.
	///
	/// Returns:
	///  - ERROR_TIMEOUT (-1) if a timeout
	///  - ERROR_OVERRUN_PREVENT (-4) if maxChars limit was reached
	///  - >1 if [end] character was read.
	int readUntil(char * dest, char end, int maxChars, unsigned int timeout);
	
	//////////////////////////////////
	// rxBuffer Searching Functions //
	//////////////////////////////////
	
	/// getSubstringBetween([dest], [src], [in], [out]) - Searches [src] string
	/// for a string between [in] and [out] characters. On exit [dest] contains
	/// string between (not including) requested characters.
	/// Returns: -1 fail, >1 success
	int getSubstringBetween(char * dest, const char * src, char in, char out);
	
	////////////////////////////
	// Configuration Commands //
	////////////////////////////
	
	/// setEcho([on]) -- Sets echo on or off.
	/// To limit the data sent back by the MG2639, the library begin's by
	/// turning echo off, then assumes its that way for the rest of the
	/// program.
	/// Returns: <0 if the module didn't respond. >0 on success.
	int setEcho(uint8_t on);
	
	//////////////////////
	// UART Abstraction //
	//////////////////////
	
	/// printString([str]) - Send a string of characters out the UART
	void printString(const char * str, size_t length);
	void printString(const char * str);
	
	/// printChar([c]) - Send a single character out the UART
	void printChar(char c);
	
	/// dataAvailable() - Returns number of characters available in
	/// UART receive buffer.
	int dataAvailable();
	
	/// uartRead() - UART read char abstraction
	unsigned char uartRead();
	
	/// readByteToBuffer() - Read first byte from UART receive buffer
	/// and store it in rxBuffer.
	unsigned int readByteToBuffer();
	
	/// clearSerial() - Empty UART receive buffer
	void clearSerial();
	
	//////////////////////
	// rxBuffer Control //
	//////////////////////
	
	/// clearBuffer() - Reset buffer pointer, set all values to 0
	void clearBuffer();
	
	/// searchBuffer([test]) - Search buffer for string [test]
	/// Success: Returns pointer to beginning of string
	/// Fail: returns NULL
	//! TODO: Fix this function so it searches circularly
	char * searchBuffer(const char * test);	
};

extern MG2639_Cell cell;

#endif