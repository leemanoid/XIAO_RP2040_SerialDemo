// The MIT License (MIT)
//
// Copyright (c) 2015 Microsoft
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 

// Modified by Shartick Worker
// https://shartick.work/

/* XIAO RP2040: SerialCommand以外は使わないのでコメントアウト */
//#include <SoftwareSerial.h>
#include <SerialCommand.h>      // Steven Cogswell ArduinoSerialCommand library from http://GitHub.com
//#include <DallasTemperature.h>  // Miles Burton Arduino-Temperature-Contol library from http://GitHub.com
//#include <OneWire.h>            // Paul Stoffregen OneWire library from http://www.pjrc.com

//#define ONE_WIRE_BUS 2
//#define LED_LOW      3
//#define LED_HIGH     6
//#define TEMP_MAX     99.99
//#define TEMP_MIN     00.00
#define DBGMSG(A)    if (dbg){ Serial.print("DBG: "); Serial.println(A);}

//
// Globals
//
/* XIAO RP2040: SerialCommand以外は使わないのでコメントアウト */
SerialCommand     serialCommand;
//OneWire           oneWire(ONE_WIRE_BUS);
//DallasTemperature sensors(&oneWire);
//DeviceAddress     thermometerAddress = {-1};
boolean           dbg = true;

/* XIAO RP2040: 各種定義 */
#include <Arduino.h>
#include <U8g2lib.h>
 
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

//ユーザーLEDのピン定義
#define LED_RED 17
#define LED_GREEN 16
#define LED_BLUE 25

/* SSD1306 OLED: 各種定義 */
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

// 表示行数
#define DSPLINE 4

// 表示位置(16ptsフォント前提の固定値)
const int CUR_X[DSPLINE] = {0,0,0,0};
const int CUR_Y[DSPLINE] = {15,31,47,63};

// 表示可能な最大横幅：16ptsフォント(u8g2_font_b16_t_japanese2)を使う場合
const int OLEDwidth = 16;

// 半角1文字の横幅：16ptsフォント(u8g2_font_b16_t_japanese2)を使う場合
const int fontwidth = 8;

// 表示する文字を保持する領域
String DSPBUF[DSPLINE] = {"\0"};

void setup() {
  Serial.begin(9600);
  
  // コマンド登録
  serialCommand.addCommand("notify", Notify );
  serialCommand.addCommand("clear", Clear );
  serialCommand.addCommand("debug", SetDebug ); 
  serialCommand.addDefaultHandler(UnrecognizedCommand );

  // OLED初期化
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_b16_t_japanese2);

  // ブートロゴ表示
  DSPBUF[0] = "-*-*-*-**-*-*-*-";
  DSPBUF[1] = "  Mini Display  ";
  DSPBUF[2] = "with XIAO RP2040";
  DSPBUF[3] = "-*-*-*-**-*-*-*-";
  SendBuf(CUR_X, CUR_Y, DSPBUF);
  if (dbg) Serial.println("Display initiated.");

  // LEDのピン定義
  pinMode(LED_RED ,OUTPUT);  
  pinMode(LED_GREEN ,OUTPUT);  
  pinMode(LED_BLUE ,OUTPUT);
}

/* シリアルからコマンドを受信し続ける */
void loop() {
  serialCommand.readSerial();
}

/* シリアルから受信した文字列をOLED液晶に表示する */
void Notify() {
  // コマンド("notify ")以降の文字列を取得
  char *arg = serialCommand.getLast();

  if (dbg) Serial.println("arg:" + String(arg));
  
  if (arg != NULL ){
    // 受け取った文字列をOLEDに表示する。'\t'で改行してMAX4行まで表示
    marquee(String(arg), OLEDwidth, fontwidth);
  }
  // 引数の文字列がない（エラー）
  else {
    DBGMSG( F("text not specified") );

    // OLEDにもエラーメッセージを出す
    DSPBUF[0] = "ERROR";
    DSPBUF[1] = " notify cmd:";
    DSPBUF[2] = "Text not ";
    DSPBUF[3] = " specified.";
    SendBuf(CUR_X, CUR_Y, DSPBUF);
    
    // エラーだと分かるようにLEDチカチカ
    digitalWrite(LED_RED, HIGH);  delay(100);
    digitalWrite(LED_RED, LOW);   delay(100);
    digitalWrite(LED_RED, HIGH);  delay(100);
    digitalWrite(LED_RED, LOW);   delay(100);
    digitalWrite(LED_RED, HIGH);
  }
}

/* OLEDをクリアする */
void Clear(){
  DSPBUF[0] = "";
  DSPBUF[1] = "";
  DSPBUF[2] = "";
  DSPBUF[3] = "";
  SendBuf(CUR_X, CUR_Y, DSPBUF); 
}

//
// Enable or disable debug messages from being printed
// on the serial terminal
//
void SetDebug() {
  char *arg = serialCommand.next();
        
  DSPBUF[0] = "SetDebug " + String(arg);
  DSPBUF[1] = "";
  DSPBUF[2] = "";
  DSPBUF[3] = "";
  SendBuf(CUR_X, CUR_Y, DSPBUF);

  if (arg != NULL){
    if ( strcmp(arg, "on" ) == 0){
      dbg=true;
      DBGMSG(F("Turn on debug"));
    }
    if ( strcmp(arg, "off" ) == 0){
      DBGMSG(F("Turn off debug"));
      dbg=false;
    }
  }
}

