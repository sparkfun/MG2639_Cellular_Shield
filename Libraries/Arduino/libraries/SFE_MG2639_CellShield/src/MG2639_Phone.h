/******************************************************************************
MG2639_Phone.h
MG2639 Cellular Shield Library - Phone Functionality Header
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

#ifndef _MG2639_PHONE_H_
#define _MG2639_PHONE_H_

#include <Arduino.h>

// Call status enum - These values match exactly what we can expect from the
// MG2639's response to "AT+CLCC".
enum call_status {
	CALL_ACTIVE,		// 0 - Call activated
	CALL_HOLD,			// 1 - Active on hold
	CALL_DIALING,		// 2 - Outgoing phone call, dialing out
	CALL_DIALED_RINGING,// 3 - Outgoing phone call, ringing on other end
	CALL_INCOMING,		// 4 - Incoming phone call, ringing
	CALL_WAITING		// 5 - Call waiting
};

// There are two possible input/output pairs for the MG2639 - 
// 1. Differential: SPK+/SPK- and MIC+/MIC- pins used
// 2. Single-ended: EAR_SPK and EAR_MIC pins used
enum audio_channel {
	AUDIO_CHANNEL_DIFFERENTIAL,
	AUDIO_CHANNEL_SINGLE
};

class MG2639_Phone
{
public:
	/// MG2639_Phone() - Constructor
	/// - Sets CELL_RING (A0) to an input
	MG2639_Phone();
	
	//////////////////////////////
	// Phone Call Status Checks //
	//////////////////////////////
	
	/// available() - Checks the CELL_RING pin to find it if a call is incoming
	/// This is a very simple yes/no is my phone ringing check. For a more
	/// complete function, check out status() below.
	bool available();
	
	/// status() - Returns the current call status - whether it's outgoing,
	/// incoming, active, or idle. The return value will be one of these:
	/// -2 (ERROR_FAIL_RESPONSE): No active call (idle)
	/// 0 (CALL_ACTIVE): Active call
	/// 1 (CALL_HOLD): Call on hold
	/// 2 (CALL_DIALING): Outgoing call, dialing out
	/// 3 (CALL_DIALED_RINGING): Outgoing call, ringing on other end
	/// 4 (CALL_INCOMING): Incoming call
	/// 5 (CALL_WAITING): Another call waiting
	int8_t status();	// AT+CLCC
	
	/// callerID([phoneNumber]) - Attempts to get the phone number of the 
	/// incoming call.
	/// Caller ID is returned by reference, [phoneNumber] can be up to
	/// 16 characters. On exit it will contain the number of the caller.
	///
	/// Returns: 1 on success, <0 on error (timeout, bad response, etc.)
	int8_t callerID(char * phoneNumber);
	
	///////////////////////////////////////////
	// Making, Receiving, Ending Phone Calls //
	///////////////////////////////////////////
	
	/// answer() - Answer an incoming phone call
	///
	/// Returns: >0 on success, <0 on error.
	int8_t answer();	// ATA
	
	/// hangUp() - Hang up an active or outgoing call. Reject an incoming call
	///
	/// Returns: >0 on success, <0 on error
	int8_t hangUp();	// ATH
	
	/// dial([phoneNumber]) - Place an outgoing call to [phoneNumber].
	/// [phoneNumber] should be a CHARACTER ARRAY (not a number).
	///
	/// Returns: >0 on success, <0 on error
	int8_t dial(char * phoneNumber);	// ATD<phoneNumber>
	
	/// dialLast() - Place a call to the previously dialed number
	///
	/// Returns: >0 on success, <0 on error
	int8_t dialLast();	// ATDL

	/////////////////////////////
	// Audio Interface Control //
	/////////////////////////////
	
	/// setAudioChannel([channel]) - Set the audio input/output channels to
	/// either the single or differential channels.
	/// [channel] should be either AUDIO_CHANNEL_DIFFERENTIAL or
	/// AUDIO_CHANNEL_SINGLE
	///
	/// Returns: >0 on success, <0 on error	
	int8_t setAudioChannel(audio_channel channel = AUDIO_CHANNEL_DIFFERENTIAL);
  
private:
  
};

extern MG2639_Phone phone;

#endif