#include <avr/pgmspace.h>
#include "RailwayBridge.h"
#include "ima_adpcm.h"


#include <Arduino.h>

void ImaAdpcmPlay(void)
{
  gJointFlg = 1;
}


//================================================================================
// void ImaAdpcmSetup()
// IMA ADPCMからヘッダ情報を取り込む
//================================================================================
void ImaAdpcmSetup()
{
  //PWM出力ピン D9を出力にセット
  pinMode(VVVF_SOUND_PIN, OUTPUT);
  
  //Timer1使用
  //Timer1.initialize();
  //初期設定
  //Timer1.pwm(VVVF_SOUND_PIN,0,0);
  //Timer1.attachInterrupt(vvvf_int1,current_period1 / 2);//interruptの設定    
  //31KHzに設定（初期設定でPhaseCorrectPWMモードになっていると思う）
  //D9,D10  キャリア周期:31kHz(分周無し）
  TCCR1B &= B11111000;
  TCCR1B |= B00000001;

  //出力の設定
  if(VVVF_SOUND_PIN == 9)
  {
    TCCR1A |= _BV(COM1A1);
  }
  else //10
  {
    TCCR1A |= _BV(COM1B1);
  }
  //interrupt設定
  TIMSK1 = _BV(TOIE1);
}

//================================================================================
// IMA ADPCM 再生割り込み
// 32kHz で割り込みが入るけど、16kHzで再生する様に細工している
//================================================================================
ISR(TIMER1_OVF_vect)
{
  enum{
      ST_IDLE = 0,
      ST_INIT,
      ST_PLAY,
  };

  static char state = ST_IDLE;
  static uint8_t hilow = LOW;   // add aya:最初は下位
  static boolean flg = true;

  uint8_t t;

  flg = !flg;     //32kHzに変更のため　サンプリングは16kHzだから、1回はお帰りしてもらってんだ。
  if(flg){
    return;
  }

  switch( state ){
    case ST_IDLE:
                  if( gJointFlg == ON){         // グローバル変数gJointFlg = ON になったら、１発ジョイント音発生
                    if (VVVF_SOUND_PIN == 9){   // 一旦音を止める。
                      OCR1A=0;
                    }
                    else//(VVVF_SOUND_PIN == 10)
                    {
                      OCR1B = 128;   // 0->128 プチ音防止
                    }
                    gJointFlg = OFF;           // ジョイントフラグOFF
                    state = ST_INIT;
                  }
                  break;
    case ST_INIT: //IMAADPCM初期設定
                  gSound_playnext = 1;
                  sample = 64;//64;      // dataまでのオフセット
                  ndata = 0;        // IMA ADPCM の block を初期化
                  nBlockAlign = (uint16_t)(readSoundRom(gSound_played, 33) << 8 | readSoundRom(gSound_played, 32)) - 4;//周波数によってブロックサイズが違う。
                  gSound_Length = (uint16_t)(readSoundRom(gSound_played, 57) << 8 | readSoundRom(gSound_played, 56)) - 13 * 20; // RIFF No56,No57
                  imad.val = (int16_t)(readSoundRom(gSound_played, 61) << 8 | readSoundRom(gSound_played, 60));   // Get 1st sample value of the block
                  imad.idx = readSoundRom(gSound_played, 62);

//char temp[50];
//sprintf(temp,"nBlock:%d , gSound_Length:%d , ima.val:%d , ima.idx:%d", nBlockAlign,gSound_Length,imad.val,imad.idx);
//Serial.println(temp);

                  
                  state = ST_PLAY;
                  break;
    case ST_PLAY:

                  if (ndata == nBlockAlign){    // 11/9 1 block 再生終わったら、val,idxを再セットする


                  if( gJointFlg == ON){  
                  state = ST_IDLE;
                  break;
                 }
                    
                    ndata = 0;
                    imad.val = (int16_t)(readSoundRom(gSound_played, sample + 1) << 8 | readSoundRom(gSound_played, sample)); // Get 1st sample value of the block
                    imad.idx = readSoundRom(gSound_played, sample + 2);
                    sample += 4;
                  }

                  if (sample >= gSound_Length){   // 最後まで再生した？
                    state = ST_IDLE;

                  }

                  t = ima_decode(&imad, readSoundRom(gSound_played, sample), hilow);  // Sound extension by MECY 2017/08/02

//Serial.println(t,DEC);


                  if (VVVF_SOUND_PIN == 9){
                    OCR1A = t;
                  } else {//(VVVF_SOUND_PIN == 10)
                    OCR1B = t;
                  }

                  /* ADPCMの4bit境界切り替え処理 */
                  if (hilow == HIGH){   // add aya : IMA ADPCM は1byteに4bit*2のデータがあるからね。
                    hilow = LOW;
                    sample++;     // 再生アドレスをインクリメント
                    ndata++;      // IMA ADPMC 1BLOCKカウントをインクリメント
                  } else {
                    hilow = HIGH;
                  }
                  break;
  default:
                  break;
  }  
}

