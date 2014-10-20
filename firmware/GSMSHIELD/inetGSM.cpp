#include "inetGSM.h"
#define _GSM_CONNECTION_TOUT_ 5
#define _TCP_CONNECTION_TOUT_ 20
#define _GSM_DATA_TOUT_ 10

/*
    No function description included
    Guessed:
    
    Given a server, port and path, open up a connection and do an HTTP get
    
    Stores result and resultlength in returned arrays
    
return: gsm.read() success or failure
*/
int InetGSM::httpGET(const char* server, int port, const char* path, char* result, int resultlength)
{
//This function hasn't been reviewed for compatibility with MG2639 module
     boolean connected = false;
     byte number_of_attempts = 0;
     int length_write;
     char end_c[2];
     end_c[0] = 0x1a;
     end_c[1] = '\0';

     /*
     Status = ATTACHED.
     if(gsm.getStatus()!=GSM::ATTACHED)
       return 0;
     */
    while(number_of_attempts < 3) {
        if(!connectTCP(server, port)) {
#ifdef DEBUG_ON
            Serial.println("DB:NOT CONN");
#endif
            number_of_attempts++;
        } else {
            connected = true;
            number_of_attempts = 3;
        }
    }

    if(!connected) return 0;

    gsm.SimpleWrite("GET ");
    gsm.SimpleWrite(path);
    gsm.SimpleWrite(" HTTP/1.0\r\nHost: ");
    gsm.SimpleWrite(server);
    gsm.SimpleWrite("\r\n");
    gsm.SimpleWrite("User-Agent: Arduino");
    gsm.SimpleWrite("\r\n\r\n");
    gsm.SimpleWrite(end_c);

    switch(gsm.WaitResp(10000, 10, "SEND: OK")) {
    case RX_TMOUT_ERR:
        return 0;
        break;
    case RX_FINISHED_STR_NOT_RECV:
        return 0;
        break;
    }


    delay(50);
#ifdef DEBUG_ON
    Serial.println("DB:SENT");
#endif
    int res = gsm.read(result, resultlength);

    //gsm.disconnectTCP();

    //int res=1;
    return res;
}

/*
    No function description included
    Guessed:
    
    Given a server, port and path, and parameters open up a connection and do an HTTP post
    
    Stores result and resultlength in returned arrays
    
return: gsm.read() success or failure
*/
//int InetGSM::httpPOST(const char* server, int port, const char* path, const char* parameters, char* result, int resultlength)
int InetGSM::httpPOST(const char* server, int port, const char* path, char* result, int resultlength)
{
    boolean connected = false;
    byte number_of_attempts = 0;
    char itoaBuffer[8];
    int num_char;
    char end_c[2];
    end_c[0] = 0x1a;
    end_c[1] = '\0';

    while(number_of_attempts < 3) {
        if(!connectTCP(server, port)) {
#ifdef DEBUG_ON
            Serial.println("DB:NOT CONN");
#endif
            number_of_attempts++;
        } else {
            connected=true;
            number_of_attempts = 3;
        }
    }

    if(!connected) return 0;

    //Assume path does not have double trailing \r\n\r\n
    //"POST " + path + " HTTP/1.1\r\nHost: " + server + "\r\n\r\n"
    //5 + path + 17 + server + 4
    int lengthOfString = 5 + strlen(path) + 17 + strlen(server) + 4;

	itoa(lengthOfString, itoaBuffer, 10); //Calculate length of path then convert to string

#ifdef DEBUG_ON
    Serial.print("Length of path: ");
    Serial.println(itoaBuffer);
#endif

    //Now that the TCP port is open, send data
	//Example: AT+ZIPSEND=1,110
    //What we will send next: 
    //GET /input/lzonOvxpqwUmEj8aR13o?private_key=ElnmvD2EePCvnxVya57g&temp=12 HTTP/1.1
    //Host: data.sparkfun.com
    //Length of data is 110 characters including the trailing \r\n\r\n
	gsm.SimpleWrite(SEND_TCP_DATA);
	gsm.SimpleWrite("=1,");
    gsm.SimpleWriteln(itoaBuffer); //Tell the module the length of string we are about to pass it

    switch(gsm.WaitResp(5000, 200, ">")) {
        case RX_TMOUT_ERR:
            return 0;
            break;
        case RX_FINISHED_STR_NOT_RECV:
            return 0;
            break;
    }

    gsm.SimpleWrite("POST ");
    gsm.SimpleWrite(path);
    gsm.SimpleWrite(" HTTP/1.1\r\nHost: ");
    gsm.SimpleWrite(server);
    gsm.SimpleWrite("\r\n\r\n");

    switch(gsm.WaitResp(10000, 10, "+ZIPSEND: OK")) {
    case RX_TMOUT_ERR:
        return 0;
        break;
    case RX_FINISHED_STR_NOT_RECV:
        return 0;
        break;
    }

    delay(50);
#ifdef DEBUG_ON
    Serial.println("DB:SENT");
#endif

    int res = gsm.skimRead(result, resultlength, "private");

    //gsm.disconnectTCP();

    return res;
}

