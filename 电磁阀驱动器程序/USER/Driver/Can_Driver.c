
/******************************************************************************************
* 文件名：	  	Can_Driver.c
* 作者：    		沈万江
* 版本：   		V1.0
* 日期：      	2014.10.22
* 功能描述:   	硬件初始话及CAN模块收发中断
* 硬件信息：		
* 使用注意：	
* 修改说明：   
*
*       >>>>  在工程中的位置  <<<<
*             3-应用层
*             2-协议层
*          √   1-硬件驱动层
*********************************************************************************************
* @copy
* <h2><center>&copy; COPYRIGHT 天津华宁电子有限公司 研发中心 软件部</center></h2>
*********************************************************************************************/
/****************************************头文件包含******************************************/
#include "includes.h"
#include "Can_Driver.h"

/****************************************变量定义******************************************/
sCanComProtocolFrm  s_EmvdCanRxQueue[EMVD_RXCAN_SIZE]; 
CanTxMsg   		    s_EmvdCanTxQueue[EMVD_TXCAN_SIZE];

extern u16  CANRxTimer;
/***存储CAN接收数据***/
u16  s_u16EmvdCanRxQueueHeadPtr;
u16  s_u16EmvdCanRxQueueEndPtr;
u16  s_u16EmvdCanRxQueueCnt;

/***存储CAN发送数据***/
u16  s_u16EmvdCanTxQueueHeadPtr;
u16  s_u16EmvdCanTxQueueEndPtr;
u16  s_u16EmvdCanTxQueueCnt;

u8 g_u8LightState[LIGHT_NUM];

/********************************************************************************************
** 函数名	：	 	SetCANSpeed
** 输　入	：  	
** 输　出	： 		无
** 功能描述	：	配置CAN通信速率
** 作　者	：	 	沈万江
** 日　期	：  	2014.10.22
** 版本		：    	V1.0A
** 更新记录	：
** 					日    期      姓    名                    描      述
** 					==========  =============  ========================================
**
*********************************************************************************************/
void SetCANSpeed(CAN_TypeDef *CANx, u16 u16CANxSpeed)
{
	CAN_InitTypeDef CAN_InitStructure;
    RCC_ClocksTypeDef  rcc_clocks; 
	/*
	 * 复位CANx结构初始化
	 */
	CAN_DeInit(CANx);
	CAN_StructInit(&CAN_InitStructure);

	/*
	 * CANx结构初始化配置
	 */
	CAN_InitStructure.CAN_TTCM = DISABLE;			//禁止时间触发
	CAN_InitStructure.CAN_ABOM = ENABLE;			//离线自动恢复
	CAN_InitStructure.CAN_AWUM = ENABLE;			//自动唤醒
	CAN_InitStructure.CAN_NART = ENABLE;			//禁止重传
	CAN_InitStructure.CAN_RFLM = DISABLE;			//接收溢出不锁定
	CAN_InitStructure.CAN_TXFP = ENABLE;			//优先级由发送请求的顺序来决定
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;	//;CAN_Mode_LoopBack	
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;  
	CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
	
    RCC_GetClocksFreq(&rcc_clocks);					//调用标准库函数，获取系统时钟。
	/*
	 * 设置波特率
	 */
	CAN_InitStructure.CAN_Prescaler = rcc_clocks.PCLK1_Frequency / 1000 / u16CANxSpeed / (3+CAN_InitStructure.CAN_BS1+CAN_InitStructure.CAN_BS2);
	CAN_Init(CANx, &CAN_InitStructure);				//复位CANx 
}

/********************************************************************************************
** 函数名	：	 	CanConfig
** 输　入	：	
** 输　出	：		无
** 功能描述	：		CAN配置
** 作　者	：		沈万江
** 日　期	：		2014.10.22
** 版本		：		V1.0A
** 更新记录	：
** 					日    期      姓    名                    描      述
** 					==========  =============  ========================================
**
*********************************************************************************************/

