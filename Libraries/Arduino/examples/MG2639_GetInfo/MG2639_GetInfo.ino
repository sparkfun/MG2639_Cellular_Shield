/************************************************************
MG2639_GetInfo.h
MG2639 Cellular Shield library - Get Information Example
Jim Lindblom @ SparkFun Electronics
Original Creation Date: April 3, 2015
https://github.com/sparkfun/MG2639_Cellular_Shield

This example demonstrates some of the simple information-
getting functions in the SparkFun MG2639 Cellular Shield
library.

Demonstrated functions include:
  cell.begin() - Initialize the cell shield
  cell.getInformation(manufacturer) - Get manufacturer,
    hardware, and software information of the module.
  cell.getIMI(imi) - Get IMI of module
  cell.getICCID(iccid) - Get ICCID of SIM card
  cell.getIMEI(imei) - Get IMEI of MG2639 module
  cell.getPhoneNumber(myPhone) - Get SIM cards phone number

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

char imi[16]; // Storage array for IMI 
char iccid[20]; // Storage array for ICCID
char imei[17]; // Storage array for IMEI
char myPhone[15]; // Storage array for your phone number
char manufacturer[64]; // Storage array for manufacturer info

void setup()
{
  // USB serial connection is used to print the information
  // we find.
  Serial.begin(9600);
  
  // serialTrigger() halts execution of the program until
  // any value is received over the serial link. Cell data
  // costs $, so we don't want to use it unless it's visible!
  serialTrigger();
  
  // This status variable will store the response error codes
  // of any functions we call.
  uint8_t status;
  
  // Every MG2639 sketch should begin with cell.begin().
  // This function will initialize the module, turn it on if
  // it's off, and verify communication.
  // If it's successful, it'll return a 1. If it can't
  // communicate with the module, it returns a 0.
  status = cell.begin();
  if (status <= 0)
  {
    Serial.println("Unable to communicate with shield. Looping");
    while(1)
      ;
  }
  
  // cell.getInformation() returns a big, long string with
  // information like the manufactuer and hardware version.
  // It requries a lot of space, and isn't all that useful
  // so you probably don't have to call it in your other
  // sketches.
  status = cell.getInformation(manufacturer);
  if (status > 0)
  {
    Serial.print("Manufacturer Info: ");
    Serial.println(manufacturer);
  }
  
  // cell.getIMI() returns the International Mobile
  // Identification (IMI or IMSI) of a SIM card. More info:
  // http://en.wikipedia.org/wiki/International_mobile_subscriber_identity
  status = cell.getIMI(imi);
  if (status > 0)
  {
    Serial.print("IMI: ");
    Serial.println(imi);
  }
  
  // cell.getICCID() returns the ICCID of your SIM card. A 
  // unique value that should also be engraved on your card:
  // http://en.wikipedia.org/wiki/Subscriber_identity_module#ICCID
  status = cell.getICCID(iccid);
  if (status > 0)
  {
    Serial.print("ICCID: ");
    Serial.println(iccid);
  }
  
  // cell.getIMEI() returns the International Mobile Station
  // Equipment Identity (IMEI) of your MG2639 module.
  // The IMEI is a unique number, specific to the module on
  // the cell shield.
  status = cell.getIMEI(imei);
  if (status > 0)
  {
    Serial.print("IMEI: ");
    Serial.println(imei);
  }
  
  // getPhoneNumber requires one parameter - a char array
  // with enough space to store a phone number (~15 bytes).
  // Upon return, cell.getPhoneNumber(myPhone) will return
  // a 1 if successful, and myPhone will contain a char array
  // of your module's phone number.
  status = cell.getPhoneNumber(myPhone);
  if (status > 0)
  { // If the function successfully returned, print the #:
    Serial.print("My phone number is: ");
    Serial.println(myPhone);
  }
}

void loop() 
{

}

void serialTrigger()
{
  Serial.println("Send some serial to start");
  while (!Serial.available())
    ;
  Serial.read();
}