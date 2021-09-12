#include <dht11.h>
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <TimerOne.h>

//Lcd实例化
LiquidCrystal lcd(9, 8, 7, 6, 5, 4);

//dht11实例化
int DHT11PIN = 3;//#define DHT11PIN 2;
dht11 DHT11;

//矩阵键盘
const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = { 14, 15, 11, 10 };
byte colPins[COLS] = { 16, 17, 18, 19 };
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

//计时用数组
char * sec[60] = { (char*)"00", (char*)"01", (char*)"02", (char*)"03", (char*)"04", (char*)"05", (char*)"06", (char*)"07", (char*)"08", (char*)"09",
                   (char*)"10", (char*)"11", (char*)"12", (char*)"13", (char*)"14", (char*)"15", (char*)"16", (char*)"17", (char*)"18", (char*)"19",
                   (char*)"20", (char*)"21", (char*)"22", (char*)"23", (char*)"24", (char*)"25", (char*)"26", (char*)"27", (char*)"28", (char*)"29",
                   (char*)"30", (char*)"31", (char*)"32", (char*)"33", (char*)"34", (char*)"35", (char*)"36", (char*)"37", (char*)"38", (char*)"39",
                   (char*)"40", (char*)"41", (char*)"42", (char*)"43", (char*)"44", (char*)"45", (char*)"46", (char*)"47", (char*)"48", (char*)"49",
                   (char*)"50", (char*)"51", (char*)"52", (char*)"53", (char*)"54", (char*)"55", (char*)"56", (char*)"57", (char*)"58", (char*)"59"
                 };
int index = 20, mindex = 20, hourindex = 20;//初始化闹钟时间
int i, j = 0;//用于循环

//键盘与时间的交互
char clockIN[4];//储存键盘输入的数字
int clockOUT[2];//将键盘输入的数字转化为具体的时间
char timeIN[4];
int timeOUT[2];
char button;//记录单词键盘所按下的按键
int lighton = 0;//记录灯亮起的时间
int lightoff = 0;//记录灯熄灭的时间
int func = 0;//记录当前的功能
/*
  func = 0:不执行功能，默认状态
  func = 1:设置闹钟，对应A
  func = 2:设置时间，对应B
  func = 3:设置闹钟铃声，对应C
  func = 4:
*/

void Isr()
{
  if (index <= 60)
  { //秒针不能溢出
    if (index == 60)
    {
      index = 0;
      mindex++;
      if (mindex == 60)
      {
        mindex = 0;
        hourindex++;
        if (hourindex == 24)
          hourindex = 0;
      }
    }
  }
  print_time();
  index++;
}

