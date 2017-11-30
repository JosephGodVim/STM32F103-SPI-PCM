#ifndef __PCM_MODE_H
#define __PCM_MODE_H
#include "sys.h"

extern int pcmbuff_sta;
extern u8 ReadDataIsReady;
extern u8 SendDataIsReady;
void PCM_GPIO_Init(void);
void OutPutToPCM_Out(void);
#endif
