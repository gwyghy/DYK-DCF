#include "includes.h"

extern u8 g_u8LightState[LIGHT_NUM];
extern u16 g_Light1DurationTimer;
extern u16 g_Light2DurationTimer;

extern vu32 TimingDelay;

/*******************************************************************************************
  **function name:MosGPIO_Configuration()
  **description: Mos管版本各个管脚的配置
  **input value:
  **output value:
*******************************************************************************************/
void MosGPIO_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_MOSRECV_LED_GPIO_RCC|NMOS_CON_GPIO_RCC|RCC_APB2Periph_AFIO,ENABLE);

	GPIO_InitStructure.GPIO_Pin = MOSTX_LED_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(MOSTX_LED_PORT, &GPIO_InitStructure);
	GPIO_SetBits(MOSTX_LED_PORT,MOSTX_LED_PIN);
	
	/* Configure LED pin */
	GPIO_InitStructure.GPIO_Pin = MOSRECV_LED_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(MOSRECV_LED_PORT, &GPIO_InitStructure);
	GPIO_SetBits(MOSRECV_LED_PORT,MOSRECV_LED_PIN);
		
	GPIO_InitStructure.GPIO_Pin = NMOS_CON_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(NMOS_CON_PORT, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = MOS_VALUE_POWER_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(MOS_VALUE_POWER_PORT, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = OVERCURRENT_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(OVERCURRENT_PORT, &GPIO_InitStructure);		
}
/*******************************************************************************************
**function name:ChipGPIO_Configuration()
**description:芯片版本各个管脚的配置
  **input value:
  **output value:
*******************************************************************************************/
void ChipGPIO_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	/* 功能复用IO时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_ADC1, ENABLE);// 使能复用时钟和ADC1的时钟	

	/* USART3 Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	/* Configure USART3 pins: USART3_TX */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
 
	/*!< Configure USART3 pins: USART3_RX */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		 //浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);

		/* Configure ADC1 pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;                //模拟输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);   


	/* SPI1 Periph clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	/* Configure SPI1 pins: SCK */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
 
	/* Configure SPI1 pins: MOSI */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure SPI1 pins: MISO */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

  	/* Configure MC33996-1 CS1 pin */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
    CS1_ENABLE; //ghy 初始化时CS1管脚默认为高电平  22-5-20
	
  	/* Configure MC33996-2 CS2 pin */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
    CS2_ENABLE;//ghy 初始化时CS2管脚默认为高电平 22-5-20

  	/*!< Configure MC33996-1/2 RST pin */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);

  	/*!< Configure MC33996-1 PWM1 pin */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
  	/* Configure MC33996-2 PWM2 pin */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);

  
	/* Configure LED1/2 pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_15);
	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC,GPIO_Pin_6);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
/*******************************************************************************************
  **function name:	   SPI1_Init()
  **description:	   SPI配置初始化
  **input value:
  **output value:
*******************************************************************************************/
void SPI1_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
  
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

  	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;	//空闲时为Low
  	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//第2个时钟沿捕获数据
  	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
  	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  	SPI_InitStructure.SPI_CRCPolynomial = 7;
  	SPI_Init(SPI1, &SPI_InitStructure);
	
  	SPI_Cmd(SPI1, ENABLE);
}
/*******************************************************************************************
**function name:s_LEDStatusProc()
**description:对于CAN接收和CAN发送指示灯的处理
  **input value:
  **output value:
*******************************************************************************************/
void s_LEDStatusProc(u32 u32ID)
{
	/*
	*如果选择CAN 
	*/
	if(EVMDCAN_ENABLED)
	{
		/*
		*CAN通讯指示灯RX端口
		*/
		if(g_u8LightState[0])
		{
			g_Light1DurationTimer = RX_LIGHT_TIMER_VAL;
			__disable_irq();
			g_u8LightState[0] = 0;
			__enable_irq();
		}
		if(g_Light1DurationTimer == TIMER_EXPIRED)
		{
			g_Light1DurationTimer = TIMER_CLOSED;
			if(u32ID ==DEVICE_MOS)
			{
				NO_MOSRECV_LED;
			}
			else
			{
				NOCHIPRECV_LED;
			}	
		}
		else if((g_Light1DurationTimer !=TIMER_EXPIRED) &&(g_Light1DurationTimer != TIMER_CLOSED))
		{
			if(u32ID == DEVICE_MOS)
			{
				MOSRECV_LED;
			}
			else
			{
				CHIPRECV_LED;
			}	
		}
		/*
		*CAN通讯指示灯TX端口
		*/
		if(g_u8LightState[1])
		{
			g_Light2DurationTimer = TX_LIGHT_TIMER_VAL;
			__disable_irq();
			g_u8LightState[1] = 0;
			__enable_irq();
		}
		if(g_Light2DurationTimer == TIMER_EXPIRED)
		{
			g_Light2DurationTimer = TIMER_CLOSED;
			if(u32ID ==DEVICE_MOS)
			{
				NO_MOSTX_LED;
			}
			else
			{
				NOCHIPTX_LED;
			}		
		}
		else if((g_Light2DurationTimer !=TIMER_EXPIRED) &&(g_Light2DurationTimer != TIMER_CLOSED))
		{
			if(u32ID ==DEVICE_MOS)
			{
				MOSTX_LED;
			}
			else
			{
				CHIPTX_LED;
			}	
		}	
	}	
}
void Delay(u16 time)
{
	TimingDelay = time;
	while(TimingDelay !=0);
}

