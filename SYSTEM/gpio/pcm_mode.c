#include "pcm_mode.h"
#include "usart.h"
#include "delay.h"
#include "dac.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define PCM_CLK     GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)
#define PCM_SYNC    GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)

u8 pcmbuff[40960];
u8 SYNCIsReady = 0;
u8 SendDataIsReady = 0;
u8 SaveDataIsReady = 0;
u8 SaveRc = 0;
u8 SendRc = 0;

int i = 7, pcmbuff_sta = 0;

void PCM_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //PCM_CLK
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //PCM_SYNC
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //PCM_IN
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //PCM_OUT
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //LED
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA,GPIO_Pin_8);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //KEY0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1);
    EXTI_InitStructure.EXTI_Line = EXTI_Line1;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource5);
    EXTI_InitStructure.EXTI_Line = EXTI_Line5;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void EXTI0_IRQHandler(void) //PCM_CLK
{
    if(PCM_CLK == 0) {
    if(SaveDataIsReady&&SYNCIsReady) {
        SaveRc = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5);
        pcmbuff[pcmbuff_sta]|=SaveRc<<i;
        i--;
        if(i < 0) {
            i = 7;
            pcmbuff_sta++;
            if(pcmbuff_sta == 40960) {
                SaveDataIsReady = 0;
                SendDataIsReady = 1;
                i = 7;
                pcmbuff_sta = 0;
            }
        }
    }
    }
    
    if(PCM_CLK) {
    if(SendDataIsReady&&SYNCIsReady) {
        SendRc = pcmbuff[pcmbuff_sta]>>i&0x01;
        if(SendRc) {
            GPIO_SetBits(GPIOA, GPIO_Pin_6);
        }
        else {
            GPIO_ResetBits(GPIOA, GPIO_Pin_6);
        }
        i--;
        if(i < 0) {
            i = 7;
            pcmbuff_sta++;
            if(pcmbuff_sta == 40000) {
                pcmbuff_sta = 0;
            }
        }
    }
    }
    EXTI_ClearITPendingBit(EXTI_Line0);
}

void EXTI1_IRQHandler(void) //PCM_SYNC
{
    SYNCIsReady = 1;
    EXTI_ClearITPendingBit(EXTI_Line1);
}

void EXTI9_5_IRQHandler(void)
{
    SaveDataIsReady = 1;
    SendDataIsReady = 0;
    EXTI_ClearITPendingBit(EXTI_Line5);
}