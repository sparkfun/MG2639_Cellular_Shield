/*
 Example showing to send a text message and how to receive one using the MG2639
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

#include "MG2639.h"
#include <SoftwareSerial.h>

#include "inetGSM.h"
InetGSM inet;

boolean module_connected = false;

void setup()
{
  Serial.begin(9600);
  Serial.println("Example posting to SparkFun's data channel");
  
  //pinMode(2, INPUT);
  //pinMode(3, INPUT);
  //while(1);
  
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

#define PUBLIC_KEY "lzonOvxpqwUmEj8aR13o"
#define PRIVATE_KEY  "ElnmvD2EePCvnxVya57g"
    
    char* server = "54.86.132.254"; //data.sparkfun.com
    int port = 80; //For HTTP action, use port 80
    
    //Put together the HTML link we want to post to the datachannel
    //Example: /input/lzonOvxpqwUmEj8aR13o?private_key=ElnmvD2EePCvnxVya57g&temp=12
    char path[100];
    strcpy(path,"/input/");
    strcat(path, PUBLIC_KEY);
    strcat(path, "?private_key=");
    strcat(path, PRIVATE_KEY);
    strcat(path, "&temp=15");
    //"temp=12";
    
    Serial.print("path: ");
    Serial.println(path);

    int result_len = 100;
    char results[result_len];

    if(inet.httpPOST(server, port, path, results, result_len)) //Push this information to the data channel
    {
      Serial.print("Result len: ");
      Serial.println(result_len, DEC);

      Serial.print("Result: ");
      Serial.println(results);
    }
    else
    {
      Serial.println("Error received");      
    }
    
    while(1);

    delay(1000);
  }
}
