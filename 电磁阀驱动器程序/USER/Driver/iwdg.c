#include "includes.h"


/********************************************************************************************
* �������ƣ�IWDG_Configuration ()
* �������������Ź���ʼ��
* ��ڲ�������
* ���ڲ�������
* ʹ��˵������
********************************************************************************************/
void IWDG_Configuration(void)
{
	/*Enable the LSI OSC 32khz*/
	RCC_LSICmd(ENABLE);
	/*wait till LSI is Ready*/
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
	{
	}
	/* ʹ�ܼĴ��� IWDG_PR �� IWDG_RLR д���� */
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	/* ���Ź���Ƶʱ��Ϊ: LSI/256 *//* �ڲ�����ʱ��256��Ƶ 40K/256=156HZ(6.4ms) */
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	/* ���ÿ��Ź�������ֵ.5S*//* ι��ʱ�� 5s/6.4MS=781 .ע�ⲻ�ܴ���0xfff*/
	IWDG_SetReload(781);
	/* ι�� */
	IWDG_ReloadCounter();
	/* ʹ�ܿ��Ź� */
	IWDG_Enable();
}


/*******************************************************************************************
  **function name:	   IWDG_Feed()
  **description:	   "ι��"����
  **input value:
  **output value:
*******************************************************************************************/
void IWDG_Feed(void)
{
	IWDG_ReloadCounter();
}





