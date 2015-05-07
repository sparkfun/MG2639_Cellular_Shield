/************************************************************
MG2639_SMS_AutoResponder.h
MG2639 Cellular Shield library - Read & auto-respond example
Jim Lindblom @ SparkFun Electronics
Original Creation Date: April 3, 2015
https://github.com/sparkfun/MG2639_Cellular_Shield

This example demonstrates how to use the SMS functionality in
the SparkFun MG2639 Cellular Shield Library. It shows how
to read an SMS, parse the contents of the message and sender
and send an SMS back.

After the sketch starts up, send a query SMS to it, like:
"Read A1" or "Read D13". If the Arduino sees a Read X 
request, it'll read that pin and respond with its value.

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

char myPhone[15]; // Char array to store local phone #

// pinReports is a struct that defines a message we're
// looking for, and pairs it with a pin to be read. There's
// also a flag to indicate if that pin is analog or digital.
struct pinReports
{
  const char inputString[9]; // String to check for
  bool isAnalogPin; // If true, we'll use analogRead
  int pin; // pin to be read
};

// PIN_REPORT_COUNT defines the number of pins 
// and messages available to be read
#define PIN_REPORT_COUNT 13

// Now we set up all 13 of our pin message/pin combinations.
// These structs will define the exact message to be looked
// for, and the pin to be read upon receipt of that message.
pinReports analog0Report = {"Read A0", true, A0};
pinReports analog1Report = {"Read A1", true, A1};
pinReports analog2Report = {"Read A2", true, A2};
pinReports analog3Report = {"Read A3", true, A3};
pinReports analog4Report = {"Read A4", true, A4};
pinReports analog5Report = {"Read A5", true, A5};
pinReports digital4Report = {"Read D4", false, 4};
pinReports digital5Report = {"Read D5", false, 5};
pinReports digital6Report = {"Read D6", false, 6};
pinReports digital10Report = {"Read D10", false, 10};
pinReports digital11Report = {"Read D11", false, 11};
pinReports digital12Report = {"Read D12", false, 12};
pinReports digital13Report = {"Read D13", false, 13};
// An array to point to each of those reports:
pinReports * pinReportArray[PIN_REPORT_COUNT] = {&analog0Report, 
  &analog1Report, &analog2Report, &analog3Report, &analog4Report, 
  &analog5Report, &digital4Report, &digital5Report, 
  &digital6Report, &digital10Report, &digital11Report, 
  &digital12Report, &digital13Report};
  
void setup() 
{
  // USB serial connection is used to print the information
  // we find and interact with the user.
  Serial.begin(9600);
  
  // serialTrigger() halts execution of the program until
  // any value is received over the serial link. Cell data
  // costs $, so we don't want to use it unless it's visible!
  serialTrigger();
  
  // Set all of our report pins as inputs:
  setupIOPins(); 
  
  // Run cell.begin() to initialize communication with the
  // module and set it up.
  uint8_t status = cell.begin();
  if (status <= 0)
  {
    // If begin() returns 0 or a negative number, the Arduino
    // is unable to communicate with the shield. Make sure
    // it's getting enough power. Try again making sure the
    // MG2639 is booted up before running the sketch.
    Serial.println(F("Unable to communicate with shield."));
    while(1)
      ;
  }
  // Delay a couple seconds to allow the module to warm up.
  delay(2000);
  
  // Use cell.getPhoneNumber to get the local phone #.
  cell.getPhoneNumber(myPhone);
  Serial.println(F("Send me a text message!"));
  Serial.print(F("My phone number is: "));
  Serial.println(myPhone);

  // Set SMS mode to text mode. This is a more commonly used,
  // ASCII-based mode. The other possible mode is 
  // SMS_PDU_MODE - protocol data unit mode - an uglier,
  // HEX-valued data mode that may include address info or
  // other user data.
  sms.setMode(SMS_TEXT_MODE);
}

void loop() 
{
  // Get first available unread message:
  byte messageIndex = sms.available(REC_UNREAD);
  // If an index was returned, proceed to read that message:
  if (messageIndex > 0)
  {
    // Call sms.read(index) to read the message:
    sms.read(messageIndex);
    
    Serial.print(F("Reading message ID: "));
    Serial.println(messageIndex);
    // Print the sending phone number:
    Serial.print(F("SMS From: "));
    Serial.println(sms.getSender());
    // Print the receive timestamp:
    Serial.print(F("SMS Date: "));
    Serial.println(sms.getDate());
    // Print the contents of the message:
    Serial.print(F("SMS Message: "));
    Serial.println(sms.getMessage());
    
    int sendValue = -1;
    int pinValue = -1;
    // Look through each of our pin reports, to see if the
    // contents of this message match what we're looking for
    for (int i = 0; i < PIN_REPORT_COUNT; i++)
    {
      // strcmp() compares two strings. If they're equal it
      // returns 0.
      if (strcmp(sms.getMessage(), 
          pinReportArray[i]->inputString) == 0)
      {
        pinValue = pinReportArray[i]->pin;
        if (pinReportArray[i]->isAnalogPin)
          sendValue = analogRead(pinValue);
        else
          sendValue = digitalRead(pinValue);
        break;
      }
    }
    if (sendValue >= 0)
    {
      byte sendStatus = sendResponse(sendValue, pinValue);
      
      if (sendStatus)
      {
        // sms.deleteMessage(index) deletes the contents of
        // an SMS memory from the SIM card. Due to the
        // Arduino's memory limitations we can only store
        // the status of 255 messages in the library.
        // Delete them when you can to save space!
        Serial.println(F("Send succeeded! Deleting."));
        sms.deleteMessage(messageIndex);
      }
      else
      {
        Serial.print(F("Send fail. Error: "));
        Serial.println(sendStatus);
      }
    }
    
  }
  delay(1000);
}

int8_t sendResponse(int value, int pin)
{
  // Sending an SMS is a three-step process:
  // 1. sms.start(phoneNumber) - sms.start() indicates that
  // an SMS message is coming, and defines the destination
  // phone number.
  sms.start(sms.getSender());
  // 2. sms.print(), sms.println(), and sms.write() - Any (or
  // all of these functions can be used to add strings,
  // integers, or other values to the body of an SMS.
  sms.print("Pin ");
  sms.print(pin);
  sms.print(": ");
  sms.print(value);
  // 3. sms.send() - After you've written the body of the
  // message, use sms.send() to ship it off. This function
  // can take a couple seconds to return successfully.
  // A positive value returned means the send was successful.
  // A negative value returned means the send failed.
  int8_t sendStatus = sms.send();
  
  return sendStatus;
}

void setupIOPins()
{
  for (int i = 0; i < PIN_REPORT_COUNT; i++)
  {
    pinMode(pinReportArray[i]->pin, INPUT);
  }
}

void serialTrigger()
{
  Serial.println(F("Send some serial to start"));
  while (!Serial.available())
    ;
  Serial.read();
}