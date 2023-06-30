#include "includes.h"


/********************************************************************************************
* 函数名称：IWDG_Configuration ()
* 功能描述：看门狗初始化
* 入口参数：无
* 出口参数：无
* 使用说明：无
********************************************************************************************/
void IWDG_Configuration(void)
{
	/*Enable the LSI OSC 32khz*/
	RCC_LSICmd(ENABLE);
	/*wait till LSI is Ready*/
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
	{
	}
	/* 使能寄存器 IWDG_PR 和 IWDG_RLR 写访问 */
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	/* 看门狗分频时钟为: LSI/256 *//* 内部低速时钟256分频 40K/256=156HZ(6.4ms) */
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	/* 设置看门狗的重载值.5S*//* 喂狗时间 5s/6.4MS=781 .注意不能大于0xfff*/
	IWDG_SetReload(781);
	/* 喂狗 */
	IWDG_ReloadCounter();
	/* 使能看门狗 */
	IWDG_Enable();
}


/*******************************************************************************************
  **function name:	   IWDG_Feed()
  **description:	   "喂狗"函数
  **input value:
  **output value:
*******************************************************************************************/
void IWDG_Feed(void)
{
	IWDG_ReloadCounter();
}





