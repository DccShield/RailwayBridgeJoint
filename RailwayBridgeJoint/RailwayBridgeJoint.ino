//--------------------------------------------------------------------------------
// レイアウト用鉄橋ジョイント音ジェネレータ
// [RailwayBridgeJoint.ino]
// Copyright (c) 2020 Ayanosuke(Maison de DCC)
//
// http://maison-dcc.sblo.jp/ http://dcc.client.jp/ http://ayabu.blog.shinobi.jp/
// https://twitter.com/masashi_214
//
// DCC電子工作連合のメンバーです
// https://desktopstation.net/tmi/ https://desktopstation.net/bb/index.php
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//--------------------------------------------------------------------------------

#include "jointini.h"


extern void ImaAdpcmSetup();
extern void RailWayBridgeState( JOINT );
extern void Bridgeini( void );

#define PWMDAC_OUTPUT_PIN   10

//各種設定、宣言

#define PIN_F0_F  3     // D3 PD3,PWM
#define PIN_F0_R  4     // D4 PD4
#define PIN_AUX1  5     // D5 PD5
#define PIN_AUX2  6     // D6 PD6
#define PIN_AUX3  7     // D7 PD7
#define PIN_AUX4  8     // D8 PB0
#define PIN_AUX5  9     // D9 PB1,DIGITAL TR,PWM
#define PIN_AUX6  10    // D10 PB2,DIGITAL TR,PWM
#define PIN_AUX7  11    // D11 PB3,DIGITAL TR,PWM

#define MAX_SERVO 2     // サーボの最大数2個
#define PIN_SERVO1 5    // D5 PD5
#define PIN_SERVO2 4    // D4 PD4
#define PIN_LED_DIV1 6  // D6 PD6 SERVO1用DIV(分岐) LED
#define PIN_LED_STR1 7  // D7 PD7 SERVO1用STR(直進) LED
#define PIN_LED_DIV2 8  // D8 PB0 SERVO2用DIV(分岐) LED
#define PIN_LED_STR2 9  // D9 PB1 SERVO2用STR(直進) LED

#define ANGLE_MIN_A 800    // DM-S0025 400-2100  0-180deg
#define ANGLE_MAX_A 2200     // DM-S0037 1000-2000 0-90deg 
                             // DM-S0037 670-2600 0-180deg
                             
#define ANGLE_MIN_B 500     // 1000 -90deg
#define ANGLE_MAX_B 2000    // 2000 +90deg


unsigned long PreviosTime = 0;
unsigned long PreviosTimeState = 0;

JOINT Train;
JOINT Cargo;

void setup() {
  Serial.begin(115200);

  ImaAdpcmSetup();
  Bridgeini();

  int i;
  for(i=0;i<=128;i++){  // プチ音の防止の為ゆっくり上げる
    OCR1B = i;
  }

  PreviosTime = millis();
  PreviosTimeState = PreviosTime;
}

void loop() {
  if( (millis() - PreviosTime ) >= 100 ){  // 1000:1000msec 前回から1000ms経過したかチェック
    PreviosTime = millis();
    RailWayBridgeState(Train);
//    RailWayBridgeState(Cargo);
  }
}

void Bridgeini(void){
//----- 電車 6両
Train.type = 0;
Train.TrainWait = 5;            // 1000ms
Train.RollingStock[0] = 6;      // 6両
Train.VehicleSpace[0][1] = 1;      // HIGH2 車両間隔
Train.VehicleSpace[0][2] = 3;      // HIGH1
Train.VehicleSpace[0][3] = 4;      // MID
Train.VehicleSpace[0][4] = 5;      // LOW1
Train.VehicleSpace[0][5] = 8;      // LOW2
Train.BogieWheelbaseNum[0] = 2;    // 台車2台
Train.ChassisWheelbase[0][1] = 5;  // HIGH2 @100ms 台車間隔
Train.ChassisWheelbase[0][2] = 10; // HIGH1 @100ms
Train.ChassisWheelbase[0][3] = 15; // MID   @100ms
Train.ChassisWheelbase[0][4] = 20; // LOW1  @100ms
Train.ChassisWheelbase[0][5] = 25; // LOW2  @100ms

//----- 機関車 1両 長貨車 10両
Cargo.type = 1;
Cargo.RollingStock[0] = 1; //1両
Cargo.VehicleSpace[0][1] = 1;      // HIGH2 車両間隔
Cargo.VehicleSpace[0][2] = 3;      // HIGH1
Cargo.VehicleSpace[0][3] = 4;      // MID
Cargo.VehicleSpace[0][4] = 5;      // LOW1
Cargo.VehicleSpace[0][5] = 8;      // LOW2
Cargo.BogieWheelbaseNum[0] = 3;    // 台車3台
Cargo.ChassisWheelbase[0][1] = 2;  // HIGH2 @100ms 台車間隔
Cargo.ChassisWheelbase[0][2] = 2; // HIGH1 @100ms
Cargo.ChassisWheelbase[0][3] = 3; // MID   @100ms
Cargo.ChassisWheelbase[0][4] = 18; // LOW1  @100ms
Cargo.ChassisWheelbase[0][5] = 23; // LOW2  @100ms

// 貨物
Cargo.RollingStock[1] = 10;        //10両
Cargo.VehicleSpace[1][1] = 1;      // HIGH2 車両間隔
Cargo.VehicleSpace[1][2] = 3;      // HIGH1
Cargo.VehicleSpace[1][3] = 4;      // MID
Cargo.VehicleSpace[1][4] = 5;      // LOW1
Cargo.VehicleSpace[1][5] = 8;      // LOW2
Cargo.BogieWheelbaseNum[1] = 2;    // 台車2台
Cargo.ChassisWheelbase[1][1] = 5;  // HIGH2 @100ms 台車間隔
Cargo.ChassisWheelbase[1][2] = 10; // HIGH1 @100ms
Cargo.ChassisWheelbase[1][3] = 15; // MID   @100ms
Cargo.ChassisWheelbase[1][4] = 20; // LOW1  @100ms
Cargo.ChassisWheelbase[1][5] = 25; // LOW2  @100ms


}
