/* Smart Meter PEA
   Author : Apiruk326@gmail.com
   Date   : 16/10/2017


   #Solution
   - Current meter (Irms)
   - Voltage meter  (Vrms)
   - Temperature (Max6675)
   - Send data to clound
   - 7 data display line graph


   #Pin connect
   Module               Arduino Mega

   - GSM module
   PWR                  11
   REF                  5v
   TX                   19
   RX                   18

   - CT sensor
   CT analog            A1
   CT analog            A2
   CT analog            A3

   - Single Phase Voltage sensor
   ZMPT analog          A4
   ZMPT analog          A5
   ZMPT analog          A6

   - MAX6675 Temperature
   SO                   D8
   CS                   D9
   CLK                  D10


*/



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


/*###   Voltage Variable    ###*/
#define SAMPLING 300  // จำนวนที่ต้องการวนอ่านค่า
#define VOFFSET 512   // 2.5V จาก 10Bit
#define pinADC1 A4    // เชื่อมต่อขา A4
#define pinADC2 A5    // เชื่อมต่อขา A5
#define pinADC3 A6    // เชื่อมต่อขา A6
#define AMPLITUDE 411.00 // Amplitude = 2V หรือ 411 ใน ADC 10 Bits
#define REAL_VAC 230  //กeหนดค่าที่อ่านได้จริงจาก Multimeter
int adc_max, adc_min;
int adc_vpp;
double Vrms1, Vrms2, Vrms3;

// read VAC
double read_VAC(int pin) {
  int cnt;
  adc_max = 0;
  adc_min = 1024;

  // วน loop อ่านค่า ADC
  for (cnt = 0; cnt < SAMPLING; cnt++) {
    int adc = analogRead(pin); //อ่านค่า ADC
    //หาค่า max
    if (adc > adc_max) {
      adc_max = adc;
    }

    //หาค่า min
    if (adc < adc_min) {
      adc_min = adc;
    }
  }

  adc_vpp = adc_max - adc_min; //หาผลต่างของ input (Vpp)
  return map(adc_vpp, 0, AMPLITUDE, 0, REAL_VAC * 100) / 100.00; // แปลงค่าที่อ่านได้เป็น VAC

}



/*###   Current Variable    ###*/
#define ct1 A1    // เชื่อมต่อขา A1
#define ct2 A2    // เชื่อมต่อขา A2
#define ct3 A3    // เชื่อมต่อขา A3

EnergyMonitor emon1;
EnergyMonitor emon2;
EnergyMonitor emon3;

double Irms1, Irms2, Irms3;




/*###   MAX6675 Variable    ###*/
int ktcSO = 8;    // เชื่อมต่อขา D8
int ktcCS = 9;    // เชื่อมต่อขา D9
int ktcCLK = 10;  // เชื่อมต่อขา D10
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

  Vrms1 =  read_VAC(pinADC1);
  Vrms2 =  read_VAC(pinADC2);
  Vrms3 =  read_VAC(pinADC3);

  // sent data every 10 sec
  if (currentTime - prevTime >= 10000) {
    prevTime = currentTime;
    up2clound(String(Vrms1), String(Vrms2), String(Vrms3), String(Irms1), String(Irms2), String(Irms3), String(tempe));
  }


}
