
/******************************************************************************************
* �ļ�����	  	Can_Driver.c
* ���ߣ�    		����
* �汾��   		V1.0
* ���ڣ�      	2014.10.22
* ��������:   	Ӳ����ʼ����CANģ���շ��ж�
* Ӳ����Ϣ��		
* ʹ��ע�⣺	
* �޸�˵����   
*
*       >>>>  �ڹ����е�λ��  <<<<
*             3-Ӧ�ò�
*             2-Э���
*          ��   1-Ӳ��������
*********************************************************************************************
* @copy
* <h2><center>&copy; COPYRIGHT ������������޹�˾ �з����� �����</center></h2>
*********************************************************************************************/
/****************************************ͷ�ļ�����******************************************/
#include "includes.h"
#include "Can_Driver.h"

/****************************************��������******************************************/
sCanComProtocolFrm  s_EmvdCanRxQueue[EMVD_RXCAN_SIZE]; 
CanTxMsg   		    s_EmvdCanTxQueue[EMVD_TXCAN_SIZE];

extern u16  CANRxTimer;
/***�洢CAN��������***/
u16  s_u16EmvdCanRxQueueHeadPtr;
u16  s_u16EmvdCanRxQueueEndPtr;
u16  s_u16EmvdCanRxQueueCnt;

/***�洢CAN��������***/
u16  s_u16EmvdCanTxQueueHeadPtr;
u16  s_u16EmvdCanTxQueueEndPtr;
u16  s_u16EmvdCanTxQueueCnt;

u8 g_u8LightState[LIGHT_NUM];

/********************************************************************************************
** ������	��	 	SetCANSpeed
** �䡡��	��  	
** �䡡��	�� 		��
** ��������	��	����CANͨ������
** ������	��	 	����
** �ա���	��  	2014.10.22
** �汾		��    	V1.0A
** ���¼�¼	��
** 					��    ��      ��    ��                    ��      ��
** 					==========  =============  ========================================
**
*********************************************************************************************/
void SetCANSpeed(CAN_TypeDef *CANx, u16 u16CANxSpeed)
{
	CAN_InitTypeDef CAN_InitStructure;
    RCC_ClocksTypeDef  rcc_clocks; 
	/*
	 * ��λCANx�ṹ��ʼ��
	 */
	CAN_DeInit(CANx);
	CAN_StructInit(&CAN_InitStructure);

	/*
	 * CANx�ṹ��ʼ������
	 */
	CAN_InitStructure.CAN_TTCM = DISABLE;			//��ֹʱ�䴥��
	CAN_InitStructure.CAN_ABOM = ENABLE;			//�����Զ��ָ�
	CAN_InitStructure.CAN_AWUM = ENABLE;			//�Զ�����
	CAN_InitStructure.CAN_NART = ENABLE;			//��ֹ�ش�
	CAN_InitStructure.CAN_RFLM = DISABLE;			//�������������
	CAN_InitStructure.CAN_TXFP = ENABLE;			//���ȼ��ɷ��������˳��������
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;	//;CAN_Mode_LoopBack	
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;  
	CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
	
    RCC_GetClocksFreq(&rcc_clocks);					//���ñ�׼�⺯������ȡϵͳʱ�ӡ�
	/*
	 * ���ò�����
	 */
	CAN_InitStructure.CAN_Prescaler = rcc_clocks.PCLK1_Frequency / 1000 / u16CANxSpeed / (3+CAN_InitStructure.CAN_BS1+CAN_InitStructure.CAN_BS2);
	CAN_Init(CANx, &CAN_InitStructure);				//��λCANx 
}

/********************************************************************************************
** ������	��	 	CanConfig
** �䡡��	��	
** �䡡��	��		��
** ��������	��		CAN����
** ������	��		����
** �ա���	��		2014.10.22
** �汾		��		V1.0A
** ���¼�¼	��
** 					��    ��      ��    ��                    ��      ��
** 					==========  =============  ========================================
**
*********************************************************************************************/

void CanConfig(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
 
	RCC_APB2PeriphClockCmd(EMVD_CAN_GPIO_RCC | RCC_APB2Periph_AFIO, ENABLE);	  //ʹ��PA�ڣ�PB�ڣ����ù���ʱ��
     
	/*
	 * CAN�˿�����
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
											

	//ʹ��CAN1����ʱ��
	RCC_APB1PeriphClockCmd(EMVD_CAN_RCC , ENABLE);  				 
  
	SetCANSpeed(EMVD_CAN, 200);		//	����CANͨ������Ϊ200K		

	/* CAN1,CAN2 filter init */
	CAN_FilterInitStructure.CAN_FilterNumber = 2;							//��������		  //�����������ݣ����˲���id=0 mask=0xffff
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = CAN1_FILTER_ID_HIGH;	
	CAN_FilterInitStructure.CAN_FilterIdLow = CAN1_FILTER_ID_LOW;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = CAN1_FILTER_MASK_ID_HIGH;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = CAN1_FILTER_MASK_ID_LOW;

	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;					//��������ʹ��һ��;
	CAN_FilterInit(&CAN_FilterInitStructure);
  
	NVIC_InitStructure.NVIC_IRQChannel = EMVD_CAN_RX_IRQN;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;				//��ʹ���ж����ȼ�Ƕ�ס���ΪSysTick���ж����ȼ�Ϊ0x0f
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;						//0������SysTick
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EMVD_CAN_TX_IRQN;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;				//��ʹ���ж����ȼ�Ƕ�ס���ΪSysTick���ж����ȼ�Ϊ0x0f
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;						//0������SysTick
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/*CAN1����Ӧ�ð��CAN1�ڣ�ͨ���ж�ʹ��*/
	CAN_ITConfig(EMVD_CAN, CAN_IT_TME, DISABLE);
	CAN_ITConfig(EMVD_CAN, CAN_IT_FMP0, ENABLE);
	CanTrsDummy(EMVD_CAN);
}