//------------------------------------------------------------------------------
//  ROMに配置されている変数から値を取り出す処理
//------------------------------------------------------------------------------
byte readSoundRom(uint8_t inNo, uint16_t inOffset)
{
  switch (inNo) {
    case 1:
      return pgm_read_byte(&RailwayBridge12_data[inOffset]);  // 1.5倍ジョイント音
      break;
    case 2:
      return pgm_read_byte(&RailwayBridge12_data[inOffset]);  // 1.2倍ジョイント音
      break;
    case 3:
      return pgm_read_byte(&RailwayBridge01_data[inOffset]);  // 1倍ジョイント音
      break;
    case 4:
      return pgm_read_byte(&RailwayBridge08_data[inOffset]);  // 0.8倍ジョイント音
      break;
    case 5:
      return pgm_read_byte(&RailwayBridge05_data[inOffset]);  // 0.5倍ジョイント音
      break;
    default:
      return 128;//無音
      return 0;
      break;
  }
}

//------------------------------------------------------------------------------------
// ChaN さんのIMA ADPCM decoder を流用

/*-------------------------------------------------*/
/* IMA ADPCM Decoder                               */

//uint8_t ima_decode(    /* Returns the sample value (-32768 to 32767) */
uint16_t ima_decode(    /* Returns the sample value (-32768 to 32767) / Sound extension by MECY 2017/01/14 */
  IMADEC* ad,     /* Work area for the ADPCM stream */
  uint8_t dat,      /* ADPCM data, b3:sign (0:+, 1:-), b2-b0:magnify (0..7) */
  uint8_t hl      /* High or low data read */
)
{
  /*
    char buffer[20];
    sprintf( buffer,"%d,",sample);
    Serial.print(buffer);
  */
  static const uint16_t step[89] __attribute__((__progmem__)) = {
    //  static const uint16_t step[89] = {
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 41, 45, 50, 55, 60, 66,
    73, 80, 88, 97, 107, 118, 130, 143, 157, 173, 190, 209, 230, 253, 279, 307, 337, 371, 408, 449,
    494, 544, 598, 658, 724, 796, 876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066, 2272,
    2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358, 5894, 6484, 7132, 7845, 8630, 9493,
    10442, 11487, 12635, 13899, 15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
  };

  int32_t dif;
  uint16_t stp;
  uint8_t idx;

  if (hl == HIGH)   // hl のフラグで、HIGHだったら、上位4bitを下位に移動
    dat = dat >> 4;
  dat = dat & 0x0f;
  /*
    sprintf( buffer,"%d,",dat);
    Serial.print(buffer);
  */
  /* Get current step size */
  idx = ad->idx;            // scale
  stp = pgm_read_word(&step[idx]);
  //  stp = step[idx];

  dif = stp >> 3;   // dif = stp / 8 と等価
  if (dat & 4) dif += stp;
  stp >>= 1;
  if (dat & 2) dif += stp;
  stp >>= 1;
  if (dat & 1) dif += stp;
  if (dat & 8) dif = 1 - dif;
  /* Add the difference to the previous sample */
  dif += ad->val;

  if (dif >  32767) dif =  32767;
  if (dif < -32768) dif = -32768;
  ad->val = (int16_t)dif;

  /* Update step size by magnify */
  dat = (dat & 7) - 4;

  if (dat > 3) {  /* Decrease step size by 1 if magnify is 0..3 */
    if (idx) idx--;
  } else {    /* Increase step size by 2..8 if magnify is 4..7 */
    idx += (dat + 1) << 1;
    if (idx > 88) idx = 88;
  }
  ad->idx = idx;

  //sprintf( buffer,"%#4x:%d",dif,dif);  要素が２つあると、ちゃんと変換できない
  //Serial.println(buffer);
  /*
    sprintf( buffer,"%d,",ad->val);
    Serial.print(buffer);
    sprintf( buffer,"%d,",ad->val/256+128);
    Serial.print(buffer);
    sprintf( buffer,"%d,",idx);
    Serial.println(buffer);
  */
  return ad->val / 256 + 128; /* PCM化した16bitのデータを8bitに間引いて、128のオフセットを履かせています*/
}
//------------------------------------------------------------------------------------