int InetGSM::openmail(char* server, char* loginbase64, char* passbase64, char* from, char* to, char* subj)
{
//This function hasn't been reviewed for compatibility with MG2639 module
     boolean connected=false;
     int n_of_at=0;
     char end_c[2];
     end_c[0]=0x1a;
     end_c[1]='\0';

     while(n_of_at<3) {
          if(!connectTCP(server, 25)) {
#ifdef DEBUG_ON
               Serial.println("DB:NOT CONN");
#endif
               n_of_at++;
          } else {
               connected=true;
               n_of_at=3;
          }
     }

     if(!connected) return 0;

     delay(100);
     gsm.SimpleWrite("HELO ");
     gsm.SimpleWrite(server);
     gsm.SimpleWrite("\n");
     gsm.SimpleWrite(end_c);
     gsm.WaitResp(5000, 100, "OK");
     if(!gsm.IsStringReceived("SEND OK"))
          return 0;
     delay(500);
     gsm.WaitResp(5000, 100);

     delay(100);
     gsm.SimpleWriteln("AT+CIPSEND");
     switch(gsm.WaitResp(5000, 200, ">")) {
     case RX_TMOUT_ERR:
          return 0;
          break;
     case RX_FINISHED_STR_NOT_RECV:
          return 0;
          break;
     }
     gsm.SimpleWrite("AUTH LOGIN\n");
     gsm.SimpleWrite(end_c);
     gsm.WaitResp(5000, 100, "OK");
     if(!gsm.IsStringReceived("OK"))
          return 0;
     delay(500);
     gsm.WaitResp(5000, 100);

     delay(100);
     gsm.SimpleWriteln("AT+CIPSEND");
     switch(gsm.WaitResp(5000, 200, ">")) {
     case RX_TMOUT_ERR:
          return 0;
          break;
     case RX_FINISHED_STR_NOT_RECV:
          return 0;
          break;
     }
     gsm.SimpleWrite(loginbase64);
     gsm.SimpleWrite("\n");
     gsm.SimpleWrite(end_c);
     gsm.WaitResp(5000, 100, "OK");
     if(!gsm.IsStringReceived("OK"))
          return 0;
     delay(500);
     gsm.WaitResp(5000, 100);

     delay(100);
     gsm.SimpleWriteln("AT+CIPSEND");
     switch(gsm.WaitResp(5000, 200, ">")) {
     case RX_TMOUT_ERR:
          return 0;
          break;
     case RX_FINISHED_STR_NOT_RECV:
          return 0;
          break;
     }
     gsm.SimpleWrite(passbase64);
     gsm.SimpleWrite("\n");
     gsm.SimpleWrite(end_c);
     gsm.WaitResp(5000, 100, "OK");
     if(!gsm.IsStringReceived("OK"))
          return 0;
     delay(500);
     gsm.WaitResp(5000, 100);


     delay(100);
     gsm.SimpleWriteln("AT+CIPSEND");
     switch(gsm.WaitResp(5000, 200, ">")) {
     case RX_TMOUT_ERR:
          return 0;
          break;
     case RX_FINISHED_STR_NOT_RECV:
          return 0;
          break;
     }
     gsm.SimpleWrite("MAIL From: <");
     gsm.SimpleWrite(from);
     gsm.SimpleWrite(">\n");
     gsm.SimpleWrite(end_c);
     gsm.WaitResp(5000, 100, "OK");
     if(!gsm.IsStringReceived("OK"))
          return 0;
     delay(500);
     gsm.WaitResp(5000, 100, "");

     delay(100);
     gsm.SimpleWriteln("AT+CIPSEND");
     switch(gsm.WaitResp(5000, 200, ">")) {
     case RX_TMOUT_ERR:
          return 0;
          break;
     case RX_FINISHED_STR_NOT_RECV:
          return 0;
          break;
     }
     gsm.SimpleWrite("RCPT TO: <");
     gsm.SimpleWrite(to);
     gsm.SimpleWrite(">\n");
     gsm.SimpleWrite(end_c);
     gsm.WaitResp(5000, 100, "OK");
     if(!gsm.IsStringReceived("OK"))
          return 0;
     delay(500);
     gsm.WaitResp(5000, 100, "");

     delay(100);
     gsm.SimpleWriteln("AT+CIPSEND");
     switch(gsm.WaitResp(5000, 200, ">")) {
     case RX_TMOUT_ERR:
          return 0;
          break;
     case RX_FINISHED_STR_NOT_RECV:
          return 0;
          break;
     }
     gsm.SimpleWrite("Data\n");
     gsm.SimpleWrite(end_c);
     gsm.WaitResp(5000, 100, "OK");
     if(!gsm.IsStringReceived("OK"))
          return 0;
     delay(500);
     gsm.WaitResp(5000, 100, "");

     delay(100);
     gsm.SimpleWriteln("AT+CIPSEND");
     switch(gsm.WaitResp(5000, 200, ">")) {
     case RX_TMOUT_ERR:
          return 0;
          break;
     case RX_FINISHED_STR_NOT_RECV:
          return 0;
          break;
     }
     gsm.SimpleWrite("Subject: ");
     gsm.SimpleWrite(subj);
     gsm.SimpleWrite("\n\n");

     return 1;
}
int InetGSM::closemail()
{
//This function hasn't been reviewed for compatibility with MG2639 module
     char end_c[2];
     end_c[0]=0x1a;
     end_c[1]='\0';

     gsm.SimpleWrite("\n.\n");
     gsm.SimpleWrite(end_c);
     disconnectTCP();
     return 1;
}

