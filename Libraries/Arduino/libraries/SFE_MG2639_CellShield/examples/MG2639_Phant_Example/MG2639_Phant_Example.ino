/************************************************************
MG2639_Phant_Example.h
MG2639 Cellular Shield library - Phant Posting Example
Jim Lindblom @ SparkFun Electronics
Original Creation Date: April 3, 2015
https://github.com/sparkfun/MG2639_Cellular_Shield

This example demonstrates how to use the GPRS and TCP 
functionality of the SparkFun MG2639 Cellular Shield library
to post sensor readings to a Phant stream on 
https://data.sparkfun.com

This sketch is set up to post to a publicly available stream
https://data.sparkfun.com/streams/DJjNowwjgxFR9ogvr45Q
Please don't abuse it! But feel free to post a few times to
verify the sketch works. If it fails, check the HTTP response
to make sure the post rate hasn't been exceeded.

This sketch also requires that the Phant Arduino library be
installed. You can download it from the GitHub repository:
https://github.com/sparkfun/phant-arduino

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
// This example also requires the Phant Arduino library.
// Download the library from our GitHub repo:
// https://github.com/sparkfun/phant-arduino
#include <Phant.h>

IPAddress myIP; // IPAddress to store the local IP

/////////////////////
// Phant Constants //
/////////////////////
// Phant detsination server:
const char server[] = "data.sparkfun.com";
// Phant public key:
const char publicKey[] = "DJjNowwjgxFR9ogvr45Q";
// Phant private key:
const char privateKey[] = "P4eKwGGek5tJVz9Ar84n";
// Create a Phant object, which we'll use from here on:
Phant phant(server, publicKey, privateKey);

void setup() 
{
  int status;
  Serial.begin(9600);
  
  // To turn the MG2639 shield on, and verify communication
  // always begin a sketch by calling cell.begin().
  status = cell.begin();
  if (status <= 0)
  {
    Serial.println(F("Unable to communicate with shield. Looping"));
    while(1)
      ;
  }
  
  // Then call gprs.open() to establish a GPRS connection.
  // gprs.open() can take upwards of 30 seconds to connect,
  // so be patient.
  while( gprs.open() <= 0)
  {
    delay(2000);
  }
  
  // Get our assigned IP address and print it:
  myIP = gprs.localIP();
  Serial.print(F("My IP address is: "));
  Serial.println(myIP);
  
  Serial.println(F("Press any key to post to Phant!"));
}

void loop()
{
  // If data has been sent over a TCP link:
  if (gprs.available())
  {  // Print it to the serial monitor:
    Serial.write(gprs.read());
  }
  // If a character has been received over serial:
  if (Serial.available())
  {
    // Post to Phant!
    postToPhant();
    // Then clear the serial buffer:
    while (Serial.available())
      Serial.read();
  }
}

void postToPhant()
{
  // Open GPRS. It's OK to call this if GPRS is already
  // open, it'll just return super-quickly.
  byte status = gprs.open();
  if (status <= 0)
  {
    Serial.print(F("Couldn't open GPRS"));
    return;
  }
  Serial.println(F("GPRS open!"));
  
  // Connect to the Phant server (data.sparkfun.com) on
  // port 80.
  status = gprs.connect(server, 80);
  if (status <= 0)
  {
    Serial.println(F("Error connecting."));
    return;
  }
  Serial.print(F("Connected to "));
  Serial.println(server);
  
  // Set up our Phant post using the Phant library. For
  // each field in the Phant stream we need to call
  // phant.add([field], value).
  // Value can be any data type, in this case we're only
  // using integers.
  phant.add(F("analog0"), analogRead(A0));
  phant.add(F("analog1"), analogRead(A1));
  phant.add(F("analog2"), analogRead(A2));
  phant.add(F("analog3"), analogRead(A3));
  phant.add(F("analog4"), analogRead(A4));
  phant.add(F("analog5"), analogRead(A5));
  // Storing fields in flash (F()) will save a good chunk
  // of RAM, which is very precious.
  
  Serial.println(F("Posting to Phant!"));
  // Encapsulate a phant.post() inside a gprs.print(). 
  // phant.post() takes care of everything in the HTTP header
  // including newlines.
  gprs.print(phant.post());
  
  /*
  // You can close here, but then won't be able to read any
  // responses from the server in the loop().
  status = gprs.close();
  if (status <= 0)
  {
    Serial.println(F("Unable to close GPRS. Looping"));
  }
  */
}