//用于在平时的状态下，在1602第一行显示当前时间的函数
void print_time()
{ //这个函数用来实时地显示时间
  lcd.setCursor(0, 0);
  lcd.print("Now:");
  lcd.print(sec[hourindex]);//显示小时的时间，sec是个数组，hourindex是个数字，最后一定要以字符串的形式打印出来
  lcd.print(":");
  lcd.print(sec[mindex]);
  lcd.print(":");
  lcd.print(sec[index]);//秒
  lcd.print(" ");
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

//这是显示温湿度的函数
void print_dht11() {
  int chk = DHT11.read(DHT11PIN);//  DHT11PIN 2
  int t = DHT11.temperature;
  int h = DHT11.humidity;
  char tem[2] = { '0', '0' };
  char hum[2] = { '0', '0' };
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

//闹钟时间到
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

void melody_play() {
  for (int thisNote = 0; thisNote < 74; thisNote++) { //74应该是整段铃声的长度
    int noteDuration = 1000 / noteDurations[thisNote];//通过两个数组的对应控制声音的音调和长度
    tone(2, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;//两个音阶间暂停相应节拍
    delay(pauseBetweenNotes);
    noTone(2);//停止tone（）函数发声
  }
  lcd.clear();
  print_time();
  print_dht11();
}

//当有按键被按下时
void function(char key)
{
  if (key == 'A') //按下A键
  {
    //按下A进入设置闹钟的功能，无论原来是什么状态
    //软件设置
    i = 0;
    j = 0;
    func = 1;
    //硬件设置
    lcd.clear();//清空1602上显示的其他多余内容
    print_time();//显示时间
    lcd.setCursor(0, 1);//第二行进入设置闹钟时间界面
    lcd.print("setclock:"); //也就是输入数字的准备阶段
  }
  else if (key == 'B')
  {
    //按下B进入设置时间的功能，无论原来是什么状态
    //软件设置
    i = 0;
    j = 0;
    func = 2;
    //硬件设置
    lcd.clear();//清空1602上显示的其他多余内容
    print_time();//显示时间
    lcd.setCursor(0, 1);//第二行进入设置时间界面
    lcd.print("set time:"); //也就是输入数字的准备阶段
  }
  else if (key == 'D')
  {
    if (func == 1)
    {
      //说明此时在设置闹钟
      i = 0;
      j = 0;
      func = 0;
      set_clock();

      lcd.clear();//清空1602此前显示的其他多余内容
      print_time();//重新显示时间
      lcd.setCursor(0, 1);
      lcd.print("OK!It's set!");
    }
    else if (func == 2)
    {
      //说明此时在设置时间
      i = 0;
      j = 0;
      func = 0;
      set_time();

      lcd.clear();//清空1602此前显示的其他多余内容
      print_time();//重新显示时间
      lcd.setCursor(0, 1);
      lcd.print("OK!It's set!");
    }
    else if (func == 0)
    {
      //这时候没有执行任何功能，可能是在“OK!It's set!"的界面，可以清除
      lcd.clear();
      print_time();
      print_dht11();
    }
  }
  else  //说明是输入数字
  {
    if (func == 1) // 正在设置闹钟
    {
      if (j < 4)
      { //时间的设置还没有结束：
        lcd.setCursor(i + 9, 1);//在已经按下A 的条件下，也就是进入了设置状态，前面是set clock：
        clockIN[j] = key;//记录当前键盘输入的数字，放进字符数组里面
        lcd.print(key);
        if (j == 1)
        {
          lcd.print(":");//作为输入的提示，进入分钟的的输入
          i++;//i用来控制LCD的光标位置
        }
        j++;//j用来控制输入的次数
        i++;
      }
      else
      { //这里意味着时间的输入已经完成
        //软件设置
        i = 0;
        j = 0;
        func = 0; //闹钟时间输入完毕后清除此前的记录，将从这里退出
        lcd.clear();//清空1602此前显示的其他多余内容
        print_time();//重新显示时间
        lcd.setCursor(0, 1);
        lcd.print("OK!It's set!");
      }
    }
    else if (func == 2)
    {
      if (j < 4)
      { //时间的设置还没有结束：
        lcd.setCursor(i + 9, 1);//在已经按下B的条件下，也就是进入了设置状态，前面是set clock：
        timeIN[j] = key;//记录当前键盘输入的数字，放进字符数组里面
        lcd.print(key);
        if (j == 1)
        {
          lcd.print(":");//作为输入的提示，进入分钟的的输入
          i++;//i用来控制LCD的光标位置
        }
        j++;//j用来控制输入的次数
        i++;
      }
      else
      { //这里意味着时间的输入已经完成
        //软件设置
        i = 0;
        j = 0;
        func = 0; //闹钟时间输入完毕后清除此前的记录，将从这里退出
        lcd.clear();//清空1602此前显示的其他多余内容
        print_time();//重新显示时间
        lcd.setCursor(0, 1);
        lcd.print("OK!It's set!");
      }
    }
    else
    {
      lcd.clear();
      print_time();
      print_dht11();
    }
  }

}

void setup() {
  Serial.begin(9600);
  Timer1.initialize( 1000000 );
  Timer1.attachInterrupt( Isr );
  pinMode(DHT11PIN, OUTPUT);
  //pinMode(2,OUTPUT);
  lcd.begin(16, 2);//初始化LCD1602，以2行，每行16格显示
  delay(1000);
  int chk = DHT11.read(DHT11PIN);//  DHT11PIN 2
  print_dht11();
  print_time();
}

void loop() {
  // put your main code here, to run repeatedly:
  if ((hourindex == clockOUT[0]) && (mindex == clockOUT[1]) && (index == 1))
  {
    //判断是否已到闹钟时间
    clock_ring();//lcd的显示
    melody_play();
    index = 23;
    print_time();
    print_dht11();
  }
  char customKey = customKeypad.getKey();//随时读取键盘的输入
  if (customKey)
    function(customKey);//调用前面的函数
}