/*
    No function description included
    Guessed:
    
    Given a APN, username and password, attach to the GPRS network
    
    Most of the time the username and password can be blank (provided automatically
    when cell module connects to cell network)
    
    If there is an error, close any connection and configure as not a server
    
return: 1 - the unit already had an IP, now GPRS connection is closed
        0 - IP address obtained successfully
*/
byte InetGSM::attachGPRS(char* apn, char* username, char* password)
{
//This function hasn't been reviewed for compatibility with MG2639 module
    delay(5000); //Why is this here?

    //gsm._tf.setTimeout(_GSM_DATA_TOUT_);	//Timeout for expecting modem responses.
    gsm.WaitResp(50, 50);

    gsm.SimpleWriteln(LOCAL_IP_ADDRESS); //Get local IP address

    if(gsm.WaitResp(5000, 50, "ERROR") != RX_FINISHED_STR_RECV) {
#ifdef DEBUG_ON
        Serial.println("DB:ALREADY HAVE AN IP");
#endif
        gsm.SimpleWriteln(CLOSE_TCP_CONNECTION); //Close connection
        gsm.WaitResp(5000, 50, "ERROR");
        delay(2000);

        gsm.SimpleWrite(CONFIGURE_AS_SERVER);
        gsm.SimpleWriteln("=0"); //Configure as *not* server
        gsm.WaitResp(5000, 50, "ERROR");

        return 1;
    } else {

#ifdef DEBUG_ON
        Serial.println("DB:STARTING NEW CONNECTION");
#endif

        gsm.SimpleWriteln(DEACTIVATE_GPRS_CONNECTION); //Shut down any current connection

        switch(gsm.WaitResp(500, 50, "SHUT OK")) {

        case RX_TMOUT_ERR:
            return 0;
            break;
        case RX_FINISHED_STR_NOT_RECV:
            return 0;
            break;
        }
#ifdef DEBUG_ON
        Serial.println("DB:SHUTTED OK");
#endif
        delay(1000);

        gsm.SimpleWrite(SET_APN); //Start task and set APN, user and pwd
        gsm.SimpleWrite("=\"");
        gsm.SimpleWrite(apn);
        gsm.SimpleWrite("\",\"");
        gsm.SimpleWrite(username);
        gsm.SimpleWrite("\",\"");
        gsm.SimpleWrite(password);
        gsm.SimpleWrite("\"\r");


        switch(gsm.WaitResp(500, 50, "OK")) {

        case RX_TMOUT_ERR:
            return 0;
            break;
        case RX_FINISHED_STR_NOT_RECV:
            return 0;
            break;
        }
#ifdef DEBUG_ON
        Serial.println("DB:APN OK");
#endif
        delay(5000);

        gsm.SimpleWriteln(BRING_UP_CONNECTION); //Bring up wireless connection

        switch(gsm.WaitResp(10000, 50, "OK")) {
        case RX_TMOUT_ERR:
            return 0;
            break;
        case RX_FINISHED_STR_NOT_RECV:
            return 0;
            break;
        }
#ifdef DEBUG_ON
        Serial.println("DB:CONNECTION OK");
#endif

        delay(1000);


        gsm.SimpleWriteln(LOCAL_IP_ADDRESS); //Get the current IP address
        if(gsm.WaitResp(5000, 50, "ERROR") != RX_FINISHED_STR_RECV) {
#ifdef DEBUG_ON
            Serial.println("DB:ASSIGNED AN IP");
#endif
            gsm.setStatus(gsm.ATTACHED);
            return 1;
        }
#ifdef DEBUG_ON
        Serial.println("DB:NO IP AFTER CONNECTION");
#endif
        return 0;
    }
}

