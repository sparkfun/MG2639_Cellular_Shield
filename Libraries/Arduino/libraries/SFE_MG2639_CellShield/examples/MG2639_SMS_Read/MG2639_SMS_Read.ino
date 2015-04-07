/************************************************************
MG2639_SMS_Read.h
MG2639 Cellular Shield library - Read SMS Example
Jim Lindblom @ SparkFun Electronics
Original Creation Date: April 3, 2015
https://github.com/sparkfun/MG2639_Cellular_Shield

This example demonstrates how to use the SMS functionality in
the SparkFun MG2639 Cellular Shield Library. It simulates an
SMS message log - cycling through every message stored in the
SIM card, presenting the body, and asking if you want to 
delete the message. Follow the prompts in the Serial Monitor
to play along.

Functions shown in this example include:
  sms.setMode() - Set SMS mode to either SMS_TEXT_MODE or
    SMS_PDU_MODE.
  sms.read(msgIndex) - Read the contents of an SMS identified
    by an index number.
  sms.getMessage() - Read the body of an SMS message (called
    after sms.read(index)).
  sms.getSender() - Read the phone number from the message
    sender (after a read()).
  sms.getDate() - Read the date an SMS was sent (after read).
  sms.deleteMessage(msgIndex) - Delete a text message at a
    specified index.  

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

void setup() 
{
  // USB serial connection is used to print the information
  // we find and interact with the user.
  Serial.begin(9600);
  
  // serialTrigger() halts execution of the program until
  // any value is received over the serial link. Cell data
  // costs $, so we don't want to use it unless it's visible!
  serialTrigger();
  
  // Call cell.begin() to turn the module on and verify
  // communication.
  int beginStatus = cell.begin();
  if (beginStatus <= 0)
  {
    Serial.println(F("Unable to communicate with shield. Looping"));
    while(1)
      ;
  }
  // Delay a bit. If phone was off, it takes a couple seconds
  // to set up SIM.
  delay(2000);
  
  // Use cell.getPhoneNumber to get the local phone #.
  cell.getPhoneNumber(myPhone);
  Serial.println(F("Send me a text message!"));
  Serial.print(F("My phone number is: "));
  Serial.println(myPhone);
  Serial.println();

  // Set SMS mode to text mode. This is a more commonly used,
  // ASCII-based mode. The other possible mode is 
  // SMS_PDU_MODE - protocol data unit mode - an uglier,
  // HEX-valued data mode that may include address info or
  // other user data.
  sms.setMode(SMS_TEXT_MODE);
}

void loop() 
{
  // The library can read up to 256 messages. We'll use
  // msgCounter to set the index of the message we want to
  // read.
  static byte msgCounter = 1; // Begin by reading the first
  
  // sms.read(msgIndex) will attempt to read the SMS stored
  // at the requested index. If there is no message there, a
  // -2 is returned. If the message read was successful a
  // number >0 will be returned.
  if (sms.read(msgCounter) > 0)
  {
    // After an sms.read(), sms.getSender(), sms.getDate(),
    // and sms.getMessage() will be updated. We'll read those
    // and print them out.
    Serial.print(F("MSG index: "));
    Serial.println(msgCounter);
    // sms.getSender() retreives a string containing the
    // sending phone number.
    Serial.print(F("SMS From: "));
    Serial.println(sms.getSender());
    // sms.getDate() retreives the time - date, and time -
    // that the read message was sent at.
    Serial.print(F("SMS Date: "));
    Serial.println(sms.getDate());
    // sms.getMessage() retreives the body of an SMS.
    Serial.print(F("SMS Message: "));
    Serial.println(sms.getMessage());
    Serial.println();
    // Unfortunately, memory limitations of the Arduino mean
    // we can't store extremely large messages. By default,
    // the maximum message size is 128. If the message 
    // exceeds that size, a flag will be set and the extra
    // characters will be ignored.
    if (sms.getOverrun())
      Serial.println(F("Message too long. End was cut off."));
    
    // Ask if you want to delete the message:
    Serial.println(F("Would you like to delete this message? y/n"));
    while (Serial.available() <= 0)
      ; // Wait for a serial character to be available.
    char c = Serial.read();
    if ((c == 'y') || (c == 'Y'))
    {
      // sms.deleteMessage(index) deletes the contents of a
      // message entirely from the SIM card. The index is 
      // also freed up, so the next time you read that index
      // it may be something new.
      int8_t status = sms.deleteMessage(msgCounter);
      if (status > 0)
        Serial.println(F("Message deleted."));
    }
    else if ((c == 'n') || (c == 'N'))
    {
      Serial.println(F("Message saved."));
    }
    else
    {
      Serial.println(F("Option not supported. Saving."));
    }
    msgCounter++;
  }
  else
  {
    Serial.print(F("No message at index "));
    Serial.println(msgCounter++);
  }
    Serial.println();
}

void serialTrigger()
{
  Serial.println(F("Send some serial to start"));
  while (!Serial.available())
    ;
  Serial.read();
}