/********************************************************************************************
** ������	��	 	CanTrsDummy
** �䡡��	��  	CAN_TypeDef* CANx��xΪ1��2���ֱ����CAN1����CAN2
** �䡡��	�� 		��
** ��������	��		����һ��dummy֡���Թ��0#���䷢�жϲ��ٲ����Ŀ��ܡ�
** ������	��	 	����
** �ա���	��  	2014.10.22
** �汾		��    	V1.0A
** ���¼�¼	��
** 					��    ��      ��    ��                    ��      ��
** 					==========  =============  ========================================
**
*********************************************************************************************/
static void CanTrsDummy(CAN_TypeDef* CANx)
{
	CANx->sTxMailBox[0].TIR = ((u32)1 << 31);						//��׼��ʽ������֡
    
   	CANx->sTxMailBox[0].TDTR &= (u32)0xFFFFFFF0;					//���ݳ���Ϊ0
   	CANx->sTxMailBox[0].TDLR = 0;
   	CANx->sTxMailBox[0].TDHR = 0;
   	CANx->sTxMailBox[0].TIR |= ((u32)0x00000001);					//������
}

/********************************************************************************************
** ������	��		CanVarInit
** �䡡��	��		��
** �䡡��	��		��
** ��������	��		
** ������	��		����
** �ա���	��		2014.10.22
** �汾		��		V1.0A
** ���¼�¼	��
** 					��    ��      ��    ��                    ��      ��
** 					==========  =============  ========================================
**
*********************************************************************************************/
void CanVarInit(void)
{
	
	s_u16EmvdCanRxQueueHeadPtr = 0;		//����λ��
	s_u16EmvdCanRxQueueHeadPtr = 0;		//��ʼλ��
	s_u16EmvdCanRxQueueCnt = 0;		   //CAN�������ݼ�����
	
	s_u16EmvdCanTxQueueHeadPtr = 0;	     //��ʼλ��
	s_u16EmvdCanTxQueueEndPtr = 0;		//����λ��
	s_u16EmvdCanTxQueueCnt = 0;		//CAN�������ݼ�����
	CANRxTimer = TIMER_CLOSED;		//�ر�CAN����֡��ʱ��
}	

/********************************************************************************************
** ������	��	 	USB_HP_CAN1_RX0_IRQHandler
** �䡡��	��  	��
** �䡡��	�� 		��
** ��������	��		CAN�����жϷ����ӳ���
** ������	��	 	����
** �ա���	��  	2014.10.22
** �汾		��    	V1.0A
** ���¼�¼	��
** 					��    ��      ��    ��                    ��      ��
** 					==========  =============  ========================================
**
*********************************************************************************************/
void USB_LP_CAN1_RX0_IRQHandler(void) 
{
    u8 			u8Tmp,u8i,u8j;
	u16 		u16FMP0;
	CanRxMsg 	RxMessage;
	
	EMVD_CAN->RF0R = (CAN_RF0R_FULL0|CAN_RF0R_FOVR0);			//��������жϵ��������
	
	u16FMP0 = EMVD_CAN->RF0R & CAN_RF0R_FMP0;					//��������FIFO0����
	
	/*
	 * �ж�Rx-0#�����б���
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
				CANRxTimer = CAN_RX_TIMER_VAL;  //�Ƿ��н�������
			}
		}		
		/*
		 *ָʾ��״̬
		 */
 		g_u8LightState[0] = 1;  //CANͨѶָʾ����
		
	 }	
}

/********************************************************************************************
** ������	��	 	USB_HP_CAN1_TX_IRQHandler
** �䡡��	��  	��
** �䡡��	�� 		��
** ��������	��		CAN�жϷ��ͷ����ӳ���
** ������	��	 	����
** �ա���	��  	2014.10.22
** �汾		��    	V1.0A
** ���¼�¼	��
** 					��    ��      ��    ��                    ��      ��
** 					==========  =============  ========================================
**
*********************************************************************************************/
void USB_HP_CAN1_TX_IRQHandler(void)
{
	/*
	 * �ж�0#�����Ƿ�Ϊ��
	 */
	while((EMVD_CAN->TSR & CAN_TSR_TME0) != CAN_TSR_TME0)		//0#�������䲻����
	{
		EMVD_CAN->TSR |= CAN_TSR_ABRQ0;							//��ֹ0#��������
	}
	/*
	 * 0#���䲻Ϊ�գ���������
	 */
	while(s_u16EmvdCanTxQueueCnt)
	{
		
		CAN_Transmit(EMVD_CAN, &s_EmvdCanTxQueue[s_u16EmvdCanTxQueueHeadPtr]);
		s_u16EmvdCanTxQueueHeadPtr++;
		s_u16EmvdCanTxQueueHeadPtr %= EMVD_TXCAN_SIZE;
	
		/*
		 * ���ͻ����е����ݸ�����1�����������û�����ݣ���ֹ�����ж�
		 */
		s_u16EmvdCanTxQueueCnt --;
		if(!s_u16EmvdCanTxQueueCnt)
			CAN_ITConfig(EMVD_CAN, CAN_IT_TME, DISABLE);		//�ر�CAN1����������ж�ʹ��
		/*
		 * ָʾ��״̬
		 */
		g_u8LightState[1] = 1; //CANͨѶָʾ����
		
	}
}

