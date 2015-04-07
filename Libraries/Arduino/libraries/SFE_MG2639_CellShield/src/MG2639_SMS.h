/******************************************************************************
MG2639_SMS.h
MG2639 Cellular Shield Library - SMS Functionality Header
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

#ifndef _MG2639_SMS_H_
#define _MG2639_SMS_H_

#include <Arduino.h>
#include <Print.h>

// MAX_PHONE_NUMBER_SIZE - Phone numbers can be a maximum of 15 characters
#define MAX_PHONE_NUMBER_SIZE 16
// MAX_DATE_SIZE - Defines maximum size of date character array.
#define MAX_DATE_SIZE 32
// SMS_DATA_SIZE - Defines the maximum size of the SMS text data array.
#define SMS_DATA_SIZE 128

// MESSAGE_INDEX_MAX defines the number of messages states that can be stored
// by the library. This defines the number of bytes used to store, so multiply
// by 8 to get the real value.
#define MESSAGE_INDEX_MAX 32 // Will store 32*8 (256) message indexes.

// sms_status enumerates the three possible SMS read requests:
// 0: REC_UNREAD - Only return unread messages
// 1: REC_READ - Only return read messages
// 2: REC_ALL - Return read & unread messages
enum sms_status {
	REC_UNREAD,
	REC_READ,
	REC_ALL
};

// sms_mode enumerates the two possible SMS modes:
// 0: PDU mode - Protocol data unit mode
// 1: Text mode - Text data mode 
enum sms_mode {
	SMS_PDU_MODE,
	SMS_TEXT_MODE
};

class MG2639_SMS : public Print
{
public:
	/// MG2639_SMS() - Constructor
	/// Sets up class variables
	MG2639_SMS();
	
	/// setMode([mode]) - Set SMS mode to either PDU or text mode.
	/// Of the two, text mode is probably the more useful. This produce messages
	/// with ASCII characters in their body.
	/// PDU mode returns messages with pure hex codes in the body.
	/// e.g.: setMode(SMS_TEXT_MODE)
	///
	/// Returns: >0 on success, <0 on fail.
	int8_t setMode(sms_mode mode);
	
	//////////////////////////////////////
	// Reading SMS Commands and Utility //
	//////////////////////////////////////
	
	/// available(status) - Returns the number of available messages of a
	/// specified sms_status type. sms_status can be either REC_UNREAD,
	/// REC_READ, or REC_ALL.
	/// This function returns the INDEX OF THE FIRST AVAILABLE MESSAGE with
	/// the requested status.
	/// e.g.: msgIndex = available(REC_READ); // Get index of first available read message
	///
	/// Returns: >0 (message index) on success, <0 on fail.
	int available(sms_status status = REC_UNREAD);
	
	/// pollAvailable() - This function attempts to poll for a "+CMTI" message
	/// from the MG2639.
	/// This is a polling function - the best way to use it is to call it in
	/// loop() and call it often. It can very easily miss a "+CMIT" alert.
	/// Use of available() is recommended over this function.
    int pollAvailable();
	
	/// read([msgIndex]) - Perform an SMS read on the specified index.
	/// This function does not return a message or sender, but it does update
	/// the values returned by getSender(), getDate() and getMessage(). Call
	/// those function after performing a read().
	/// Use the return from available() to get a message index.
	///
	/// Returns: >0 on success, <0 on fail.
	int8_t read(uint8_t msgIndex);
	
	/// deleteMessage([msgIndex]) - Delete a message at specified index from the
	/// contents of a SIM card.
	///
	/// Returns: >0 on success, <0 on fail.
	int8_t deleteMessage(uint8_t msgIndex);
	
	/// getSender() - Returns the phone number from the last, read SMS.
	/// A read(msgIndex) function must be called to update this value.
	char * getSender();
	
	/// getDate() - Returns a date string containing the date a read message
	/// was sent.
	/// A read(msgIndex) call must be made to update this value.
	char * getDate();
	
	/// getMessage() - Returns the message contents of a read SMS.
	/// A read(msgIndex) call must be made to update this value.
	char * getMessage();
	
	/// getOverrun() returns a boolean indicating if the last read SMS was
	/// larger than SMS_DATA_SIZE. In that case, the any chars above that
	/// point were cut off.
	//! No recourse for this yet, but at least we're failing gracefully.
	inline bool getOverrun() { return messageOverrun;};
	
	//////////////////////////////////////
	// SMS Sending Commands and Utility //
	//////////////////////////////////////
	
	/// start([phoneNumber]) - Begin writing an SMS message to [phoneNumber]
	/// Sending an SMS is a three+ step process: start(), print(), and send().
	/// [phoneNumber] should be a CHARACTER ARRAY.
	/// e.g.: start("13032840979"); // begin writing a text message to sparkfun
	///
	/// Returns: >0 on success, <0 on fail.
	int8_t start(const char * phoneNumber);
	
	/// send() - Send an SMS to the number specified in start().
	/// This function closes an SMS send. One or more calls to write() or
	/// print() should come between start() and this function.
	///
	/// Returns: >0 on success, <0 on fail.
	int8_t send();

	/// write(b) - Write a single byte to the SMS message buffer.
	virtual size_t write(uint8_t b);
	
	/// write([buf], [size]) - Write an array of bytes - of size [size] - to
	/// the SMS buffer.
	virtual size_t write(uint8_t *buf, size_t size);
	
	/// Print is a virtual function defined in Print.h and Print.cpp. In those
	/// files, print is defined for a large number of data types.
	/// It uses the write(*buf, size) function to perform its action.
	using Print::write;	
	
private:
	// Storage array for destination phone number
	char _destPhone[MAX_PHONE_NUMBER_SIZE];
	// Storage array for message statuses
	uint8_t _msgIndex[MESSAGE_INDEX_MAX];
	// Storage array for sending phone number, updated by read().
	char _lastNumber[MAX_PHONE_NUMBER_SIZE];
	// Storage array for send date, updated by read()
	char _lastDate[MAX_DATE_SIZE];
	// Storage array for SMS message content, updated by read()
	char _lastSMSData[SMS_DATA_SIZE];
	// Boolean to track if our last message read was too long:
	bool messageOverrun;
};

extern MG2639_SMS sms;

#endif