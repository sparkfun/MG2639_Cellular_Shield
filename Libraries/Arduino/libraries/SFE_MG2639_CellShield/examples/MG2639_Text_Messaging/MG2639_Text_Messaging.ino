/************************************************************
MG2639_Text_Messaging.h
MG2639 Cellular Shield library - Read/Send SMS Example
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
  sms.start(phoneNumber) - Begin writing an SMS to the
    specified phone number.
  sms.print(), sms.println() and sms.write() - Write the
    message body with these functions. Multiple can be called
    to create a long message.
  sms.send() - Once the message has been written, call
    this function to send it.

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

// A character to confirm the end of a written message.
// Any message you write will not be able to include this
// character.
const char EOM_CHAR = '~';

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
  
  Serial.print(F("To send a message begin by typing "));
  Serial.print(F("WnnnnnnnnnnX. Where n's are digits"));
  Serial.println(F(" in the destination phone number"));
  Serial.println();
}

void loop() 
{
  // Get first available unread message:
  byte messageIndex = sms.available(REC_UNREAD);
  // If an index was returned, proceed to read that message:
  if (messageIndex > 0)
  {
    printSMS(messageIndex);
    
    Serial.println(F("Would you like to delete the message? y/n"));
    while (!Serial.available())
      ;
    char c = Serial.read();
    if ((c == 'y') || (c == 'Y'))
    {
      sms.deleteMessage(messageIndex);
      Serial.println("Message deleted.");
      Serial.println();
    }
  }
  // If serial was received, maybe we'll send a message:
  if (Serial.available())
  {
    char c = Serial.read();
    // Begin an SMS send with the 'W' character.
    if (c == 'W')
    {
      writeSMS();
    }
  }
}

void writeSMS()
{
  char c = 0;
  char destinationPhone[16];
  uint8_t i = 0;
  // Loop until we see a terminating 'X', or receive too
  // many characters.
  while ((c != 'X') & (i < 16))
  {
    // Serial.read() should return -1 if there's nothing
    // there.
    c = Serial.read();
    if ((c >= '0') && (c <= '9'))
      destinationPhone[i++] = c;
  }
  
  Serial.print(F("Sending a message to "));
  Serial.println(destinationPhone);
  
  // To begin sending an SMS, use sms.start(phoneNumber):
  sms.start(destinationPhone);
  
  Serial.print(F("Type your message. End it by sending: "));
  Serial.println(EOM_CHAR);
  
  // Loop until we see the EOM character.
  while (c != EOM_CHAR)
  {
    if (Serial.available())
    {
      c = Serial.read();
      Serial.print(c);
      if ((c != EOM_CHAR) && (c >= 0))
        sms.print((char)c); // Add to a message with sms.print
    }
  }
  Serial.println();
  Serial.println("Sending the message.");
  Serial.println();
  
  // Send an sms message by calling sms.send()
  sms.send();
}

void printSMS(int index)
{
  // Call sms.read(index) to read the message:
  sms.read(index);
  
  Serial.print(F("Reading message ID: "));
  Serial.println(index);
  // Print the sending phone number:
  Serial.print(F("SMS From: "));
  Serial.println(sms.getSender());
  // Print the receive timestamp:
  Serial.print(F("SMS Date: "));
  Serial.println(sms.getDate());
  // Print the contents of the message:
  Serial.print(F("SMS Message: "));
  Serial.println(sms.getMessage());
  Serial.println();
  
}
void serialTrigger()
{
  Serial.println(F("Send some serial to start"));
  while (!Serial.available())
    ;
  Serial.read();
}
