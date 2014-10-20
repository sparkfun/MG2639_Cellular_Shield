#ifndef MG2639_COM_H
#define MG2639_COM_H
#include <SoftwareSerial.h>
#include "HWSerial.h"
#include "GSM.h"

//Module specific AT commands for MG2639
#define SET_GPRS_SERVICE    "AT+CGATT"
#define BRING_UP_CONNECTION "AT+CIICR"
#define SET_APN             "AT+CSTT"
#define DEACTIVATE_GPRS_CONNECTION  "AT+CIPSHUT"
#define CONFIGURE_AS_SERVER "AT+CIPSERVER"

#define OPEN_TCP_CONNECTION	"AT+ZPPPOPEN"
#define CLOSE_TCP_CONNECTION    "AT+ZPPPCLOSE"
#define SETUP_TCP_CONNECTION    "AT+ZIPSETUP"
#define SEND_TCP_DATA       "AT+ZIPSEND"
#define LOCAL_IP_ADDRESS    "AT+ZIPGETIP"


class MG2639_COM : public virtual GSM {

private:
    int configandwait(char* pin);
    int setPIN(char *pin);
    int changeNSIPmode(char);

public:
    MG2639_COM();
	~MG2639_COM();
	int getCCI(char* cci);
	int getIMEI(char* imei);
	int sendSMS(const char* to, const char* msg);
	boolean readSMS(char* msg, int msglength, char* number, int nlength);
	boolean readCall(char* number, int nlength);
	boolean call(char* number, unsigned int milliseconds);
	char forceON();
	virtual int read(char* result, int resultlength);
    virtual int skimRead(char* buffer, int buffersize, char* wordToLookFor);

	virtual uint8_t read();
	virtual int available();
	int readCellData(int &mcc, int &mnc, long &lac, long &cellid);
	void SimpleRead();
	void WhileSimpleRead();
	void SimpleWrite(char *comm);
	void SimpleWrite(char const *comm);
	void SimpleWrite(int comm);
	void SimpleWrite(const __FlashStringHelper *pgmstr);
	void SimpleWriteln(char *comm);
	void SimpleWriteln(char const *comm);
	void SimpleWriteln(const __FlashStringHelper *pgmstr);
	void SimpleWriteln(int comm);
};

extern MG2639_COM gsm;

#endif

