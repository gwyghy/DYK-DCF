#include "includes.h"
void RCC_Configuration(void)
{
	ErrorStatus  HSEStartUpStatus;
	
	/*������RCC�Ĵ�������Ϊȱʡֵ */
	RCC_DeInit();
	
	/*�����ⲿ���پ���HSE��*/
	RCC_HSEConfig(RCC_HSE_ON);//RCC_HSE_Bypass);   //RCC_HSE_ON--HSE�����(ON) //16M
	
	/*�ȴ�HSE����*/
	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	
	if(HSEStartUpStatus == SUCCESS)	  //SUCCESS��HSE�����ȶ��Ҿ���
	{
		/*����AHBʱ�ӣ�HCLK��*/ 
		RCC_HCLKConfig(RCC_SYSCLK_Div1);  //RCC_SYSCLK_Div1--AHBʱ�� = ϵͳʱ��	//16M
		
		/* ���ø���AHBʱ�ӣ�PCLK2��*/ 
		RCC_PCLK2Config(RCC_HCLK_Div1);	//RCC_HCLK_Div1--APB2ʱ�� = HCLK	//16M
		
		/*���õ���AHBʱ�ӣ�PCLK1��*/    
		RCC_PCLK1Config(RCC_HCLK_Div2);	 //RCC_HCLK_Div2--APB1ʱ�� = HCLK / 2	//8M
		
		/*����FLASH�洢����ʱʱ��������*/
//		FLASH_SetLatency(FLASH_Latency_2);    //FLASH_Latency_2  2��ʱ����
		
		/*ѡ��FLASHԤȡָ�����ģʽ*/	
//		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);	// Ԥȡָ����ʹ��
		
		/*����PLLʱ��Դ����Ƶϵ��*/	
		//RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_4);	
		RCC_PLLConfig(RCC_PLLSource_HSE_Div2, RCC_PLLMul_9);	//72MHz	
		// PLL������ʱ�� = HSEʱ��Ƶ�ʣ�RCC_PLLMul_9--PLL����ʱ��x 9   16/2*9=72M
		
		/*ʹ��PLL */
		RCC_PLLCmd(ENABLE); 
		
		/*���ָ����RCC��־λ(PLL׼���ñ�־)�������*/   
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)	  
		{
		}
		
		/*����ϵͳʱ�ӣ�SYSCLK�� */ 
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); 
		//RCC_SYSCLKSource_PLLCLK--ѡ��PLL��Ϊϵͳʱ��
		
		/* PLL��������ϵͳʱ�ӵ�ʱ��Դ*/
		while(RCC_GetSYSCLKSource() != 0x08)	  //0x08��PLL��Ϊϵͳʱ��
		{ 
		}
	}
	
	/*ʹ�ܻ���ʧ��APB2����ʱ��*/	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE); 
	
}
/*******************************************************************************************
  **function name:	   SysTick_Configuration()
  **description:	   ��શ�ʱ���ĳ�ʼ��
  **input value:
  **output value:
*******************************************************************************************/
void SysTick_Configuration(void)
{
	if(SysTick_Config(SystemCoreClock / 100)) //10ms
	{
		while(1);
	}
}