void CanConfig(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
 
	RCC_APB2PeriphClockCmd(EMVD_CAN_GPIO_RCC | RCC_APB2Periph_AFIO, ENABLE);	  //使能PA口，PB口，复用功能时钟
     
	/*
	 * CAN端口设置
	 */
	/* Configure CAN1 RX pin */																	
	GPIO_InitStructure.GPIO_Pin = EMVD_CAN_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(EMVD_CAN_PORT, &GPIO_InitStructure);
   
	/* Configure CAN1 TX pin */
	GPIO_InitStructure.GPIO_Pin =  EMVD_CAN_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(EMVD_CAN_PORT, &GPIO_InitStructure);

// 	/* Remap CAN1 and CAN2 GPIOs */
	GPIO_PinRemapConfig(EMVD_REMAP1_CAN, DISABLE);
	GPIO_PinRemapConfig(EMVD_REMAP2_CAN, DISABLE);
											

	//使能CAN1功能时钟
	RCC_APB1PeriphClockCmd(EMVD_CAN_RCC , ENABLE);  				 
  
	SetCANSpeed(EMVD_CAN, 200);		//	设置CAN通信速率为200K		

	/* CAN1,CAN2 filter init */
	CAN_FilterInitStructure.CAN_FilterNumber = 2;							//过滤器号		  //接收所有数据，不滤波，id=0 mask=0xffff
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = CAN1_FILTER_ID_HIGH;	
	CAN_FilterInitStructure.CAN_FilterIdLow = CAN1_FILTER_ID_LOW;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = CAN1_FILTER_MASK_ID_HIGH;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = CAN1_FILTER_MASK_ID_LOW;

	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;					//必须至少使能一个;
	CAN_FilterInit(&CAN_FilterInitStructure);
  
	NVIC_InitStructure.NVIC_IRQChannel = EMVD_CAN_RX_IRQN;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;				//不使用中断优先级嵌套。因为SysTick的中断优先级为0x0f
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;						//0级用于SysTick
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EMVD_CAN_TX_IRQN;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;				//不使用中断优先级嵌套。因为SysTick的中断优先级为0x0f
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;						//0级用于SysTick
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/*CAN1（对应该板的CAN1口）通信中断使能*/
	CAN_ITConfig(EMVD_CAN, CAN_IT_TME, DISABLE);
	CAN_ITConfig(EMVD_CAN, CAN_IT_FMP0, ENABLE);
	CanTrsDummy(EMVD_CAN);
}

/********************************************************************************************
** 函数名	：	 	CanTrsDummy
** 输　入	：  	CAN_TypeDef* CANx，x为1或2，分别代表CAN1或者CAN2
** 输　出	： 		无
** 功能描述	：		发送一个dummy帧，以规避0#邮箱发中断不再产生的可能。
** 作　者	：	 	沈万江
** 日　期	：  	2014.10.22
** 版本		：    	V1.0A
** 更新记录	：
** 					日    期      姓    名                    描      述
** 					==========  =============  ========================================
**
*********************************************************************************************/
static void CanTrsDummy(CAN_TypeDef* CANx)
{
	CANx->sTxMailBox[0].TIR = ((u32)1 << 31);						//标准格式、数据帧
    
   	CANx->sTxMailBox[0].TDTR &= (u32)0xFFFFFFF0;					//数据长度为0
   	CANx->sTxMailBox[0].TDLR = 0;
   	CANx->sTxMailBox[0].TDHR = 0;
   	CANx->sTxMailBox[0].TIR |= ((u32)0x00000001);					//请求发送
}

/********************************************************************************************
** 函数名	：		CanVarInit
** 输　入	：		无
** 输　出	：		无
** 功能描述	：		
** 作　者	：		沈万江
** 日　期	：		2014.10.22
** 版本		：		V1.0A
** 更新记录	：
** 					日    期      姓    名                    描      述
** 					==========  =============  ========================================
**
*********************************************************************************************/
void CanVarInit(void)
{
	
	s_u16EmvdCanRxQueueHeadPtr = 0;		//结束位置
	s_u16EmvdCanRxQueueHeadPtr = 0;		//起始位置
	s_u16EmvdCanRxQueueCnt = 0;		   //CAN接收数据计数器
	
	s_u16EmvdCanTxQueueHeadPtr = 0;	     //起始位置
	s_u16EmvdCanTxQueueEndPtr = 0;		//结束位置
	s_u16EmvdCanTxQueueCnt = 0;		//CAN发送数据计数器
	CANRxTimer = TIMER_CLOSED;		//关闭CAN接收帧计时器
}	

