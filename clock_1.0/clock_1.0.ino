#include <dht11.h>
#include <Keypad.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 10, 9, 8, 7, 6);//LCD1602相应接口，建立一个LCD对象
//        这里是   rs,en,d4,d5,d6,d7
//定义矩阵键盘相关参数
const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {':', '0', '#', 'D'}
};
//连接4x4按键的行位端口，相应控制板的数字IO口
byte rowPins[ROWS] = { 0, 3, 4, 11 };
byte colPins[COLS] = { 14, 15, 16, 17 };
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
//计时用数组
char * sec[60] = { (char*)"00", (char*)"01", (char*)"02", (char*)"03", (char*)"04", (char*)"05", (char*)"06", (char*)"07", (char*)"08", (char*)"09",
                   (char*)"10", (char*)"11", (char*)"12", (char*)"13", (char*)"14", (char*)"15", (char*)"16", (char*)"17", (char*)"18", (char*)"19",
                   (char*)"20", (char*)"21", (char*)"22", (char*)"23", (char*)"24", (char*)"25", (char*)"26", (char*)"27", (char*)"28", (char*)"29",
                   (char*)"30", (char*)"31", (char*)"32", (char*)"33", (char*)"34", (char*)"35", (char*)"36", (char*)"37", (char*)"38", (char*)"39",
                   (char*)"40", (char*)"41", (char*)"42", (char*)"43", (char*)"44", (char*)"45", (char*)"46", (char*)"47", (char*)"48", (char*)"49",
                   (char*)"50", (char*)"51", (char*)"52", (char*)"53", (char*)"54", (char*)"55", (char*)"56", (char*)"57", (char*)"58", (char*)"59"
                 };
int index = 45, mindex = 25, hourindex = 19;//初始化闹钟时间
int i, j = 0;//用循环
int counta, countb, ms;//用于计时
char clockIN[4];//储存键盘输入的数字
int clockOUT[2];//将键盘输入的数字转化为具体的时间
char timeIN[4];
int timeOUT[2];//补充的与set有关的两个变量
char button;////记录单词键盘所按下的按键
int lighton = 0;//记录灯亮起的时间
int lightoff = 0;//记录灯熄灭的时间
int DHT11PIN = 2;//#define DHT11PIN 2;
dht11 DHT11;
int led1 = 18;
int led2 = 19;
int sensorTouch = 5;

//用于在平时的状态下，在1602第一行显示当前时间的函数
void print_time() {//这个函数用来实时地显示时间
  lcd.setCursor(0, 0);
  lcd.print("Now:");
  lcd.print(sec[hourindex]);//显示小时的时间，sec是个数组，hourindex是个数字，最后一定要以字符串的形式打印出来
  lcd.print(":");
  lcd.print(sec[mindex]);
  lcd.print(":");
  lcd.print(sec[index]);//秒
}
//读取按键信息的函数
/*按键功能：按下A进入闹钟设置界面，可输入具体的闹钟时间；按下C设置时钟时间；
   按下D时间输入完毕后的确定按钮；按下其他任意键，显示温湿度*/

void function_decide(char customKey) {
  if (customKey == 'A')
    button = customKey;//按下A时记录当前按键信息
  if (customKey == 'C')
    button = customKey;//按下C时记录按键信息
  if (button == 'A') {
    lcd.setCursor(0, 1);//lcd光标的位置放好
    if (customKey == 'A') {
      //多次判断的原因，进入时间设置状态后会继续输入时间,这里是还没有开始输入数字
      lcd.clear();//清空1602上显示的其他多余内容
      print_time();//显示时间
      lcd.setCursor(0, 1);//第二行进入设置闹钟时间界面
      lcd.print("setclock:"); //也就是输入数字的准备阶段
    }
    else {//开始设置时间，也就是从键盘输入数字，可以想象输入是类似于“1334”这样的东西
      if (j < 4) {//时间的设置还没有结束：
        lcd.setCursor(i + 9, 1);//在已经按下A 的条件下，也就是进入了设置状态，前面是set clock：
        clockIN[j] = customKey;//记录当前键盘输入的数字，放进字符数组里面
        lcd.print(customKey);
        if (j == 1) {
          lcd.print(":");//作为输入的提示，进入分钟的的输入
          i++;//i用来控制LCD的光标位置
        }
        j++;//j用来控制输入的次数
        i++;
      }
      else {//这里意味着时间的输入已经完成
        i = 0;
        j = 0;
        button = 0; //闹钟时间输入完毕后清除此前的记录，将从这里退出
      }
    }
  }
  else if (button == 'C') {
    lcd.setCursor(0, 1);
    if (customKey == 'C') {
      lcd.clear();//清空1602此前显示的其他多余内容
      print_time();//重新显示时间
      lcd.setCursor(0, 1);//第二行开始显示进入设置时间模式
      lcd.print("set time:");
    }
    else {
      if (j < 4) {
        lcd.setCursor(i + 9, 1);//在已经按下C的条件下
        timeIN[j] = customKey;//记录当前键盘输入的时间
        lcd.print(customKey);//显示当前输入的数字
        if (j == 1) {
          lcd.print(":");
          i++;
        }
        j++;
        i++;
      }
      else {
        i = 0;
        j = 0;
        button = 1;//闹钟时间输入完毕后清除此前的记录
      }
    }
  }
  else if (customKey == 'D') {//按下D时，表示闹钟时间输入完毕，确认
    lcd.clear();//清空1602此前显示的其他多余内容
    print_time();//重新显示时间
    lcd.setCursor(0, 1);
    lcd.print("OK!It's set!");
    if (button == 0)//设置闹钟
      set_clock();
    else {//设置时间
      set_time();
      button = 0;//时间输入完成后清除此前的记录
    }
  }
  else {//按其他的任意按键都会显示温湿度
    lcd.clear();
    print_time();
    print_dht11();
  }
}

