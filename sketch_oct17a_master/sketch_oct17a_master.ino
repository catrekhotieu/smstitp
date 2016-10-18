#include "SIM900.h"
#include <SoftwareSerial.h>
#include "inetGSM.h"
#include <Wire.h>
//#include <string.h>

InetGSM inet;

//---------------------------function--------------------------------//
void inetGet();
void formatNumber(char input[]);
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
    delay(2711);
    String soupTmp = soupBuffer;
    soupTmp.replace("<br>", "\n");
    strcpy(soupBuffer, "");
    int posOfStart = soupTmp.indexOf("<body>");
    soupTmp.remove(0, posOfStart + 6);
    posOfStart = soupTmp.indexOf("0");
    soupTmp.remove(0, posOfStart);
    int posOfEnd = soupTmp.indexOf("</body>") - 1;
    soupTmp.remove(posOfEnd); // raw remaining ---------------------------------
    soupTmp.toCharArray(soupResult, 160);
    int posOfDeli = soupTmp.indexOf('|');
    if(posOfDeli != -1){
      String rcvTmp = soupTmp.substring(0, posOfDeli);
      rcvTmp.toCharArray(smsReceiver, 20);
      String cntTmp = soupTmp.substring(posOfDeli + 1);
      cntTmp.toCharArray(smsContent, 160);
      strcpy(lastProc, rcvTmp.c_str());
      formatNumber(smsReceiver);
//      smsReceiver = rcvTmp.c_str();
//      strcpy(smsContent, cntTmp.c_str());
      Serial.println("-------------------------------------");
      Serial.println(smsReceiver);
      Serial.println(smsContent);
      
      //Serial.println(soupResult);
      
       for(int i = 0; i < rcvTmp.length(); i++){
        Wire.beginTransmission(27);
        Wire.write('@');
        Wire.write(rcvTmp.charAt(i));
        Wire.endTransmission();
       }
      
       for(int i = 0; i < cntTmp.length(); i++){
        Wire.beginTransmission(27);
        Wire.write('$');
        Wire.write(cntTmp.charAt(i));
        Wire.endTransmission();
       }
      Wire.beginTransmission(27);
      Wire.write('#');
      Wire.endTransmission();
    }
}
void formatNumber(char input[]){
  String ftmp = input;
  if(ftmp.charAt(0) == '0'){
    ftmp.remove(0, 1);
    ftmp = "+84" + ftmp;
    ftmp.toCharArray(input, ftmp.length() + 1);
  }
}
