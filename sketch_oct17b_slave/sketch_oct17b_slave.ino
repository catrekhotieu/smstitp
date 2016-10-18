#include "SIM900.h"
#include <SoftwareSerial.h>
#include <Wire.h>
#include "sms.h"
#include "inetGSM.h"

//-------------------------function-------------------------//
//----------------------------------------------------------//



//-------------------------variable-------------------------//
boolean started = false; //trạng thái modul sim
volatile boolean forceRun = false;
char smstext[160];// nội dung tin nhắn
char number[15]; // số điện thoại format theo định dạng quốc tế
char mrLong[13] = "+84968686717";
char smsReceiver[20];
char smsContent[160];
char smsMessage[160];
volatile char soupBuffer[160];
char lastProc[20];
char soupResult[160];
byte posOfReadSMS;    
volatile byte x, y; 
//----------------------------------------------------------//
SMSGSM sms;
void setup() {
  Serial.begin(9600);
  Wire.begin(27);
  Wire.onReceive(receiveEvent);
 // Wire.onRequest(requestEvent);
  Serial.println("initializing...");
  if (gsm.begin(4800)) {
    started = true;
    delay(2711);
    if(started){
       delay(5412);
      } else {
       sms.SendSMS(mrLong, "gsm1-gprs0");
       delay(2711);
      } 
  } else {
    sms.SendSMS(mrLong, "gsm0 :((");
  }
}

void loop() {
 delay(2711); 
  smsProcess();
  /* for(int i = 0; i < 3; i++){
    posOfReadSMS = sms.IsSMSPresent(SMS_READ);
    if (posOfReadSMS){
      sms.DeleteSMS(posOfReadSMS);
    }
   } */
  // i2c
  if(forceRun){
    sms.SendSMS(smsReceiver, smsMessage);
    delay(500);
    x = 0;
    y = 0;
    strcpy(smsReceiver, "");
    strcpy(smsMessage, "");
    delay(2711);
    forceRun = false;
  }
}


void receiveEvent(int msgLen){
  Serial.print("nhan duoc lenh tu master: ");
  Serial.println(msgLen);
  char c[1];
  while(Wire.available() > 0){
    char c = Wire.read();
    if(c == '@'){
      c = Wire.read();
      Serial.println(c);
      smsReceiver[x] = c;
      x++;
    } else if(c == '$'){
      c = Wire.read();
      Serial.println(c);
      smsMessage[y] = c;
      y++;
    } else if(c == '#'){
      Serial.println(c);
      forceRun = true;
    } else {
      Serial.println("unknow error @ # $");
    }
    
  }
}


void smsProcess(){
  if (started) {
    char pos;
    pos = sms.IsSMSPresent(SMS_UNREAD);
    if ((int)pos) {
      if (sms.GetSMS(pos, number, smstext, 160)) {
        char smsContent[160];
        if (strcmp(smstext, "kttk") == 0) {
          gsm.SimpleWriteln(F("AT+CUSD=1,\"*101#\""));
          delay(15000);//phai doi 5 giay moi gui va doc het du lieu
          char resp[160];//khoi tao xau cho noi dung tra ve
          gsm.read(resp, 160); //lay no
          Serial.println(resp);//in ra lan 2
          delay(2711);
          sms.SendSMS(mrLong, resp);
          delay(2711);
          strcpy(resp, "");
          // *102#
          gsm.SimpleWriteln(F("AT+CUSD=1,\"*102#\""));
          delay(5000);//phai doi 5 giay moi gui va doc het du lieu
          gsm.read(resp, 200); //lay no
          Serial.println(resp);//in ra lan 2
          delay(2711);
          sms.SendSMS(mrLong, resp);
          strcpy(resp, "");
          delay(2711);
        } else if (strcmp(smstext, "kttn") == 0) {
              sms.SendSMS("109", "kt100");
              delay(2711);
        } else {
              strcpy(smsContent, "");
              strcat(smsContent, "rvcd 1 sms from :: ");
              strcat(smsContent, number);
              strcat(smsContent, "\n");
              strcat(smsContent, smstext);
              sms.SendSMS(mrLong, smsContent);
              strcpy(smsContent, "");
              delay(2711);
          }
       }
        sms.DeleteSMS(byte(pos));
    }
    delay(2711);
  }
}