//时钟转换函数，将输入的单个数字转化为时钟的时间
void set_time() {
  timeOUT[0] = (timeIN[0] - 48) * 10 + timeIN[1] - 48;//数字和他的char之间差48
  hourindex = timeOUT[0];//hourindex直接和print_time关联
  timeOUT[1] = (timeIN[2] - 48) * 10 + timeIN[3] - 48;
  mindex = timeOUT[1];
  index = 0;
}

//闹钟时间转化函数，将键盘输入的单个数字转化为具体的时间格式
void set_clock() {
  clockOUT[0] = (clockIN[0] - 48) * 10 + clockIN[1] - 48;
  clockOUT[1] = (clockIN[2] - 48) * 10 + clockIN[3] - 48;
}

//预设时间到，闹钟启动
void clock_ring() {
  //lcd.clear();
  //print_time();
  lcd.setCursor(0, 0);
  lcd.print("  Time is up!");
  lcd.setCursor(0, 1);
  lcd.print("  GET UP GET UP!");
}

#define NOTE_B0 31
#define NOTE_C1 33
#define NOTE_CS1 35
#define NOTE_D1 37
#define NOTE_DS1 39
#define NOTE_E1 41
#define NOTE_F1 44
#define NOTE_FS1 46
#define NOTE_G1 49
#define NOTE_GS1 52
#define NOTE_A1 55
#define NOTE_AS1 58
#define NOTE_B1 62
#define NOTE_C2 65
#define NOTE_CS2 69
#define NOTE_D2 73
#define NOTE_DS2 78
#define NOTE_E2 82
#define NOTE_F2 87
#define NOTE_FS2 93
#define NOTE_G2 98
#define NOTE_GS2 104
#define NOTE_A2 110
#define NOTE_AS2 117
#define NOTE_B2 123
#define NOTE_C3 131
#define NOTE_CS3 139
#define NOTE_D3 147
#define NOTE_DS3 156
#define NOTE_E3 165
#define NOTE_F3 175
#define NOTE_FS3 185
#define NOTE_G3 196
#define NOTE_GS3 208
#define NOTE_A3 220
#define NOTE_AS3 233
#define NOTE_B3 247
#define NOTE_C4 262
#define NOTE_CS4 277
#define NOTE_D4 294
#define NOTE_DS4 311
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_FS4 370
#define NOTE_G4 392
#define NOTE_GS4 415
#define NOTE_A4 440
#define NOTE_AS4 466
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_CS5 554
#define NOTE_D5 587
#define NOTE_DS5 622
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_FS5 740
#define NOTE_G5 784
#define NOTE_GS5 831
#define NOTE_A5 880
#define NOTE_AS5 932
#define NOTE_B5 988
#define NOTE_C6 1047
#define NOTE_CS6 1109
#define NOTE_D6 1175
#define NOTE_DS6 1245
#define NOTE_E6 1319
#define NOTE_F6 1397
#define NOTE_FS6 1480
#define NOTE_G6 1568
#define NOTE_GS6 1661
#define NOTE_A6 1760
#define NOTE_AS6 1865
#define NOTE_B6 1976
#define NOTE_C7 2093
#define NOTE_CS7 2217
#define NOTE_D7 2349
#define NOTE_DS7 2489
#define NOTE_E7 2637
#define NOTE_F7 2794
#define NOTE_FS7 2960
#define NOTE_G7 3136
#define NOTE_GS7 3322
#define NOTE_A7 3520
#define NOTE_AS7 3729
#define NOTE_B7 3951
#define NOTE_C8 4186
#define NOTE_CS8 4435
#define NOTE_D8 4699
#define NOTE_DS8 4978

