#include "Arduino.h"
#include "SoftwareSerial.h"
#include <DFRobotDFPlayerMini.h>
#include <Keypad.h>
#include <ld3320.h>
#define B 10
#define sensorTouch 8

VoiceRecognition Voice;
SoftwareSerial mySoftwareSerial(6, 7); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);
int f = 0;//MP3状态
int record =3;
int play_e=5;
int light=1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(B ,OUTPUT);
  digitalWrite(B,LOW);
  pinMode(sensorTouch,INPUT);
  mySoftwareSerial.begin(9600);
    Voice.init();
    Voice.addCommand("kai",0);  // 开灯
    Voice.addCommand("guan",1);//关灯
    Voice.addCommand("huan",2);//切歌
    Voice.addCommand("ting",3);//暂停
    Voice.addCommand("heihie",4);//继续播放
    Voice.addCommand("fang yin yue",5);//放音乐
    Voice.addCommand("bei wang",6);//备忘录音
    Voice.addCommand("bo",7);  //播放录音
    Voice.addCommand("shei",8);//你是谁
    //Voice.addCommand("ma de",9);//妈的
    Voice.addCommand("cao",9);
    //Voice.addCommand("ni ma",9);
    Voice.addCommand("jian",10);//报时
    Voice.start();
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    mySoftwareSerial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true);
  }
  Serial.println(F("DFPlayer Mini online."));
  pinMode(record,OUTPUT);
  pinMode(play_e,OUTPUT);
  myDFPlayer.volume(20);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(sensorTouch)==HIGH){
    if (light==0){
    digitalWrite(B,LOW);
    light=1;}
    else{
    digitalWrite(B,HIGH);
    light=0;}
  }
    int i=Voice.read();
      switch(i){
      case 0:
          digitalWrite(B,HIGH);
          light=0;
          break;
      case 1:
        digitalWrite(B,LOW);
        light=1;
          break;
      case 2:
          myDFPlayer.next();
          break;
       case 3:
             myDFPlayer.pause();
            f = 2;
            break;
       case 4:
        myDFPlayer.start();
        f = 1;
        break;
       case 5:
          myDFPlayer.play(1);
          break;
       case 6:
          digitalWrite(record,HIGH);
          delay(100000);
          //myDFPlayer.play(*);
          digitalWrite(record,LOW);
          break;
       case 7:
          digitalWrite(play_e,HIGH);
          delay(100000);
          digitalWrite(play_e,LOW);
       case 8:
          myDFPlayer.play(8);
          break;
       case 9:
          myDFPlayer.play(9);
          break;
        case 10:
          myDFPlayer.play(10);
          break;
      default:
        break;
  }
  if (myDFPlayer.available())
  {
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }
}

void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;

    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;

    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;

    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;

    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;

    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {

        case Busy:
          Serial.println(F("Card not found"));
          break;

        case Sleeping:
          Serial.println(F("Sleeping"));
          break;

        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;

        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;

        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;

        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;

        case Advertise:
          Serial.println(F("In Advertise"));
          break;

        default:
          break;
      }
      break;
    default:
      break;
  }
}
