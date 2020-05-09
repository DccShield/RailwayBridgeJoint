#include  <arduino.h>
#include "cds.h"

Cds::Cds(char ch1 , char ch2 , char ct, int LTh, int HTh){
  port1 = ch1;
  port2 = ch2;
  cntup = ct;
  LThreshold = LTh;
  HThreshold = HTh;
}

void Cds::Reset( void ){
  state = ST_INIT;
}

char Cds::statechk( char range ){
  static unsigned long PreviosTimerSp;  // 速度監視用変数
  static unsigned long TimTemp;
    
  switch(state){
    case ST_INIT:
Serial.println("cds:ST_INIT");
              state = ST_MEAS1;
              cnt = 0;
              TimTemp = 0;
              break;
    case ST_MEAS1:
              if(cnt > cntup ){
                state = ST_MEAS2;
                cnt = 0;
                PreviosTimerSp = millis();
                break;
              }
              Ain = analogRead( port1 );
              if(range == LOW){
                if(Ain <=  LThreshold){
                  cnt++;
                } else
                  cnt = 0;
              }
              if(range == HIGH){
                if(Ain <=  HThreshold){
                  cnt++;
                } else
                  cnt = 0;
              }
              break;
    case ST_MEAS2:
              if(cnt > cntup ){
                state = ST_DETECTION;
                cnt = 0;
                TimTemp = millis() - PreviosTimerSp;
                break;
              }
              if(cnt == 0){
                if(millis() -  PreviosTimerSp >= CDS2_TIMEOUT){   // センサ2タイムアップ 5秒
Serial.println("cds:ST_MEAS2:Timeup");
                  state = ST_INIT;
                  break; 
                }
              }
              Ain = analogRead( port2 );
              if(range == LOW){
                if(Ain <=  LThreshold){
                  cnt++;
                } else {
                  cnt = 0;
                }
              }
              if(range == HIGH){
                if(Ain <=  HThreshold){
                  cnt++;
                } else {
                  cnt = 0;
                }
              }
              break;            
    case ST_DETECTION:
Serial.print("cds:ST_DETECTION:");
Serial.println(TimTemp);

              if(TimTemp >= LOW2threshold )
                return LOW2;
              else if(TimTemp >= LOW1threshold )
                return LOW1;
              else if(TimTemp >= MIDthreshold  )
                return MID;
              else if(TimTemp >= HIGH1threshold )
                return HIGH1;
              else if(TimTemp >= HIGH2threshold )
                return HIGH2;
              break;

    default:
              break;
  }
  return 0;
}
