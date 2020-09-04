#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SoftwareSerial.h>

/**
   View Mode 전환
*/
#define SW_MODE 1
#define SC_MODE 2

/**
   Button 입력값
*/
#define BTN_ON 0
#define BTN_OFF 1

/**
   입력 핀
*/
#define BT_RXD 2
#define BT_TXD 3
#define LED_R 4
#define LED_G 5
#define LED_B 6
#define MOD_BTN 7
#define SW_BTN 8

/**
   SW_MODE(STOP WATCH MODE) 스톱 워치 모드
   SC_MODE(SCORE MODE) 스코어 모드
*/

/**
   블루투스 객체 선언
*/
SoftwareSerial bluetooth(BT_TXD, BT_RXD);


/**
   LCD 객체 선언
*/
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);


void setup() {
  /* Bluetooth init */
  Serial.begin(9600);
  bluetooth.begin(9600);

  /* RGB LED init */
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  /* Mode Change Button init */
  pinMode(MOD_BTN, INPUT_PULLUP);

  /* Stop Watch Button init */
  pinMode(SW_BTN, INPUT_PULLUP);

  while (!Serial);

  /* LCD init */
  lcd.begin(16, 2);
  lcd.setBacklight(HIGH);
}

short mode = SW_MODE;
short mode_btn_read, mode_btn_have = 1;
/* 스톱워치에 저장할 시간 */
unsigned long long s_time;

void loop() {
  /**
     모드 변경 버튼 (toggle)
  */
  mode_btn_read = digitalRead(MOD_BTN);
  if (mode_btn_read == BTN_ON && mode_btn_read != mode_btn_have)
  {
    mode = mode == SW_MODE ? SC_MODE : SW_MODE;
    mode_btn_have = mode_btn_read;
    lcd.clear();
  }
  if (mode_btn_read == BTN_OFF) {
    mode_btn_have = mode_btn_read;
  }

  /**
     STOP WAHCT
  */
  if (mode == SW_MODE)
  {
    digitalWrite(LED_B, LOW);
    digitalWrite(LED_R, HIGH);
    /*
       블루투스 단말에서 입력시 과목 저장 후 스톱워치 버튼을 누를 때까지 대기
    */
    lcd.setCursor(0, 0);
    lcd.print("Please enter");
    lcd.setCursor(0, 1);
    lcd.print("your name");
    
    
    String b_data = "";
    if (bluetooth.available())
    {
      lcd.clear();
      while (bluetooth.available())
      {
        b_data += (char)bluetooth.read();
        delay(10);
      }
      /* b_data에 휴대폰으로 입력받은 과목명이 저장되어 있음 */

      lcd.setCursor(0, 0);
      lcd.print("Please enter");
      lcd.setCursor(0, 1);
      lcd.print("start button");


      /* 스톱워치를 누르고 땔 때까지 대기 */
      while (digitalRead(SW_BTN) == BTN_OFF);
      while (digitalRead(SW_BTN) == BTN_ON);

      
      lcd.clear();

      delay(500);
      lcd.setCursor(0, 0);
      lcd.print(b_data);

      s_time = millis() / 1000; // 현재 시간 저장

      /* 스톱워치 시작 */
      int count = millis() / 1000 - s_time; // 총 경과된 시간
      String hour, min, sec;
      while (true)
      {
        /* 스톱워치 버튼을 다시 누르면 종료 */
        if (digitalRead(SW_BTN) == BTN_ON) break;

        /* 시간 계산 */
        hour = parseHour(count);
        min = parseMin(count);
        sec = parseSec(count);


        lcd.setCursor(0, 1);
        lcd.print(parseTime(count));
        count = millis() / 1000 - s_time;
      }

      Serial.println(b_data + " " + String(count));

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Success to save");
      delay(3000);
      lcd.clear();
    }
  }
  /**
     SCORE MODE
  */
  else if (mode == SC_MODE)
  {
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_B, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("Enter task data");
    lcd.setCursor(0, 1);
    lcd.print("in terminal");
    
    
    /* Serial에 값이 들어오길 대기 */
    if (Serial.available())
    {
      String no_read = "";
      String temp = "";
      String read;
      int i = 0, span_cnt = 0;
      int d_cnt = 1;

      /* 값을 입력받음 */
      read = Serial.readStringUntil('\n');
      while ( read.length() > 0 )
      {
        lcd.clear();
        
        /* 이름 */
        lcd.setCursor(16 - read.indexOf(' '), 0);
        temp = read.substring(0, read.indexOf(' '));
        if(no_read.indexOf(temp) >= 0) {
          read = read.substring(read.indexOf(' ') + 1);
          read = read.substring(read.indexOf(' ') + 1);
          continue;
        }
        no_read += temp + " ";
        
        lcd.print(temp);
        read = read.substring(read.indexOf(' ')+1);

        /* 시간 */
        lcd.setCursor(0, 1);
        if(read.indexOf(' ') >= 0) lcd.print(parseTime(read.substring(0, read.indexOf(' ')).toInt()));
        else {
          lcd.print(parseTime(read.toInt()));
          read = "";
        }
        read = read.substring(read.indexOf(' ')+1);
        
        /* 순위 */
        lcd.setCursor(0, 0);
        lcd.print(d_cnt);

        /* 버튼을 누를때까지 대기, 누르면 다음 페이지로 */
        while(digitalRead(SW_BTN) == BTN_OFF);
        while(digitalRead(SW_BTN) == BTN_ON);
        d_cnt++;
      }
      lcd.clear();
    }
  }
}

String parseTime(int count){
  return parseHour(count) + ":" + parseMin(count) + ":" + parseSec(count);
}


String parseHour(int count) {
  return String(count / 3600).length() == 1 ? "0" + String(count / 3600) : String(count / 3600);
}

String parseMin(int count) {
  return String(count / 60 % 60).length() == 1 ? "0" + String(count / 60 % 60) : String(count / 60 % 60);
}

String parseSec(int count) {
  return String(count % 60).length() == 1 ? "0" + String(count % 60) : String(count % 60);
}
