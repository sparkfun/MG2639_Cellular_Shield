/******************************************************************************
MG2639_AT.h
MG2639 Cellular Shield Library - AT command definitions
Jim Lindblom @ SparkFun Electronics
Original Creation Date: April 3, 2015
https://github.com/sparkfun/MG2639_Cellular_Shield

This header defines AT commands used throughout the MG2639 Cell Shield library.

Development environment specifics:
	IDE: Arduino 1.6.3
	Hardware Platform: Arduino Uno
	MG2639 Cellular Shield Version: 1.0

This code is beerware; if you see me (or any other SparkFun employee) at the
local, and you've found our code helpful, please buy us a round!

Distributed as-is; no warranty is given.
******************************************************************************/

//////////////////////
// Common Responses //
//////////////////////
const char RESPONSE_OK[] = "OK";
const char RESPONSE_ERROR[] = "ERROR";

/////////////////////
// Common Commands //
/////////////////////
// These commands do not require a precding "AT":
const char REPEAT[] = "A/";	// Repeat the previous command.
const char ENTER_CMD_MODE[]  = "+++";	// Switch from data mode to command mode
// These commands DO require a preceding "AT":
const char ANSWER[] = "A";		// Answer a call.
const char DIAL[] = "D";		// Originate a voice call, data, and fax call.
const char DIAL_LAST[] = "DL";	// Dial the last number called
const char HANG_UP[] = "H";		// Hang up the call
const char ENABLE_ECHO[] = "E1";	// Enable command echo
const char DISABLE_ECHO[] = "E0";	// Disable command echo
const char GET_INFORMATION[] = "I";		// Display the module's manufacturer's information
const char DISPLAY_RETURN[] = "Q";		// Set whether or not to display the returned value
const char ENTER_DAT_MODE[] = "O";		// Switch from command mode to data mode
const char PULSE_DIALING[] = "P";		// Set dialing method to pulse
const char AUTO_ANSWER[] = "S0";	// Control the module's auto-answer mode
const char SET_RINGER[] = "+CRC";	//
const char READ_IMI[] = "+CIMI";	// Read the international mobile identification of SIM
const char GET_IMEI[] = "+GSN"; // Get the current device's IMEI
const char CHECK_SIM[] = "*TSIMINS?"; // Check SIM card status
const char CHECK_STATUS[] = "+CLCC";

///////////////////////////////
// Data Compression Commands //
///////////////////////////////
const char FLOW_CONTROL[] = "+IFC";
const char SET_DTR_MODE[] = "%D";
const char	SET_DCD_MODE[] = "%C";
const char SET_BAUD_RATE[] = "+IPR";

////////////////////////////
// ZTE Exclusive Commands //
////////////////////////////
const char MODULE_STATUS[] = "+ZSTR";
const char GET_ICCID[] = "+ZGETICCID";


////////////////////////////////
// Network Parameter Commands //
////////////////////////////////
const char OPEN_GPRS[] = "+ZPPPOPEN";		// Open a the GPRS connection
const char CLOSE_GPRS[] = "+ZPPPCLOSE";	// Close the GPRS connection
const char GET_IP[] = "+ZIPGETIP";	// Check current IP address
const char DNS_GET_IP[] = "+ZDNSGETIP";	// Obtain internet domain name's IP address

//////////////////
// SMS Commands //
//////////////////
const char SMS_CENTER[] = "+CSCA";
const char SMS_ACK[] = "+CNMA";
const char SMS_MODE[] = "+CMGF";
const char SMS_INDICATION[] = "+CNMI";
const char SMS_READ[] = "+CMGR";
const char SMS_WRITE[] = "+CMGW";
const char SMS_SELECT[] = "+CSMS";
const char SMS_SEND[] = "+CMGS";
const char SMS_STORAGE[] = "+CPMS";
const char SMS_DELETE[] = "+CMGD";
const char SMS_LIST[] = "+CMGL";
const char SMS_SIM_SAVED[] = "+CMSS";
const char SMS_FULL[] = "+ZSMGS";

////////////////////////
// Phonebook Commands //
////////////////////////
const char OWNERS_NUMBER[] = "+CNUM";

///////////////////////
// TCP Link Commands //
///////////////////////
const char TCP_SETUP[] = "+ZIPSETUP";		// Set up a TCP link
const char TCP_SEND[] = "+ZIPSEND";		// Send data over a TCP link
const char TCP_STATUS[] = "+ZPPPSTATUS";	// Check GPRS connection status

////////////////////
// Audio Commands //
////////////////////
const char SPEAKER_SELECT[] = "+SPEAKER";