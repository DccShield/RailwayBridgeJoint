#ifndef CDS_H_
#define CDS_H_

#include <stdio.h>

// Cds通過時間閾値
#define HIGH2threshold  100     //HIGH2 の通過時間
#define HIGH1threshold  500     //HIGH1 の通過時間 500ms
#define MIDthreshold    1000    //MID の通過時間 1000ms
#define LOW1threshold   2000    //LOW1 の通過時間 2000ms
#define LOW2threshold   3000    //LOW2 の通過時間 3000ms
#define CDS2_TIMEOUT    5000    //センサ2のタイムアウト時間 5000ms

// 状態基底クラス
class Cds
{
public:
  Cds(char ch1 , char ch2 , char ct , int LTh, int HTh);
  char statechk( char range );
  void Reset( void );

private:
  int LThreshold;
  int HThreshold;
  int Ain;
  int cnt;
  int cntup;
  char port1;
  char port2;
  char state;
    
  enum{
      ST_INIT = 0,
      ST_MEAS1,
      ST_MEAS2,
      ST_DETECTION,
  };

  enum{
      HIGH2 = 1,
      HIGH1,
      MID,
      LOW1,
      LOW2,
  };

};

#endif