/********************************************************************************************
** 函数名	：	 	USB_HP_CAN1_RX0_IRQHandler
** 输　入	：  	无
** 输　出	： 		无
** 功能描述	：		CAN接收中断服务子程序。
** 作　者	：	 	沈万江
** 日　期	：  	2014.10.22
** 版本		：    	V1.0A
** 更新记录	：
** 					日    期      姓    名                    描      述
** 					==========  =============  ========================================
**
*********************************************************************************************/
void USB_LP_CAN1_RX0_IRQHandler(void) 
{
    u8 			u8Tmp,u8i,u8j;
	u16 		u16FMP0;
	CanRxMsg 	RxMessage;
	
	EMVD_CAN->RF0R = (CAN_RF0R_FULL0|CAN_RF0R_FOVR0);			//清除接收中断的满和溢出
	
	u16FMP0 = EMVD_CAN->RF0R & CAN_RF0R_FMP0;					//接收数据FIFO0计数
	
	/*
	 * 判断Rx-0#邮箱有报文
	 */
	if(u16FMP0)
	{
		for(u8i = 0; u8i < u16FMP0; u8i++)
		{
			CAN_Receive(EMVD_CAN, CAN_FIFO0, &RxMessage);			
			if((s_u16EmvdCanRxQueueCnt < EMVD_RXCAN_SIZE) && (RxMessage.IDE == CAN_ID_EXT))
			{				
				s_EmvdCanRxQueue[s_u16EmvdCanRxQueueEndPtr].u32ID.u32Id = RxMessage.ExtId;
				s_EmvdCanRxQueue[s_u16EmvdCanRxQueueEndPtr].u16DLC = RxMessage.DLC;
				u8Tmp = RxMessage.DLC;
				for(u8j = 0; u8j < u8Tmp; u8j++)
					s_EmvdCanRxQueue[s_u16EmvdCanRxQueueEndPtr].u8Data[u8j] = RxMessage.Data[u8j];
				s_u16EmvdCanRxQueueEndPtr++;
				s_u16EmvdCanRxQueueEndPtr %= EMVD_RXCAN_SIZE;
				s_u16EmvdCanRxQueueCnt++;
				CANRxTimer = CAN_RX_TIMER_VAL;  //是否有接收数据
			}
		}		
		/*
		 *指示灯状态
		 */
 		g_u8LightState[0] = 1;  //CAN通讯指示灯亮
		
	 }	
}

/********************************************************************************************
** 函数名	：	 	USB_HP_CAN1_TX_IRQHandler
** 输　入	：  	无
** 输　出	： 		无
** 功能描述	：		CAN中断发送服务子程序。
** 作　者	：	 	沈万江
** 日　期	：  	2014.10.22
** 版本		：    	V1.0A
** 更新记录	：
** 					日    期      姓    名                    描      述
** 					==========  =============  ========================================
**
*********************************************************************************************/
void USB_HP_CAN1_TX_IRQHandler(void)
{
	/*
	 * 判断0#邮箱是否为空
	 */
	while((EMVD_CAN->TSR & CAN_TSR_TME0) != CAN_TSR_TME0)		//0#发送邮箱不可用
	{
		EMVD_CAN->TSR |= CAN_TSR_ABRQ0;							//中止0#发送邮箱
	}
	/*
	 * 0#邮箱不为空，发送数据
	 */
	while(s_u16EmvdCanTxQueueCnt)
	{
		
		CAN_Transmit(EMVD_CAN, &s_EmvdCanTxQueue[s_u16EmvdCanTxQueueHeadPtr]);
		s_u16EmvdCanTxQueueHeadPtr++;
		s_u16EmvdCanTxQueueHeadPtr %= EMVD_TXCAN_SIZE;
	
		/*
		 * 发送缓存中的数据个数减1；如果队列中没有数据，禁止发送中断
		 */
		s_u16EmvdCanTxQueueCnt --;
		if(!s_u16EmvdCanTxQueueCnt)
			CAN_ITConfig(EMVD_CAN, CAN_IT_TME, DISABLE);		//关闭CAN1发送邮箱空中断使能
		/*
		 * 指示灯状态
		 */
		g_u8LightState[1] = 1; //CAN通讯指示灯亮
		
	}
}

