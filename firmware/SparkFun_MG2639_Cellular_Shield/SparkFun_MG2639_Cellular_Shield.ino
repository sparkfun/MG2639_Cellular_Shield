/*
 Library and example code for the SparkFun MG2639 Cellular shield.
 By: Nathan Seidle
 SparkFun Electronics
 Date: October 20th, 2014
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
 
The SparkFun MG2639 Cellular shield is an Arduino compatible shield that gives the user access to the MG2639_V3 GSM+GPS module.
 
 The shield is wired in the following way:
 
 Arduino pin RX -> MG2639 TX
 TX -> MG2639 RX
 D2 -> TX (enable with solder jumper SJ3)
 D3 -> RX (enable with solder jumper SJ4)
 D4 -> GPS RX
 D5 -> GPS TX
 D6 -> Cell Reset (pull down 500ms minimum to reset module)
 D7 -> On/Off (Provide 2-5s low pulse to turn on/off module)
 
 NOTE: 
 This shield draws more than most usb ports can provide. Apply external power to prevent erratic rebooting.
 
 TODO:
 Get bulletproof boot sequence with ATOK testing, baud rate setting, and AT&F if nescessary
 
 Solder Jumpers
 SJ1 -> Open to disconnect gps power from board power.
 SJ2 -> Open to free up GPIO pin. If opened, you have to reboot via boot button.
 SJ3 -> Select between hardware UART and software UART. Default = hardware
 SJ4 -> Select between hardware UART and software UART. Default = hardware
 SJ5 -> Select between 3.3v and 5v. Default = 3.3v
 SJ6 -> Cut to power board exclusively from battery.
 
 */

//GPIO declarations
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

byte cellRX = 2; //Listens to the cell module
byte cellTX = 3; //Transmit to the cell module
byte gpsRX = 4; //Listens to the GPS module inside the MG2639
byte gpsTX = 5; //Transmit to the GPS module inside the MG2639
byte cellReset = 6;
byte cellOnOff = 7;

byte trigger = 12; //Used for triggering the logic analyzer

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include <SoftwareSerial.h>
SoftwareSerial MG2639(cellRX, cellTX); //RX, TX

//#include <AltSoftSerial.h>
//AltSoftSerial MG2639; //TX is on 9, RX is on 8

const byte MAX_INPUT = 50;
static char incomingText [MAX_INPUT];
static unsigned int input_pos = 0;
byte newlineCount = 0;

void setup()
{
  Serial.begin(9600);
  Serial.println("MG2639 example");

  //pinMode(cellRX, INPUT);
  //pinMode(cellTX, INPUT);
  
  pinMode(cellReset, OUTPUT);
  digitalWrite(cellReset, LOW); //Don't reset

  pinMode(cellOnOff, OUTPUT);
  digitalWrite(cellOnOff, LOW); //Don't turn on at this time
  
  pinMode(trigger, OUTPUT);
  digitalWrite(trigger, LOW);

  //delay(1000);

  if(!initMG2639()) //Set baud rates and bring module online
  {      
    Serial.println("Module failed to respond. Hanging.");
    while(1);
  }
  Serial.println("Module online!");
  
  //MG2639.print("AT+CREG=2\r");
  //delay(100);

  boolean textSent = false;
  
  while(1)
  {
    int rssi = checkSignalStrength();
    Serial.print("RSSI: ");
    Serial.println(rssi, DEC);
  
    //String response = checkIMEI();
    //Serial.print("IMEI: ");
    //Serial.println(response);
    
    checkRegistration();
    
    delay(2000);
    
    if(rssi < 40 && textSent == false)
    {
      textSent = true;
      //sendTextMessage("Hallo world", "3032466316");
      
      digitalWrite(trigger, HIGH);
      
      String response;

      response = sendCommand("+ZPPPOPEN");
      Serial.print("Open GRPS connection: ");
      Serial.println(response);
      
      delay(2000); //ZIP open takes a while
      
      response = sendCommand("+ZIPSETUP=1,204.144.132.37,80");
      Serial.print("Setup IP: ");
      Serial.println(response);

      delay(2000); //ZIP open takes a while

      response = sendCommand("+ZIPSEND=1,18\n\r");
      MG2639.print("GET / HTTP/1.0\n\r\n\r");
      Serial.print("GPRS Status: ");
      Serial.println(response);

      
      
      while(1);





      /*response = sendCommand("+ZPPPSTATUS");
      Serial.print("GPRS Status: ");
      Serial.println(response);*/

      /*String response = sendCommand("+ZIPSETUP=1,data.sparkfun.com,8081");
      Serial.print("Port open: ");
      Serial.print(response);*/

      response = sendCommand("+CGATT=1");
      Serial.print("GPRS Attach: ");
      Serial.println(response);

//      response = sendCommand("+CGDCONT=1, \"IP\",\"CMNET\"");
      response = sendCommand("+CGDCONT=1, \"IP\",\"internet.t-mobile\"");
//      response = sendCommand("+CGDCONT=1, \"IP\",\"internet.t-mobile\"\r\r\nATD*99#"); //That connects?
      Serial.print("GPRS Connect: ");
      Serial.println(response);
      
      delay(1000);

      response = sendCommand("+CGACT=1,1");
      Serial.print("GPRS Activate: ");
      Serial.println(response);
      
      delay(1000); //Can take as long as 700ms

      /*response = sendCommand("+CGCLASS?");
      Serial.print("GPRS Class: ");
      Serial.println(response);*/

//      response = sendCommand("+ZIPSETUP=1,195.34.89.241,21"); //Test Ublox FTP
      response = sendCommand("+ZIPSETUP=1,54.86.132.254,80");
      Serial.print("Setup IP: ");
      Serial.println(response);

      response = sendCommand("+ZPPPSTATUS");
      Serial.print("GPRS Status: ");
      Serial.println(response);


      response = sendCommand("D*99#");      
//      response = sendCommand("D*99***1#");      
      Serial.print("GPRS Connect: ");
      Serial.println(response);
      
      delay(1000);

      MG2639.print("GET / HTTP/1.0\r\nHost: www.google.com User-Agent: Arduino\r\n\r\n");
      
      
      while(1);

      response = sendCommand("D*99#");      
//      response = sendCommand("D*99***1#");      
      Serial.print("GPRS Connect: ");
      Serial.println(response);
      
      //delay(4500);

      delay(100);
      
      MG2639.print("GET / HTTP/1.0\r\nHost: www.google.com User-Agent: Arduino\r\n\r\n");
      
      
      while(1);
      
      
      /*String strToSend = "https://data.sparkfun.com/input/QGoEWpOOw9snOKqbvxzA?private_key=JqnPm4MM5zujmlyM8p1J&test1=8";
      String strLen = String(strToSend.length());
      
      byte ctrlZ = 26;

      String strToSend2 = "AT+ZIPSEND=8081," + strLen + "\r" + strToSend + ctrlZ;
      Serial.print("Test print: ");
      Serial.println(strToSend2);
            
      delay(100);
      //MG2639.print(strToSend2);

      delay(100);*/
      
    }
  }
  

}

