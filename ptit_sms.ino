#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"
#include "inetGSM.h"
#include <string.h>

SMSGSM sms;
InetGSM inet;

boolean started = false; //trạng thái modul sim
char soupBuffer[1000];
char smstext[160];// nội dung tin nhắn
char number[15]; // số điện thoại format theo định dạng quốc tế
char mrLong[13] = "+84968686717";
char lastProc[20] = "longdeptrai"; //init for first requests
char smsReceiver[20];
char smsContent[160];
char soupResult[160];
char lastProcBuffer[20];
bool forceRun = true;
        
void formatNumber(char input[]){
  String ftmp = input;
  if(ftmp.charAt(0) == '0'){
    ftmp.remove(0, 1);
    ftmp = "+84" + ftmp;
    ftmp.toCharArray(input, ftmp.length() + 1);
  }
}
void inetGet(){
    Serial.println(F("\nGPRS status=READY"));
    char inetPath[100] = "/listofnumber.php?mssv=";
    strcat(inetPath, lastProc);
    int nData = inet.httpGET("www.emyeuptit.com", 80, inetPath, soupBuffer, 1000);
    delay(5422);
    String rResponse = soupBuffer;
    if(nData){
      String soupTmp = soupBuffer;
      soupTmp.replace("<br>", "\n");
      int posOfStart = soupTmp.indexOf("<body>");
      soupTmp.remove(0, posOfStart + 6);
      posOfStart = soupTmp.indexOf("0");
      soupTmp.remove(0, posOfStart);
      int posOfEnd = soupTmp.indexOf("</body>") - 1;
      soupTmp.remove(posOfEnd); // raw remaining
      soupTmp.toCharArray(soupResult, 160);
      int posOfDeli = soupTmp.indexOf('|');
      if(posOfDeli != -1){
        String rcvTmp = soupTmp.substring(0, posOfDeli);
        rcvTmp.toCharArray(smsReceiver, 20);
        String cntTmp = soupTmp.substring(posOfDeli + 1);
        cntTmp.toCharArray(smsContent, 160);
        Serial.println(F("-------------------------------------------posOfDeli------------------------------------"));
        Serial.println(posOfDeli);
        Serial.println(smsReceiver);
        strcpy(lastProc, smsReceiver);
        formatNumber(smsReceiver);
        if(sms.SendSMS(smsReceiver, smsContent)){
          char goodSMSBuffer[100] = "Love Viettel: ";
          strcat(goodSMSBuffer, smsReceiver);
          sms.SendSMS(mrLong, goodSMSBuffer);
          delay(5422);
        } else {
          char badSMSBuffer [100] = "Viettel fucked: ";
          strcat(badSMSBuffer, smsReceiver);
          sms.SendSMS(mrLong, badSMSBuffer);
          delay(5422);
        }
        
      } else {
        if(forceRun){
          sms.SendSMS(mrLong, "n0thingElse :boss:");
        }
        delay(2711);
        strcpy(lastProc, "a");
        //delay(5000000);
      }
      
    }
} 

void setup() {
  Serial.begin(9600);
  Serial.println(F("Sim900 Initializing..."));
  delay(2711);
  if (gsm.begin(4800)) {
    Serial.println(F("\nGSM status=READY"));
    started = true;
    delay(2711);
    // Inet Get work plans
    if(started){
      if(inet.attachGPRS("v-internet", "", "")){
       delay(5412);
      } else {
       Serial.println(F("\nGPRS status=IDLE"));
       sms.SendSMS(mrLong, "gsm1-gprs0");
       inet.dettachGPRS();
       delay(2711);
       if(inet.attachGPRS("v-internet", "", "")){
       delay(5412);
       } else {
          return;
       }
      }
      inetGet();
    }
  } else {
    Serial.println(F("\nstatus=IDLE"));
  }
  delay(500);
}

void loop() {
  if (started) {
    char pos;
    pos = sms.IsSMSPresent(SMS_UNREAD);
    //hàm này sẽ trả về giá trị trong khoảng từ 0-40
    if ((int)pos) { //nêu có tin nhắn chưa đọc
      if (sms.GetSMS(pos, number, smstext, 160)) {
        char smsContent[160];
        if (strcmp(smstext, "kttk") == 0) {
          gsm.SimpleWriteln(F("AT+CUSD=1,\"*101#\""));
          delay(15000);//phai doi 5 giay moi gui va doc het du lieu
          char resp[160];//khoi tao xau cho noi dung tra ve
          gsm.read(resp, 160); //lay no
          Serial.println(resp);//in ra lan 2
          sms.SendSMS(mrLong, resp);
          delay(1000);
          strcpy(resp, "");
          // *102#
          gsm.SimpleWriteln(F("AT+CUSD=1,\"*102#\""));
          delay(5000);//phai doi 5 giay moi gui va doc het du lieu
          gsm.read(resp, 200); //lay no
          Serial.println(resp);//in ra lan 2
          sms.SendSMS(mrLong, resp);
          strcpy(resp, "");
          delay(1000);
          } else if (strcmp(smstext, "kttn") == 0) {
              sms.SendSMS("109", "kt100");
              delay(1000);
          } else if (strcmp(smstext, "cdey") == 0) {
               forceRun = true;
               if(forceRun){
                sms.SendSMS(mrLong, "oke anh yeu - forceRun -> 1");
               }
          } else if (strcmp(smstext, "drey") == 0) {
               forceRun = false;
               if (!forceRun){
                sms.SendSMS(mrLong, "oke anh yeu - forceRun -> 0");
               }
          } else {
              strcpy(smsContent, "");
              strcat(smsContent, "rvcd 1 sms from :: ");
              strcat(smsContent, number);
              strcat(smsContent, "\n");
              strcat(smsContent, smstext);
              sms.SendSMS(mrLong, smsContent);
              strcpy(smsContent, "");
              delay(1000);
          }
       }
      sms.DeleteSMS(byte(pos));
    }
    delay(1000);
  }
  inetGet();
  /* if (Serial.available() > 0) {
    String serialIn = Serial.readString();
    char smsSender[20];
    char smsContent[160];
    if (serialIn.charAt(0) == '0') {
      serialIn.remove(0, 1);
      serialIn = "+84" + serialIn;
    }
    byte posOfDeli = serialIn.indexOf('|');
    serialIn.substring(0, posOfDeli).toCharArray(smsSender, 20);
    serialIn.substring(posOfDeli + 1).toCharArray(smsContent, 160);

    if (sms.SendSMS(smsSender, smsContent)) {
      char smsSuccess[160];
      String smsSS = "module sent Succeed! |" + serialIn;
      if (smsSS.length() >= 159) {
        smsSS.remove(159);
      }
      smsSS.toCharArray(smsSuccess, 160);
      sms.SendSMS(mrLong, smsSuccess);
    } else {
      char smsFailure[160];
      String smsFL = "module sent Failed!|" + serialIn;
      if (smsFL.length() >= 159) {
        smsFL.remove(159);
      }
      smsFL.toCharArray(smsFailure, 160);
      sms.SendSMS(mrLong, smsFailure);
    }
  }
  */
}

