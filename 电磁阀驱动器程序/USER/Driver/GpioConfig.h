#ifndef __GPIOCONFIG_H
#define __GPIOCONFIG_H
#include "includes.h"

/****************************************************/
#define MOSTX_LED_PORT	 	 GPIOA
#define MOSTX_LED_PIN 		 GPIO_Pin_7
#define RCC_MOSTX_LED_GPIO_RCC       RCC_APB2Periph_GPIOA

#define	MOSRECV_LED_PORT    GPIOC
#define MOSRECV_LED_PIN 	 GPIO_Pin_5
#define RCC_MOSRECV_LED_GPIO_RCC  	 RCC_APB2Periph_GPIOC


#define NMOS_CON_PORT   	   GPIOA
#define NMOS_CON_PIN   	       GPIO_Pin_5
#define NMOS_CON_GPIO_RCC      RCC_APB2Periph_GPIOA	

#define MOS_VALUE_POWER_PORT   	   GPIOB
#define MOS_VALUE_POWER_PIN   	   GPIO_Pin_12
#define MOS_VALUEPOWER_GPIO_RCC    RCC_APB2Periph_GPIOB	

#define OVERCURRENT_PORT   		 GPIOB
#define OVERCURRENT_PIN   		 GPIO_Pin_1
#define OVERCURRENT_GPIO_RCC  	 RCC_APB2Periph_GPIOB	


#define RX_LIGHT_TIMER_VAL  	1  //CAN接收指示灯1持续时长
#define TX_LIGHT_TIMER_VAL  	1 //CAN发送指示灯持续时长

#define MOSRECV_LED   	 	GPIO_ResetBits(MOSRECV_LED_PORT,MOSRECV_LED_PIN)
#define MOSTX_LED  		    GPIO_ResetBits(MOSTX_LED_PORT,MOSTX_LED_PIN)

#define NO_MOSRECV_LED     GPIO_SetBits(MOSRECV_LED_PORT,MOSRECV_LED_PIN)
#define NO_MOSTX_LED  	   GPIO_SetBits(MOSTX_LED_PORT,MOSTX_LED_PIN)

#define NMOS_CON_On			GPIO_SetBits(NMOS_CON_PORT,NMOS_CON_PIN)
#define NMOS_CON_Off		GPIO_ResetBits(NMOS_CON_PORT,NMOS_CON_PIN)

#define MosValue_PowerOn   GPIO_SetBits(MOS_VALUE_POWER_PORT,MOS_VALUE_POWER_PIN)
#define MosValue_PowerOff  GPIO_ResetBits(MOS_VALUE_POWER_PORT,MOS_VALUE_POWER_PIN)


/*******************************************************/
#define NOCHIPRECV_LED      GPIO_SetBits(GPIOB,GPIO_Pin_15)
#define NOCHIPTX_LED        GPIO_SetBits(GPIOC,GPIO_Pin_6)
#define CHIPRECV_LED       	GPIO_ResetBits(GPIOB,GPIO_Pin_15)
#define CHIPTX_LED       	GPIO_ResetBits(GPIOC,GPIO_Pin_6)
#define ChipVale_PowerOn  	GPIO_SetBits(GPIOB,GPIO_Pin_1);
#define ChipVale_PowerOff 	GPIO_ResetBits(GPIOB,GPIO_Pin_1);

#define CS1_ENABLE   	GPIO_SetBits(GPIOB,GPIO_Pin_5)
#define CS1_DISABLE  	GPIO_ResetBits(GPIOB,GPIO_Pin_5)
#define CS2_ENABLE   	GPIO_SetBits(GPIOB,GPIO_Pin_6)
#define CS2_DISABLE  	GPIO_ResetBits(GPIOB,GPIO_Pin_6)
#define MC_RST       	GPIO_ResetBits(GPIOD,GPIO_Pin_2)
#define MC_NO_RST    	GPIO_SetBits(GPIOD,GPIO_Pin_2)


void s_LEDStatusProc(u32 u32ID);
void MosGPIO_Configuration(void);
void ChipGPIO_Configuration(void);
void SPI1_Init(void);
void Delay(u16 time);
#endif 