//闹钟铃声，想必还可以换成别的
int melody[] = {
  NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_G4, NOTE_G3,

  NOTE_C4, NOTE_G3, NOTE_E3, NOTE_A3, NOTE_B3, NOTE_AS3, NOTE_A3, NOTE_G3, NOTE_E4, NOTE_G4,
  NOTE_A4, NOTE_F4, NOTE_G4, NOTE_E4, NOTE_C4, NOTE_D4, NOTE_B3,

  NOTE_C4, NOTE_G3, NOTE_E3, NOTE_A3, NOTE_B3, NOTE_AS3, NOTE_A3, NOTE_G3, NOTE_E4, NOTE_G4,
  NOTE_A4, NOTE_F4, NOTE_G4, NOTE_E4, NOTE_C4, NOTE_D4, NOTE_B3,

  NOTE_G4, NOTE_FS4, NOTE_E4, NOTE_DS4, NOTE_E4, NOTE_GS3, NOTE_A3, NOTE_C4, NOTE_A3, NOTE_C4,
  NOTE_D4, NOTE_G4, NOTE_FS4, NOTE_E4, NOTE_DS4, NOTE_E4, NOTE_C5, NOTE_C5, NOTE_C5,

  NOTE_G4, NOTE_FS4, NOTE_E4, NOTE_DS4, NOTE_E4, NOTE_GS3, NOTE_A3, NOTE_C4, NOTE_A3, NOTE_C4,
  NOTE_D4, NOTE_DS4, NOTE_D4, NOTE_C4
};
int noteDurations[] = {
  8, 4, 4, 8, 4, 2, 2,
  3, 3, 3, 4, 4, 8, 4, 8, 8, 8, 4, 8, 4, 3, 8, 8, 3,
  3, 3, 3, 4, 4, 8, 4, 8, 8, 8, 4, 8, 4, 3, 8, 8, 2,
  8, 8, 8, 4, 4, 8, 8, 4, 8, 8, 3, 8, 8, 8, 4, 4, 4, 8, 2,
  8, 8, 8, 4, 4, 8, 8, 4, 8, 8, 3, 3, 3, 1
};

//用于播放闹钟铃声的函数（内含灯光变换）
void melody_play() {
  for (int thisNote = 0; thisNote < 74; thisNote++) { //74应该是整段铃声的长度
    setColor(200 * (thisNote % 2), 200 * ((thisNote + 1) % 2));//灯光变换
    int noteDuration = 1000 / noteDurations[thisNote];//通过两个数组的对应控制声音的音调和长度
    tone(13, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;//两个音阶间暂停相应节拍
    delay(pauseBetweenNotes);
    noTone(8);//停止tone（）函数发声
  }
  lcd.clear();
}

//RGB 三色LED亮度控制函数
void setColor(int red, int green) {
  analogWrite(led1, red);
  analogWrite(led2, green);
}

void print_dht11() {//这是显示温湿度的函数
  int chk = DHT11.read(DHT11PIN);//  DHT11PIN 2
  int t = DHT11.temperature;
  int h = DHT11.humidity;
  char tem[2] = {'0', '0'};
  char hum[2] = {'0', '0'};
  for (int i = 1; i >= 0; i--)
  {
    tem[i] = t % 10 + 48;
    hum[i] = h % 10 + 48;
    t = t / 10;
    h = h / 10;
  }
  lcd.setCursor(0, 1);
  lcd.print("Tep:");
  lcd.print(tem);
  lcd.setCursor(6, 1);
  lcd.print("C");
  lcd.setCursor(7, 1);
  lcd.print(" ");
  lcd.print("Hum:");
  lcd.print(hum);
  lcd.setCursor(14, 1);
  lcd.print("%");
  lcd.setCursor(15, 1);
  lcd.print(" ");
}


void setup() {
  Serial.begin(9600);
  pinMode(sensorTouch, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(DHT11PIN, OUTPUT);// DHT11PIN 2
  lcd.begin(16, 2);//初始化LCD1602，以2行，每行16格显示
  delay(1000);
  ms = millis();//ms表示1s 的时间长度，用于loop循环中的计时
  countb = ms;//初始化时间countb
  int chk = DHT11.read(DHT11PIN);//  DHT11PIN 2
  print_dht11();
}

void loop() {
  if ((hourindex == clockOUT[0]) && (mindex == clockOUT[1]) && (index == 1)) {
    //判断是否已到闹钟时间
    clock_ring();//lcd的显示
    melody_play();
    index = 23;
    tone(5, 0);//tone(pin,frequency,duration(可不要))
    digitalWrite(5, LOW);
    print_time();
    print_dht11();
  }
  counta = millis();//记录当前的时间
  if ((counta - countb) % ms == 0) {//当前与上一记录时间相差一秒钟是显示时间应加1
    if (index <= 60) {//秒针不能溢出
      if (index == 60) {
        index = 0;
        mindex++;
        if (mindex == 60) {
          mindex = 0;
          hourindex++;
          if (hourindex == 24)
            hourindex = 0;
        }
      }
    }
    countb = counta;
    print_time();
    index++;
  }//一般情况下走时间的函数片段
  char customKey = customKeypad.getKey();//随时读取键盘的输入
  if (customKey)
    function_decide(customKey);//调用前面的函数
  if (digitalRead(sensorTouch) == HIGH) {//按下触摸开关后LED灯亮10s
    lighton = index;//记录按下开关的时间
    lightoff = (index + 10) % 60;//记录灯灭的时间
  }
  if (index < lightoff)
    setColor(200, 200);
  else {
    setColor(0, 0);
    lighton = lightoff = 0;
  }
}
