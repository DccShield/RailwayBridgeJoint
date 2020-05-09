#ifndef JOINT_H_
#define JOINT_H_

#include <Arduino.h>

typedef struct {
  uint8_t type;                   // 0:電車 1:機関車 2:タンク系 
  uint8_t TrainWait;              // センサを踏んでから音がなるまでのウエイト
  uint8_t RollingStock[2];        // 車両数　0:電車の車両数 1:貨物の車両数
  uint8_t VehicleSpace[2][6];     // 車両間隔  1-6 に速さによるパラメータを設定
  uint8_t BogieWheelbaseNum[2];   // 台車数 0:電車の車両数 1:貨物の車両数
  uint8_t ChassisWheelbase[5][6]; // 車両の軸距 1-6 に速さによるパラメータを設定
} JOINT;

#endif
