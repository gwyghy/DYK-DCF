#include "includes.h"
const u16 OutPoint_GpioPinBuf[]=
{
	OUTPOINT_1_GPIO_PIN,OUTPOINT_2_GPIO_PIN,OUTPOINT_3_GPIO_PIN,OUTPOINT_4_GPIO_PIN,
	OUTPOINT_5_GPIO_PIN,OUTPOINT_6_GPIO_PIN,OUTPOINT_7_GPIO_PIN,OUTPOINT_8_GPIO_PIN,
	OUTPOINT_9_GPIO_PIN,OUTPOINT_10_GPIO_PIN,OUTPOINT_11_GPIO_PIN,OUTPOINT_12_GPIO_PIN,
	OUTPOINT_13_GPIO_PIN,OUTPOINT_14_GPIO_PIN,OUTPOINT_15_GPIO_PIN,OUTPOINT_16_GPIO_PIN,
	
	OUTPOINT_17_GPIO_PIN,OUTPOINT_18_GPIO_PIN,OUTPOINT_19_GPIO_PIN,OUTPOINT_20_GPIO_PIN,
	OUTPOINT_21_GPIO_PIN,OUTPOINT_22_GPIO_PIN,OUTPOINT_23_GPIO_PIN,OUTPOINT_24_GPIO_PIN,
	OUTPOINT_25_GPIO_PIN,OUTPOINT_26_GPIO_PIN,OUTPOINT_27_GPIO_PIN,OUTPOINT_28_GPIO_PIN,
	OUTPOINT_29_GPIO_PIN,OUTPOINT_30_GPIO_PIN,OUTPOINT_31_GPIO_PIN,OUTPOINT_32_GPIO_PIN
};
GPIO_TypeDef * const OutGpioPortBuf[]=
{
	OUTPOINT_1_GPIO_PORT,OUTPOINT_2_GPIO_PORT,OUTPOINT_3_GPIO_PORT,OUTPOINT_4_GPIO_PORT,
	OUTPOINT_5_GPIO_PORT,OUTPOINT_6_GPIO_PORT,OUTPOINT_7_GPIO_PORT,OUTPOINT_8_GPIO_PORT,
	OUTPOINT_9_GPIO_PORT,OUTPOINT_10_GPIO_PORT,OUTPOINT_11_GPIO_PORT,OUTPOINT_12_GPIO_PORT,
	OUTPOINT_13_GPIO_PORT,OUTPOINT_14_GPIO_PORT,OUTPOINT_15_GPIO_PORT,OUTPOINT_16_GPIO_PORT,
	
	OUTPOINT_17_GPIO_PORT,OUTPOINT_18_GPIO_PORT,OUTPOINT_19_GPIO_PORT,OUTPOINT_20_GPIO_PORT,
	OUTPOINT_21_GPIO_PORT,OUTPOINT_22_GPIO_PORT,OUTPOINT_23_GPIO_PORT,OUTPOINT_24_GPIO_PORT,
	OUTPOINT_25_GPIO_PORT,OUTPOINT_26_GPIO_PORT,OUTPOINT_27_GPIO_PORT,OUTPOINT_28_GPIO_PORT,
	OUTPOINT_29_GPIO_PORT,OUTPOINT_30_GPIO_PORT,OUTPOINT_31_GPIO_PORT,OUTPOINT_32_GPIO_PORT,
};
void OutPoint_Init(void)
{
	u8 u8i = 0;
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(OUTPOINT_GPIO_RCC_1 | OUTPOINT_GPIO_RCC_2 | OUTPOINT_GPIO_RCC_3 | OUTPOINT_GPIO_RCC_4|RCC_APB2Periph_AFIO , ENABLE); 
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//关闭jtag,使能SWD,可以使用SWD模式调试
	for(u8i=0;u8i<32;u8i++)
	{
		GPIO_InitStructure.GPIO_Pin = OutPoint_GpioPinBuf[u8i];
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(OutGpioPortBuf[u8i],&GPIO_InitStructure);
		GPIO_ResetBits(OutGpioPortBuf[u8i],OutPoint_GpioPinBuf[u8i]);
	}	
}