/*
    No function description included
    Guessed:
    
    Detach from GPRS
    
return: 0 - the unit was already detached or there was an error
        1 - unit is now detached
*/
byte InetGSM::dettachGPRS()
{
//This function hasn't been reviewed for compatibility with MG2639 module
    if (gsm.getStatus() == gsm.IDLE) return 0;

    //gsm._tf.setTimeout(_GSM_CONNECTION_TOUT_);

    //_cell.flush();

    //GPRS dettachment.
    gsm.SimpleWrite(SET_GPRS_SERVICE);
    gsm.SimpleWriteln("=0"); //Detach from GPRS service
    if(gsm.WaitResp(5000, 50, "OK") != RX_FINISHED_STR_NOT_RECV) {
        gsm.setStatus(gsm.ERROR);
        return 0;
    }
    delay(500);

    // Commented in initial trial code!!
    //Stop IP stack.
    //_cell << "AT+WIPCFG=0" <<  _DEC(cr) << endl;
    //	if(!gsm._tf.find("OK")) return 0;
    //Close GPRS bearer.
    //_cell << "AT+WIPBR=0,6" <<  _DEC(cr) << endl;

    gsm.setStatus(gsm.READY);
    return 1;
}

/*
    connectTCP
    
    Given a server and a port number, open up a TCP connection
    
return: 1 - Ready and TCP connected, for a few seconds
        0 - Error
*/
int InetGSM::connectTCP(const char* server, int port)
{
//This function hasn't been reviewed for compatibility with MG2639 module
    //gsm._tf.setTimeout(_TCP_CONNECTION_TOUT_);

    //Status = ATTACHED.
    //if (getStatus()!=ATTACHED)
    //return 0;

    //_cell.flush();

    
    //Open TCP connection
	//For the MG2639 we first open or allow a connection and then set it up with the IP
	//Example: AT+ZPPPOPEN
	gsm.SimpleWriteln("AT+ZPPPOPEN");
    switch(gsm.WaitResp(1000, 200, "OK")) {
        case RX_TMOUT_ERR:
            return 0;
            break;
        case RX_FINISHED_STR_NOT_RECV:
            return 0;
            break;
    }

    //Check to see if we are already connected to TCP
	/*gsm.SimpleWriteln("AT+ZPPPSTATUS");
    switch(gsm.WaitResp(1000, 200, "OK")) {
        case RX_TMOUT_ERR:
            return 0;
            break;
        case RX_FINISHED_STR_NOT_RECV:
            return 0;
            break;
    }
    if(gsm.IsStringReceived("ESTABLISHED"))
    {
        Serial.println("TCP already established");
        return 1;
    }*/
    
    //Setup TCP connection
	//Example: AT+ZIPSETUP=1,54.86.132.254,80
	gsm.SimpleWrite(SETUP_TCP_CONNECTION);
    gsm.SimpleWrite("=1,");
    gsm.SimpleWrite(server);
    gsm.SimpleWrite(",");
    gsm.SimpleWriteln(port);
	
    switch(gsm.WaitResp(3000, 200, "OK")) {
        case RX_TMOUT_ERR:
            return 0;
            break;
        case RX_FINISHED_STR_NOT_RECV:
            return 0;
            break;
    }
    
#ifdef DEBUG_ON
    Serial.println("DB:RECVD CMD");
#endif
    if (!gsm.IsStringReceived("+ZIPSETUP:CONNECTED")) {
        switch(gsm.WaitResp(15000, 200, "OK")) {
            case RX_TMOUT_ERR:
                return 0;
                break;
            case RX_FINISHED_STR_NOT_RECV:
                return 0;
                break;
        }
    }

#ifdef DEBUG_ON
    Serial.println("TCP Connection Established");
#endif

    return 1;
}

