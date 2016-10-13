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
char reqParam[100] = "mssv=";
char inetPath[100] = "/listofnumber2.php";
int nData;


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
    strcpy(reqParam, "mssv=");
    strcat(reqParam, lastProc);
    Serial.println(F("-----------------------------------------#--#-#--------------------------------"));
    Serial.println(reqParam);
    Serial.println(lastProc);
    nData = inet.httpPOST("www.emyeuptit.com", 80, inetPath, reqParam, soupBuffer, 1000);
    delay(2711);
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
      Serial.println(F("----------------------------------------------#--------------------------------"));
      Serial.println(soupResult);
      int posOfDeli = soupTmp.indexOf('|');
      int posOfDone = soupTmp.indexOf("|0");
      Serial.println(posOfDone);
      if(posOfDeli){
        String rcvTmp = soupTmp.substring(0, posOfDeli);
        rcvTmp.toCharArray(smsReceiver, 20);
        String cntTmp = soupTmp.substring(posOfDeli + 1);
        cntTmp.toCharArray(smsContent, 160);
        Serial.println(F("-------------------------------------------------------------------------------"));
        Serial.println(posOfDeli);
        strcpy(lastProc, smsReceiver);
        formatNumber(smsReceiver);
        sms.SendSMS(mrLong, smsReceiver);
        delay(2711);
        Serial.println(smsContent);
        sms.SendSMS(mrLong, smsContent);
        delay(2711);
        
      } else if(soupTmp.indexOf("nothingElse")){
        sms.SendSMS(mrLong, "xong roi anh yeu oi :3");
        delay(2711);
        strcpy(lastProc, "ptit_sms");
        //delay(5000000);
      } else {
        sms.SendSMS(mrLong, "Error occur :(");
      }
      
    }
  }
/*
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
       delay(2711);
      } else {
       Serial.println(F("\nGPRS status=IDLE"));
       sms.SendSMS(mrLong, "gsm1-gprs0");
       inet.dettachGPRS();
       delay(2711);
       if(inet.attachGPRS("v-internet", "", "")){
       delay(2711);
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
} */

void setup(){
  Serial.begin(9600);
  Serial.println(F("Sim900 Initializing..."));
  delay(2711);
  if (gsm.begin(4800)) {
    Serial.println(F("\nGSM status=READY"));
    started = true;
  }
}
void loop() {
  if (started) {
    char pos;
    pos = sms.IsSMSPresent(SMS_ALL);
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
  //inetGet();
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