//
// An unrecognized command was recieved
//
void UnrecognizedCommand(){  
  DBGMSG(F("未定義のコマンドが入力されました"));
  DBGMSG(F(" notify   - 通知を表示する" ));
  DBGMSG(F(" clear    - 画面をクリアする" ));
  DBGMSG(F(" debug on - turn on debug messages" ));
  DBGMSG(F(" debug off- turn off debug messages" ));

  // OLEDにもエラーメッセージを出す
  DSPBUF[0] = "ERROR";
  DSPBUF[1] = "unknown command";
  DSPBUF[2] = "";
  DSPBUF[3] = "";
  SendBuf(CUR_X, CUR_Y, DSPBUF);

  // エラーだと分かるようにLEDチカチカ
  digitalWrite(LED_GREEN, LOW);  delay(200);
  digitalWrite(LED_GREEN, HIGH); delay(200);
  digitalWrite(LED_RED, LOW);    delay(200);
  digitalWrite(LED_RED, HIGH);   delay(200);
  digitalWrite(LED_BLUE, LOW);   delay(200);
  digitalWrite(LED_BLUE, HIGH);  delay(200);
}

/* 文字列を受け取ってOLEDに表示する。はみ出る場合はスクロールする */
bool marquee(String msg, int OLEDwidth, int fontwidth){
  int maxmsglen = 0;
  String cmds[DSPLINE] = {"\0"};

  // 受け取った文字列をタブ文字で分割する
  split(msg, '\t', cmds);
  if (dbg) Serial.println("split1: " + cmds[0]);
  if (dbg) Serial.println("split2: " + cmds[1]);
  if (dbg) Serial.println("split3: " + cmds[2]);
  if (dbg) Serial.println("split4: " + cmds[3]);
  
  /* 1行ずつ文字数と文字幅を計算する */
  for (int j = 0; j < DSPLINE; j++){
    int cnt = 0;    //文字幅計算用変数;

    // 1バイトずつ処理するためStringからchar[]に変換する
    char msgArray[256];
    cmds[j].toCharArray(msgArray, 256);

    if (dbg) Serial.println("msg:" + String(cmds[j]));
    if (dbg) Serial.println("msgArray:" + String(msgArray));

    /* 文字数と文字幅を計算するために1バイトずつ処理する */
    for (int i = 0; i < sizeof(msgArray)-1; i++)
    {
      // 当該1バイト(HEX)の頭文字を切り出す
      char oneChar = String(msgArray[i], HEX).charAt(0);
      if (dbg) Serial.println(String(i) + ":" + String(msgArray[i], HEX) + " " + oneChar);

      // 改行文字ならスキップ
      if(String(msgArray[i], HEX) == "a" || //LF
         String(msgArray[i], HEX) == "d" ){ //CR
        continue;
      }
      
      // NULL文字なら終了
      if(String(msgArray[i], HEX) == "0"){ //NULL
        break;
      }

      // 先頭がEならUTF8全角文字とする(雑判定)
      if(oneChar == 'e'){
        i = i + 2;  /* 3バイト読む分カーソル位置を進める */
        cnt += 2;   /* 全角文字なので幅は+2 */
      }
      // TODO: 先頭がFなら4バイトらしい(5バイトや6バイトの場合もあるとか…)
      else if (oneChar == 'f'){
        i = i + 3;  /* 4バイト読む分カーソル位置を進める */
        cnt += 2;   /* 全角文字なので幅は+2 */
      }
      // それ以外ならASCIIコードとする
      else {
        cnt += 1;   /* 半角文字なので幅は+1 */    
      }
    } /* i */
    
    // 4行のなかで最も長い横幅をmaxmsglenに保持する
    if(maxmsglen < cnt) maxmsglen = cnt;
  } /* j */

  // とりあえずOLEDへ出力
  SendBuf(CUR_X, CUR_Y, cmds);
  delay(2000);

  // 文字列の最大横幅がOLEDパネル横幅を超える場合はスクロールする
  if (maxmsglen > OLEDwidth){
    // スクロールが必要な文字幅
    int diff = maxmsglen - OLEDwidth;
    // スクロールが必要なピクセル数
    int width = diff * fontwidth;

    // 各行のX座標を1ピクセルずつマイナスしていく
    for (int k = 0; k < width; k++){
      const int X[DSPLINE] = {0-k,0-k,0-k,0-k};
      SendBuf(X, CUR_Y, cmds);
      //delay(5); //スクロール速度を調整
    }
    return true;
  } else {
    return false;
  }
}

/* 4行をOLEDに出力する（X座標, Y座標, 出力文字列） */ 
void SendBuf(const int *X, const int *Y, String *str){
  u8g2.clearBuffer();
  u8g2.setCursor(X[0], Y[0]); u8g2.print(str[0]);
  u8g2.setCursor(X[1], Y[1]); u8g2.print(str[1]);
  u8g2.setCursor(X[2], Y[2]); u8g2.print(str[2]);
  u8g2.setCursor(X[3], Y[3]); u8g2.print(str[3]);
  u8g2.sendBuffer();
}

/* 文字列を分割して配列で返す（元文字列, デリミタ, 出力配列） */
int split(String data, char delimiter, String *dst){
    int index = 0;
    int arraySize = (sizeof(data)/sizeof((data)[0]));  
    int datalength = data.length();
    for (int i = 0; i < datalength; i++) {
        char tmp = data.charAt(i);
        if ( tmp == delimiter ) {
            index++;
            if ( index > (arraySize - 1)) return -1;
        }
        else dst[index] += tmp;
    }
    return (index + 1);
}
