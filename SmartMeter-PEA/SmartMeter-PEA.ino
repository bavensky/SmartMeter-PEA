#include "TEE_UC20.h"
#include "SoftwareSerial.h"
#include <AltSoftSerial.h>
#include "call.h"
#include "sms.h"
#include "internet.h"
#include "File.h"
#include "http.h"
#include <DS1307RTC.h>
#include <TimeLib.h>
#include <SPI.h>
#include <SD.h>
#include "EmonLib.h"
#include <string.h>
#include "max6675.h"



/*###   GSM Variable    ###*/
INTERNET net;
UC_FILE file;
HTTP http;

//SIM TRUE  internet
#define APN "internet" // AIS
#define USER ""
#define PASS ""

AltSoftSerial mySerial;

void debug(String data)
{
  Serial.println(data);
}
void data_out(char data)
{
  Serial.write(data);
}

/*###   Current Variable    ###*/
#define ct1 A0
#define ct2 A1
#define ct3 A2

EnergyMonitor emon1;
EnergyMonitor emon2;
EnergyMonitor emon3;

double Irms1, Irms2, Irms3;
double Vrms1, Vrms2, Vrms3;

/*###   MAX6675 Variable    ###*/
int ktcSO = 8;
int ktcCS = 9;
int ktcCLK = 11;
MAX6675 ktc(ktcCLK, ktcCS, ktcSO);



/*###   Normal Variable    ###*/
unsigned long prevTime = 0;


/*###   Void loop    ###*/
void up2clound(String data1, String data2, String data3, String data4, String data5, String data6, String data7) {
  Serial.println(F("Start HTTP"));
  http.begin(1);
  Serial.println(F("Send HTTP GET"));
  http.url("http://api.thingspeak.com/update?api_key=6C0IJZX24FXU98BV&field1=" + data1 + "&field2="
           + data2 + "&field3=" + data3 + "&field4=" + data4 + "&field5=" + data5 + "&field6=" + data6 + "&field7=" + data7);
  Serial.println(http.get());
  delay(20000);
  
}

/*###   Void setup    ###*/
void setup()  {
  Serial.begin(9600);

  // gsm setup
  gsm.begin(&Serial1, 9600);  // use hardware Serial1
  gsm.Event_debug = debug;
  Serial.println(F("GSM Power ON"));
  gsm.PowerOn();
  while (gsm.WaitReady()) {}
  Serial.print(F("GetOperator --> "));
  Serial.println(gsm.GetOperator());
  Serial.print(F("SignalQuality --> "));
  Serial.println(gsm.SignalQuality());
  Serial.println(F("Disconnect net"));
  net.DisConnect();
  Serial.println(F("Set APN and Password"));
  net.Configure(APN, USER, PASS);
  Serial.println(F("Connect net"));
  net.Connect();
  Serial.println(F("Show My IP"));
  Serial.println(net.GetIP());
  Serial.println(F("Finish initiate ..."));


  // Current: input pin, calibration.
  emon1.current(ct1, 115);
  emon2.current(ct2, 115);
  emon3.current(ct3, 115);


}



/*###   Void loop    ###*/
void loop()
{
  unsigned long currentTime = millis();
  
  Irms1 = emon1.calcIrms(1500);
  Irms2 = emon2.calcIrms(1500);
  Irms3 = emon3.calcIrms(1500);

  int tempe = ktc.readCelsius();


if(currentTime - prevTime >= 10000) {
  prevTime = currentTime;
  up2clound(String(Vrms1), String(Vrms2), String(Vrms3), String(Irms1), String(Irms2), String(Irms3), String(tempe));
}
  
  
}
