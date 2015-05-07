/************************************************************
MG2639_Phone.h
MG2639 Cellular Shield library - Phone call/receive example
Jim Lindblom @ SparkFun Electronics
Original Creation Date: April 3, 2015
https://github.com/sparkfun/MG2639_Cellular_Shield

This example demonstrates how to use the phone functionality
of the SparkFun MG2639 Cellular Shield library. Functions
include:
  phone.setAudioChannel(channel) - Set the speaker/mic
    interface to either differential or single pins.
  phone.status() - Check whether the phone is idle, active,
    making an outgoing call, or receiving a call.
  phone.callerID(phoneNumber) - Check the phone number of an
    incoming call.
  phone.answer() - Pick up a ringing call.
  phone.hangUp() - Hang up a call.
  phone.dial(phoneNumber) - Make an outgoing call.
    
Development environment specifics:
	IDE: Arduino 1.6.3
	Hardware Platform: Arduino Uno
	MG2639 Cellular Shield Version: 1.0

This code is beerware; if you see me (or any other SparkFun 
employee) at the local, and you've found our code helpful, 
please buy us a round!

Distributed as-is; no warranty is given.
************************************************************/
// The SparkFun MG2639 Cellular Shield uses SoftwareSerial
// to communicate with the MG2639 module. Include that
// library first:
#include <SoftwareSerial.h>
// Include the MG2639 Cellular Shield library
#include <SFE_MG2639_CellShield.h>

char myPhone[15]; // Array to store local phone number
char yourPhone[15]; // Array to store incoming phone number

// This sketch sets up as a state machine. Our Arduino phone
// can be in any of these possible states:
enum phone_state
{
  PHONE_IDLE,
  PHONE_INCOMING_RINGING,
  PHONE_INCOMING_ACTIVE,
  PHONE_OUTGOING_DIALING,
  PHONE_OUTGOING_RINGING,
  PHONE_OUTGOING_ACTIVE,
} phoneState = PHONE_IDLE;

// currenStatus contains the MG2639 modules actual status.
// We'll use phone.status() to update it.
int8_t currentStatus;

void setup() 
{
  // USB serial connection is used to print the information
  // we find.
  Serial.begin(9600);
  
  // serialTrigger() halts execution of the program until
  // any value is received over the serial link. Cell data
  // costs $, so we don't want to use it unless it's visible!
  serialTrigger();
  
  // Use cell.begin() to turn the module on and verify that
  // we can communicate with it.
  uint8_t status = cell.begin();
  if (status <= 0)
  {
    Serial.println(F("Unable to communicate. Looping"));
    while(1)
      ;
  }
  // Need a delay here just in case. If the phone was off, 
  // it'll takes a couple seconds to set up SIM
  delay(2000);  
  
  // Use cell.getPhoneNumber() to find the local phone 
  // number assigned to this SIM card.
  cell.getPhoneNumber(myPhone);
  Serial.print(F("My phone number is: "));
  Serial.println(myPhone);
  Serial.println(F("Call me!"));
  Serial.println(F("Or send 'D<phone number>X' to make a call."));
  
  // Use phone.setAudioChannel(channel) to tell the module
  // if your speaker/microphone are connected to either the
  // single (EAR_SPK/EAR_MIC) or differential (MIC+/MIC- and
  // SPK+/SPK-) pins.
  //phone.setAudioChannel(AUDIO_CHANNEL_SINGLE);
  phone.setAudioChannel(AUDIO_CHANNEL_DIFFERENTIAL);
}

void loop() 
{
  // loop() just runs our state machine function.
  phoneStateMachine();
}

