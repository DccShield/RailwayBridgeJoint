#ifndef IMA_ADPCM_H_
#define IMA_ADPCM_H_

#define VVVF_SOUND_PIN 10
#define OFF 0
#define ON 1

// IMA　ADPCM typedef
typedef struct {  /* Decoder work area of an IMA-ADPCM stream */
  int16_t val;    /* Previous sample value (-32768..32768) */
  uint8_t idx;    /* Current index of step size (0..88) */
} IMADEC;

IMADEC imad;

uint16_t ima_decode(    /* Returns the sample value (-32768 to 32767) */
  IMADEC* ad,           /* Work area for the ADPCM stream */
  uint8_t dat,          /* ADPCM data, b3:sign (0:+, 1:-), b2-b0:magnify (0..7) */
  uint8_t hl            /* High or low data read */
);



//IMA WAV関連転記
//Sound played flag
volatile uint8_t gSound_played = 0;
uint8_t gSound_playnext = 0;
int gSound_Length = 0;
int gSound_offset = 0;          // aya add  RIFF フォーマットをそのまま使っているのでdata chunkまでoffsetを履かせる
int sample;
uint16_t ndata;                 // 11/9 block数をカウント
byte lastSample;                //
uint16_t nBlockAlign = 252;
uint8_t gJointFlg;

uint8_t readSoundRom(uint8_t inNo, uint16_t inOffset);


#endif
