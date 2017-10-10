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
#define ct4 A3
#define ct5 A8
#define ct6 A9

EnergyMonitor emon1;
EnergyMonitor emon2;
EnergyMonitor emon3;
EnergyMonitor emon4;
EnergyMonitor emon5;
EnergyMonitor emon6;

double Irms1, Irms2, Irms3, Irms4, Irms5, Irms6;


/*###   Void setup    ###*/
int ktcSO = 8;
int ktcCS = 9;
int ktcCLK = 11;

MAX6675 ktc(ktcCLK, ktcCS, ktcSO);


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
  emon4.current(ct4, 115);
  emon5.current(ct5, 115);
  emon6.current(ct6, 115);


}



/*###   Void loop    ###*/
void loop()
{
  Irms1 = emon1.calcIrms(1500);
  Irms2 = emon2.calcIrms(1500);
  Irms3 = emon3.calcIrms(1500);
  Irms4 = emon4.calcIrms(1500);
  Irms5 = emon5.calcIrms(1500);
  Irms6 = emon6.calcIrms(1500);


  int tempe = ktc.readCelsius();
  String data1 = String(random(0, 100));
  String data2 = String(random(0, 100));
  String data3 = String(random(0, 100));
  String data4 = String(random(0, 100));
  String data5 = String(random(0, 100));
  String data6 = String(random(0, 100));

  Serial.println(F("Start HTTP"));
  http.begin(1);
  Serial.println(F("Send HTTP GET"));
  http.url("http://api.thingspeak.com/update?api_key=6C0IJZX24FXU98BV&field1=" + data1 + "&field2="
           + data2 + "&field3=" + data3 + "&field4=" + data4 + "&field5=" + data5 + "&field6=" + data6);
  Serial.println(http.get());
  delay(20000);
}