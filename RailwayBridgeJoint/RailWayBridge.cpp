#include "RailwayBridge.h"
#include "jointini.h"
#include "cds.h"

void RailWayBridgeState( JOINT ad ){
  static Cds TrainSensor(0,1,2,200,500);    // CDSセンサのセットアップ
  static int state = ST_INIT;
  static int TrainVal = 0;
  static int wait = 0;
  int re;
  
  switch(state){
    case ST_INIT:
Serial.println("Railway:ST_INIT");
                  TrainVal = ad.RollingStock[0];
                  digitalWrite(LED_BUILTIN,LOW);
                  state = ST_TRSENSING;
                  break;
    case ST_TRSENSING:      // 車両侵入チェック
                   re = TrainSensor.statechk(LOW); // 明るさ閾値LOWでチェック
                  if( re == 0 )
                    break;
                  else {        
                    TrainSensor.Reset();
                    gSound_played = re;
                    wait = ad.TrainWait;
                    state = ST_STTRAINWAIT;
                  }
                  break;
    case ST_STTRAINWAIT:
                  digitalWrite(LED_BUILTIN,HIGH);           // 再生中
                  wait--;
                  if(wait<=0)
                    state = ST_TRAIN;
                  break;   
    case ST_TRAIN:
                  if(TrainVal < 1){
                    state  = ST_INIT;
                    break;  
                  } 
                  TrainVal--;
                  state = ST_WHEELBASE1;
                  break;                  
    case ST_WHEELBASE1:             // 最初の台車のガタンガタン
                  ImaAdpcmPlay();   // ジョイント音発生依頼
                  wait = ad.ChassisWheelbase[0][gSound_played];  //台車間隔時間を算出
Serial.println(gSound_played);
Serial.println(wait);
                  state = ST_CHASSILENWAIT;
                  break;  
    case  ST_CHASSILENWAIT:
                  wait--;
                  if(wait<=0)
                    state = ST_WHEELBASE2;
                  break;                      
    case ST_WHEELBASE2:             // 2番目の台車のガタンガタン
                  ImaAdpcmPlay();   // ジョイント音発生依頼
                  if( ad.BogieWheelbaseNum[0] == 3){          // 台車が3台ある？
                    wait = ad.ChassisWheelbase[0][gSound_played];  //台車間隔時間を算出
 Serial.println(wait);
                    state = ST_CHASSILENWAIT2;
                    break;
                  }
                  wait = ad.VehicleSpace[0][gSound_played];  //車両間隔時間を算出

                  state = ST_TRAINWAIT;                       
                  break;  

    case  ST_CHASSILENWAIT2:
                  wait--;
                  if(wait<=0)
                    state = ST_WHEELBASE3;
                  break;    

    case ST_WHEELBASE3:             // 3番目の台車のガタンガタン
                  ImaAdpcmPlay();   // ジョイント音発生依頼
                  wait = ad.VehicleSpace[0][gSound_played];  //車両感覚時間を算出
                  state = ST_TRAINWAIT;                       
                  break;  
    case ST_TRAINWAIT:
                  wait--;
                  if(wait<=0)
                    state = ST_TRAIN;
                  break;                         
    default:
                  break;
  }
}
