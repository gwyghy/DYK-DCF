#include "includes.h"

extern u32 Hare_Id; //定义硬件ID识别
u8 HareId_Init(void)
{
	u32 i;
	u32 j;
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(ID_GPIO_RCC,ENABLE);
		
	/* Configure ID pin */
	GPIO_InitStructure.GPIO_Pin = ID0_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(ID_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ID1_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(ID_PORT, &GPIO_InitStructure);
	
	i =GPIO_ReadInputData(ID_PORT);
	i =GPIO_ReadInputData(ID_PORT);
	i =GPIO_ReadInputData(ID_PORT);
	i &=(ID0_PIN |ID1_PIN);
	Hare_Id = i;
}