void loop()
{


}

//Sends a message to a phone number
//Returns true if successful
//Generally, text messages have a 160 character limit

boolean sendTextMessage(String message, String phoneNumber)
{
  //Put us in text SMS mode
  String response = sendCommand("+CMGF=1");
  Serial.print("CMGF: ");
  Serial.println(response);

  if(response != "OK") return false; //Bail on error
  
  byte ctrlZ = 26;
  
  String toSend = "+CMGS=\"" + phoneNumber + "\"\r" + message + (char)ctrlZ;
  
  Serial.print("toSend: ");
  Serial.println(toSend);

  response = sendCommand(toSend);
  Serial.print("message: ");
  Serial.println(response);

  
}

//Checks network registration
//Example response = "1, 0", or "<mode>, <stat>"
//Returns the status
//0: Not logged on
//1: Logged on
//2: Not logged on, searching for BS(?)
//4: Unknown code
//5: Logged on, roaming
byte checkRegistration()
{
  String response = sendCommand("+CREG?");
  
  Serial.print("CREG: ");
  Serial.println(response);  
}

//Returns the module unique International Mobile Station Equipment Identity
String checkIMEI()
{
  return(sendCommand("+GSN"));
}

//Sends the standard AT command and looks for OK response
boolean checkATOK()
{
  //Send no command, just AT will be sent
  if(sendCommand("") == "OK") return true;

  return false;
}

//Checks the current reception strength of the cellular signal
//Example response is "3, 99" where 3 is the RSSI and 99 is bit the rate (99 = network unavailable)
int checkSignalStrength()
{
  String response = sendCommand("+CSQ");

  //Serial.print("response: ");
  //Serial.println(response);

  String rssiStr, berStr;

  byte commaPosition = response.indexOf(',');
  if(commaPosition != -1)
  {
    rssiStr = response.substring(0, commaPosition);
    berStr = response.substring(commaPosition + 1, response.length());
  }
  else
  {
    //Serial.println("Error: No comma found");
    return(-2);
  }

  int rssi = rssiStr.toInt();
  int ber = berStr.toInt();

  //Serial.print("rssi: ");
  //Serial.println(rssi);
  //Serial.print("ber: ");
  //Serial.println(ber);

  //An RSSI of 99 means the network is unavailable
  return(rssi);
}

//This sends a command and checks that the response is valid (not error)
//This function is not pretty but it works. Improvments are welcomed.
//For each AT command sent, two to three reponses come back. See ATQuery-Response_Example.png for more info

//Response example: "AT+CSQ\r\r\n+CSQ: 4, 99\r\n\r\nOK\r\n"
//We need to strip off the echoed command, check that we got an OK on the 3rd chunk
//And return the 2nd chunk

//Chunk 1: copy/echo of the command
//Chunk 2: The response data or OK or ERROR
//Chunk 3: OK or nothing