int InetGSM::disconnectTCP()
{
//This function hasn't been reviewed for compatibility with MG2639 module
     //Status = TCPCONNECTEDCLIENT or TCPCONNECTEDSERVER.
     /*
     if ((getStatus()!=TCPCONNECTEDCLIENT)&&(getStatus()!=TCPCONNECTEDSERVER))
        return 0;
     */
     //gsm._tf.setTimeout(_GSM_CONNECTION_TOUT_);


     //_cell.flush();

     //Switch to AT mode.
     //_cell << "+++" << endl;

     //delay(200);

     //Close TCP client and deact.
     gsm.SimpleWriteln("AT+CIPCLOSE");

     //If remote server close connection AT+QICLOSE generate ERROR
     /*if(gsm._tf.find("OK"))
     {
       if(getStatus()==TCPCONNECTEDCLIENT)
         gsm.setStatus(ATTACHED);
       else
         gsm.setStatus(TCPSERVERWAIT);
       return 1;
     }
     gsm.setStatus(ERROR);

     return 0;    */
     if(gsm.getStatus()==gsm.TCPCONNECTEDCLIENT)
          gsm.setStatus(gsm.ATTACHED);
     else
          gsm.setStatus(gsm.TCPSERVERWAIT);
     return 1;
}

int InetGSM::connectTCPServer(int port)
{
//This function hasn't been reviewed for compatibility with MG2639 module
     /*
       if (getStatus()!=ATTACHED)
          return 0;
     */
     //gsm._tf.setTimeout(_GSM_CONNECTION_TOUT_);

     //_cell.flush();

     // Set port

     gsm.SimpleWrite("AT+CIPSERVER=1,");
     gsm.SimpleWriteln(port);
     /*
       switch(gsm.WaitResp(5000, 50, "OK")){
     	case RX_TMOUT_ERR:
     		return 0;
     	break;
     	case RX_FINISHED_STR_NOT_RECV:
     		return 0;
     	break;
       }

       switch(gsm.WaitResp(5000, 50, "SERVER")){ //Try SERVER OK
     	case RX_TMOUT_ERR:
     		return 0;
     	break;
     	case RX_FINISHED_STR_NOT_RECV:
     		return 0;
     	break;
       }
     */
     //delay(200);

     return 1;

}

boolean InetGSM::connectedClient()
{
//This function hasn't been reviewed for compatibility with MG2639 module
     /*
     if (getStatus()!=TCPSERVERWAIT)
        return 0;
     */

     gsm.SimpleWriteln("AT+CIPSTATUS");
     // Alternative: AT+QISTAT, although it may be necessary to call an AT
     // command every second,which is not wise
     /*
     switch(gsm.WaitResp(1000, 200, "OK")){
     case RX_TMOUT_ERR:
     	return 0;
     break;
     case RX_FINISHED_STR_NOT_RECV:
     	return 0;
     break;
     }*/
     //gsm._tf.setTimeout(1);
     if(gsm.WaitResp(5000, 50, "CONNECT OK")!=RX_FINISHED_STR_RECV) {
          gsm.setStatus(gsm.TCPCONNECTEDSERVER);
          return true;
     } else
          return false;
}
