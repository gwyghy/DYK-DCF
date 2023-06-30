#ifndef __HARD_ID_H
#define __HARD_ID_H

#include "stm32f10x.h"

#define ID_PORT     		GPIOB
#define ID_GPIO_RCC 		RCC_APB2Periph_GPIOB
#define ID0_PIN        		GPIO_Pin_10
#define ID1_PIN					GPIO_Pin_11

#define DEVICE_MOS	 0x00   //�豸����ΪMOS��
	
u8 HareId_Init(void);
#endif