#define ERROR_NOCHARACTERS  "NOCHAR"
#define ERROR_COMMAND_STRING_MISMATCH "MISMATCH"
#define ERROR_RESPONSE2  "REPONSE2ERROR"
#define ERROR_RESPONSE3  "REPONSE3ERROR"

String sendCommand(String command)
{
  String firstChunk; //No command should be longer than 20
  String secondChunk; //Not sure but responses can be large
  String thirdChunk; //Should always be 'OK' or empty

  boolean breakFlag = false;
  int newlineCount = 0;

  String strToSend = "AT" + command + "\r"; //Commands get sent with *only* a \r.
  //Adding a \n will cause the parser to break

  Serial.print(F("Sending command: "));
  Serial.println(strToSend);

  while(MG2639.available()) MG2639.read(); //Remove everything in the buffers
  
  MG2639.print(strToSend); //Send this string to the module

  for(byte x = 0 ; x < 100 ; x++)
  {
    if(MG2639.available()) break; //Wait until we have some characters
    delay(1);
  }
    
  delay(55); //We need a few ms at 9600 to get. 15ms works well

  while(breakFlag == false)
  {
    if(MG2639.available() == false) return(ERROR_NOCHARACTERS); //This shouldn't happen

    char incoming = MG2639.read();

    switch(incoming)
    {
    case '\r':
      //We're done!
      breakFlag = true;
      firstChunk += incoming;
      break;

    default:
      firstChunk += incoming;
      break;
    }
  }

  //Serial.print("Chunk1: ");
  //Serial.println(firstChunk);

  //First test
  if(firstChunk != strToSend)
  {
    Serial.print("Chunk1: ");
    Serial.println(firstChunk);
    return(ERROR_COMMAND_STRING_MISMATCH);
  }

  //Reset the variables
  breakFlag = false;
  newlineCount = 0;

  while(breakFlag == false)
  {
    if(MG2639.available() == false) return(ERROR_NOCHARACTERS); //This shouldn't happen

    char incoming = MG2639.read();

    switch(incoming)
    {
    case '\r':
      //Ignore it
      break;

    case '\n':
      newlineCount++;

      if(newlineCount == 1)
      {
        //Ignore it          
      }
      else if(newlineCount == 2)
      {
        //We're done!
        breakFlag = true;
        secondChunk += incoming;
      }
      break;

    default:
      secondChunk += incoming;
      break;
    }
  }

  //Serial.print("Chunk2: ");
  //Serial.println(secondChunk);

  //Second test
  if(secondChunk == "OK\n")
  {
    //We're ok, this command just doesn't return any data
    return("OK"); //Ignore trailing \n
  }
  else if (secondChunk == "ERROR")
  {
    //This might be an error
    return(ERROR_RESPONSE2);
  }

  //Reset the variables
  breakFlag = false;
  newlineCount = 0;

  while(breakFlag == false)
  {
    if(MG2639.available() == false) return(ERROR_NOCHARACTERS); //This shouldn't happen

    char incoming = MG2639.read();

    switch(incoming)
    {
    case '\r':
      //Ignore it
      break;

    case '\n':
      newlineCount++;

      if(newlineCount == 1)
      {
        //Ignore it          
      }
      else if(newlineCount == 2)
      {
        //We're done!
        breakFlag = true;
        //thirdChunk += incoming; //Ignore the final \n
      }
      break;

    default:
      thirdChunk += incoming;
      break;
    }
  }

  if(thirdChunk == "OK")
  {
    //We're so good!

    command.replace("?", ""); //Some commands (+CREG?) use ? but reponses do not have them. This removes the ? from the command

    //Now remove the original command from the response. 
    //For example: +CSQ: 4, 99 should become " 4, 99"
    secondChunk.replace(command + ":", "");

    secondChunk.trim(); //Get rid of any leading white space

    return(secondChunk); //Report this chunk to the caller
  }
  else
  {
    //This is bad. Probably 'Error'
    Serial.print("Chunk 3: ");
    Serial.print(thirdChunk);
    return(ERROR_RESPONSE3);
  }

}

//This transmits the baud rate command at a few different baud rates in
//an attempt to get module down to 9600
//Returns turn if module successfully responded.
boolean initMG2639(void)
{
  //Quick test to see if module is already on and responding to AT commands
  MG2639.begin(9600);
  for(int x = 0 ; x < 5 ; x++)
  {
    if(checkATOK()) return(true);
    delay(100);
  }
  MG2639.end(); //That failed
  
  Serial.println("Turning on module");
  digitalWrite(cellOnOff, HIGH); //Turn on module
  delay(3000);
  digitalWrite(cellOnOff, LOW); //Leave module on

  Serial.println("Module should now be on");  

  MG2639.begin(115200);

  delay(10);

  MG2639.print("AT+IPR=9600\r\n"); //Set baud rate to 9600bps

  delay(10);

  MG2639.end(); //Hang up and go to 9600bps

  MG2639.begin(9600);

  delay(10);

  //Try sending AT 5 times before giving up
  for(int x = 0 ; x < 5 ; x++)
  {
    if(checkATOK()) return(true);
    delay(100);
  }

  return(false);
}

