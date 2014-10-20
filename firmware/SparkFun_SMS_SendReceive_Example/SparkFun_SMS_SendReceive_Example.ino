/*
 Example showing to send a text message and how to receive one
 By: Nathan Seidle
 SparkFun Electronics
 Date: October 12th, 2014
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

 This will send a text message to the cell phone number of your choice. This will give you the phone number of
 the SIM card in your posession. It will also scan for incoming text messages and if you send the module the message
 "turn off water" the example will respond with 'Turning off water!' and you can control GPIOs and such.
 
 Based on examples from www.open-electronics.org
 
 To turn off the extra print statemets comment out DEBUG_ON inside of GSM.h
 
 This assumes the cellular shield is connected to an Uno and has the software serial pins connected to pins 2 and 3
 To change pins for Software Serial see GSM.cpp

*/


#include "SIM900.h"
#include <SoftwareSerial.h>

#include "sms.h"
SMSGSM sms;


boolean module_connected = false;

void setup()
{
  Serial.begin(9600);
  Serial.println("Send and receive SMS example");

  //Start configuration of shield with baudrate.
  //For http uses it is recommended to use 4800 or slower.
  Serial.println("Starting module");
  if (gsm.begin(2400))
  {
    Serial.println("Module online!");
    module_connected = true;
  } 
  else 
  {
    Serial.println("Module failed to connect. Check that you have external power.");
  }

  if(module_connected) {
    //Send an SMS after module connects
    //if (sms.SendSMS("3032850979", "Hello cellular world"))
    //  Serial.println("\nSMS sent OK");
  }

}

void loop()
{
  if(module_connected) {
    //Read if there are messages on SIM card and print them.

    char position;
    char phone_number[20]; // array for the phone number string
    char sms_text[100];

    position = sms.IsSMSPresent(SMS_UNREAD);
    if (position) {
      // read new SMS
      sms.GetSMS(position, phone_number, sms_text, 100);
      // now we have phone number string in phone_num
      // and SMS text in sms_text
      Serial.print("From: ");
      Serial.println(phone_number);

      Serial.print("Message: ");
      Serial.println(sms_text);
      
      //Check to see if this is a message we understand
      if(strstr(sms_text, "turn off water"))
      {
        //We received a valid message
        Serial.println("Turning off the water!!");
        //Control valves and things here
      }
    }

    delay(1000);
  }
}
