#include "SIM900.h"
#include <SoftwareSerial.h>
#include "inetGSM.h"
#include <Wire.h>
//#include <string.h>

InetGSM inet;

//---------------------------function--------------------------------//
void inetGet();
void formatNumber();
//---------------------------        --------------------------------//
//
//
//---------------------------variable------------------------//
boolean started = false; //trạng thái modul sim
char soupBuffer[1000];
char lastProc[20] = "longdeptrai"; //init for first requests
char mrLong[20] = "+84968686717";
char soupResult[160];
char* smsReceiver;
char* smsContent;
char* slaveBuffer;
char lastProcBuffer[20];
byte posOfReadSMS;
//-----------------------------------------------------------//






void setup() {
  Serial.begin(9600);
  Wire.begin();
  Serial.println("Initializing...");
  delay(2711);
  if (gsm.begin(2400)) {
    started = true;
    delay(2711);
  }
   if(started){
      if(inet.attachGPRS("v-internet", "", "")){
       Serial.println("debug 1");
       delay(5412);
      }
   }
  delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:
  inetGet();
  delay(2711);
}

void inetGet(){
    delay(2711);
    char inetPath[100] = "/listofnumber.php?mssv=";
    strcat(inetPath, lastProc);
    int nData = inet.httpGET("www.emyeuptit.com", 80, inetPath, soupBuffer, 1000);
    delay(5422);
    String rResponse = soupBuffer;
    rResponse.replace("", "<sp>");
    String soupTmp = soupBuffer;
    int posOfStart = soupTmp.indexOf("<body>");
    soupTmp.remove(0, posOfStart + 6);
    posOfStart = soupTmp.indexOf("0");
    soupTmp.remove(0, posOfStart);
    int posOfEnd = soupTmp.indexOf("</body>") - 1;
    soupTmp.remove(posOfEnd); // raw remaining
    soupTmp.toCharArray(soupResult, 160);
    Wire.requestFrom(27, 1); //  request 100 byte form device #27
    while (Wire.available()){
      char c = Wire.read();
      if (c == '1'){
       Wire.beginTransmission(27);
       Wire.write(soupResult);
       Wire.endTransmission();
       } else {
         delay(1);
       }
    }
}