void phoneStateMachine()
{
  // Our phone Arduino should be in one of the states 
  // enumerated at the top of this sketch. The state of our
  // phone is stored in phoneState
  switch (phoneState)
  {
  // Phone is idle, no active, incoming, or outgoing calls.
  case PHONE_IDLE: 
    // Check the phone status with phone.status().
    if (phone.status() == CALL_INCOMING)
    { // If a call is incoming
      printCallerID(); // Check the caller ID
      // And change the state to incoming/ringing:
      phoneState = PHONE_INCOMING_RINGING; 
    }
    else if (Serial.available())
    { // Otherwise, if serial's available, maybe we want to
      // make a phone call:
      if (makePhoneCall())
      {
        // If makePhoneCall() successfully returned, print
        // a message:
        Serial.println();
        Serial.println("Press any key to hang up.");
        Serial.print("Dialing");
        // Then change the state to outgoing/dialing:
        phoneState = PHONE_OUTGOING_DIALING;
      }
    }
    break; // end PHONE_IDLE state
  // State: Phone call incoming, ringing:
  case PHONE_INCOMING_RINGING:
    // Check current status of phone. If it's <0 the module
    // is idle. The other line must have hung up. (You took
    // too long to answer!)
    if (phone.status() < 0)
    {
      Serial.println("Caller hung up.");
      // Change state back to idle.
      phoneState = PHONE_IDLE;
    }
    else if (checkForSerial())
    {
      // If serial is available, answer the phone
      phone.answer();
      Serial.println("Answered. Press any key to hang up.");
      // Change state to incoming/active:
      phoneState = PHONE_INCOMING_ACTIVE;
    }
    break; // end PHONE_INCOMING_RINGING
  // State: Phone call active (incoming or outgoing)
  case PHONE_INCOMING_ACTIVE:
  case PHONE_OUTGOING_ACTIVE:
    // If phone.status() is <0 the call has ended. The other
    // line probably hung up.
    if (phone.status() < 0)
    {
      Serial.println("Caller hung up.");
      // Change state back to idle.
      phoneState = PHONE_IDLE;
    }
    else if (checkForSerial())
    {
      // If serial is available, we'll hang up.
      hangUp();
      // Change state back to idle:
      phoneState = PHONE_IDLE;
    }
    break; // end PHONE_INCOMING_ACTIVE/PHONE_OUTGOING_ACTIVE
  // State: Outgoing phone call dialing:
  case PHONE_OUTGOING_DIALING:
    // Check phone.status(), store in variable
    currentStatus = phone.status();
    if (checkForSerial())
    {
      // If serial is available, we'll hang up
      hangUp();
      // Change state back to idle
      phoneState = PHONE_IDLE;
    }
    else if (currentStatus < 0)
    {
      // If phone.status() returns <0, the call has ended.
      // The other party probably hung up/rejected.
      Serial.println("Call failed.");
      phoneState = PHONE_IDLE;
    }
    else if (currentStatus == CALL_DIALING)
    {
      // If we're still dialing, print another . and delay
      // for half a second:
      Serial.print('.');
      delay(500);
    }
    else if (currentStatus == CALL_DIALED_RINGING)
    {
      // If the state has changed to ringing. Print a new
      // message:
      Serial.println();
      Serial.print("Ringing");
      // Change the state to outgoing/ringing:
      phoneState = PHONE_OUTGOING_RINGING;
    }
    break; // end PHONE_OUTGOING_DIALING
  case PHONE_OUTGOING_RINGING:
    currentStatus = phone.status();
    if (checkForSerial())
    {
      // If serial is available, hang up:
      hangUp();
      // Change state back to idle
      phoneState = PHONE_IDLE;
    }
    else if (currentStatus < 0)
    {
      // If phone.status() returns <0 the call has ended.
      // The other party probably hung up.
      Serial.println("Call failed.");
      phoneState = PHONE_IDLE;
    }
    else if (currentStatus == CALL_ACTIVE)
    {
      // If phone.status() returns active the other line
      // picked up. We're on.
      Serial.println();
      Serial.println("Other line picked up. You're on!");
      Serial.println("Press any key to hang up.");
      // Change state to outgoing/active:
      phoneState = PHONE_OUTGOING_ACTIVE;
    }
    else if (currentStatus == CALL_DIALED_RINGING)
    {
      // If we're still in the same state, print another .
      // and delay half a second.
      Serial.print('.');
      delay(500);
    }
    break;
  }
}

void printCallerID()
{
  // phone.callerID(phone) will return (by reference) the
  // phone number of an incoming phone call:
  phone.callerID(yourPhone);
  Serial.print("Phone call coming in from: ");
  Serial.println(yourPhone);
  Serial.println("Press any key to answer.");
}

void hangUp()
{
  // phone.hangUp() will end a call whether it's active,
  // outgoing or ringing.
  phone.hangUp();
  Serial.println();
  Serial.println("Hung up.");
}

bool checkForSerial()
{
  // Check if a character has come in on Serial
  if (Serial.available())
  {
    // Clear out that character and any others that
    // may have come in.
    while (Serial.available())
      Serial.read();
    // Return true - we received a character
    return true;
  }
  else
  {
    // No characters received - return false
    return false;
  }
}

// Phone calls are initiated by the Serial Monitor. A string
// formated as DnnnnnnnnnnnX (where n is any numeral) will
// define the number we're calling. The number string can be 
// any length - as required for the phone number being dialed
bool makePhoneCall()
{
  // We'll enter this function only when a serial character
  // is available.
  char c = Serial.read(); // Read the first available char
  if (c == 'D') // If the first char is 'D', get the #:
  {
    // Clear yourPhone
    memset(yourPhone, 0, strlen(yourPhone));
    int i = 0;
    // Loop until we read a 'X', or the number gets too big:
    while ((c != 'X') && (i < 15))
    {
      while (!Serial.available())
        ;
      c = Serial.read();
      if ((c >= '0') && (c <= '9')) // If c is a numeral
        yourPhone[i++] = c; // Add it to yourPhone
    }
    Serial.print("Calling ");
    Serial.println(yourPhone);
    
    // phone.dial(phone) is used to place an outgoing phone
    // call.
    phone.dial(yourPhone);
    
    return true;
  }
  else
  {
    return false;
  }
}

void serialTrigger()
{
  Serial.println("Send some serial to start");
  while (!Serial.available())
    ;
  while (Serial.available())
    Serial.read();
}