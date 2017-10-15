#include "TEE_UC20.h"
#include "SoftwareSerial.h"
#include <AltSoftSerial.h>
#include "call.h"
#include "sms.h"
#include "internet.h"
#include "File.h"
#include "http.h"
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
void debug(String data){
  Serial.println(data);
}
void data_out(char data){
  Serial.write(data);
}



/*###   Current Variable    ###*/
#define ct1 A0
#define ct2 A1
#define ct3 A2
<<<<<<< HEAD

EnergyMonitor emon1;
EnergyMonitor emon2;
EnergyMonitor emon3;

double Irms1, Irms2, Irms3;
=======
//#define ct4 A3
//#define ct5 A8
//#define ct6 A9
EnergyMonitor emon1;
EnergyMonitor emon2;
EnergyMonitor emon3;
//EnergyMonitor emon4;
//EnergyMonitor emon5;
//EnergyMonitor emon6;
double Irms1, Irms2, Irms3, Irms4, Irms5, Irms6;
>>>>>>> ff8ab6acd268bc668135cadf9872730dc97aa446



/*###   MAX6675 Variable    ###*/
int ktcSO = 8;
int ktcCS = 9;
int ktcCLK = 11;
MAX6675 ktc(ktcCLK, ktcCS, ktcSO);



/*###   ZMPT101B Variable    ###*/
#define SAMPLING 300      //กำหนดจำนวนการสุ่มค่า
#define VOFFSET 512       // 2.5V จาก 10Bit
#define ADC_PIN A0        // เชื่อมต่อขา A0
#define AMPLITUDE 411.00  //กำหนดให้ Amplitude = 2V หรือ 411 ใน ADC 10
#define REAL_VAC 233.5    //กำหนดค่า Vinput ที่อ่านได้จริงจาก Multimeter
int adc_max, adc_min;
int adc_vpp;
float Voltage;
//ฟังก์ชั่นอ่านค่าแรงดันซีกบวก และลบเพื่อแปลงแรงดัน
void read_VAC(){
  int cnt;
  adc_max = 0;
  adc_min = 1024;

  for (cnt = 0; cnt < SAMPLING; cnt++) //วน loop อ่านค่า ADC
  {
    int adc = analogRead(ADC_PIN); //อ่านค่า ADC
    if (adc > adc_max) //หาค่า max
    {
      adc_max = adc;
    }
    if (adc < adc_min) //หาค่า min
    {
      adc_min = adc;
    }
  }
  adc_vpp = adc_max - adc_min; //หาผลต่างของ input (Vpp)
  // แปลงค่าที่อ่านได้เป็น  VAC
  Voltage = map(adc_vpp, 0, AMPLITUDE, 0, REAL_VAC * 100) / 100.00; 
  Serial.print("Phase 1 = ");
  Serial.print(Voltage);
  Serial.println(" VAC");
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
//  emon4.current(ct4, 115);
//  emon5.current(ct5, 115);
//  emon6.current(ct6, 115);


}



/*###   Void loop    ###*/
void loop() {
  Irms1 = emon1.calcIrms(1500);
  Irms2 = emon2.calcIrms(1500);
  Irms3 = emon3.calcIrms(1500);
//  Irms4 = emon4.calcIrms(1500);
//  Irms5 = emon5.calcIrms(1500);
//  Irms6 = emon6.calcIrms(1500);


  int temper = ktc.readCelsius();
  String data1 = String(Irms1);
  String data2 = String(Irms2);
  String data3 = String(Irms3);
  String data4 = String(Voltage);
  String data5 = "0";
  String data6 = "0";
  String data7 = String(temper);

  Serial.println(F("Start HTTP"));
  http.begin(1);
  Serial.println(F("Send HTTP GET"));
  http.url("http://api.thingspeak.com/update?api_key=6C0IJZX24FXU98BV&field1=" + data1 + "&field2="
           + data2 + "&field3=" + data3 + "&field4=" + data4 + "&field5=" + data5 + "&field6=" + data6 + "&field7=" + data7);
  Serial.println(http.get());
  Serial.println(F("Done HTTP GET"));
  delay(20000);
}
