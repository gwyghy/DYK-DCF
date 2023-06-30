/******************************************************************************************
* �ļ�����		Can_Pro.c
* ���ߣ�    		����
* �汾��   		V1.0
* ���ڣ�      	2014.10.22
* ��������:  	�����ŷ���������֧�ܿ�����֮���CAN����ͨ�š���ģ������ײ�������Ӧ�ò����
* Ӳ����Ϣ��		
* ʹ��ע�⣺	
* �޸�˵����   
*
*       >>>>  �ڹ����е�λ��  <<<<
*          ��   3-Ӧ�ò�
*          ��   2-Э���
*          ��   1-Ӳ��������
*********************************************************************************************
* @copy
* <h2><center>&copy; COPYRIGHT ������������޹�˾ �з����� �����</center></h2>
*********************************************************************************************/
/****************************************ͷ�ļ�����******************************************/
#include "Can_Pro.h"
#include "includes.h"
/****************************************��������******************************************/
static u8	s_u8CRC_TABLE[]= {0x00,0x31,0x62,0x53,0xC4,0xF5,0xA6,0x97,0xB9,0x88,0xDB,0xEA,0x7D,0x4C,0x1F,0x2E};	 //CRCУ���

//32�ӿڰ��Ӧ26�Ľӿ�˳�� 
static const int EMV_26_Out_Table[32] ={22,21,20,19,18,17,16,15,14,13,27,28,26,25,24,23, 7, 8, 5, 6, 3, 4, 1, 2,11,12,10, 9,29,30,31,32};
      
//32����33996�������˳��     
static const int EMV_32_Out_Table[32] ={24,19,26,21,25,23,29,30,11,12,16,15,18,20,22,17, 2,32, 1,31, 6,28, 4,27,10, 5, 8, 3, 9,14, 7,13};
/****************************************������������******************************************/
u32 ActionRxQueue[ACTION_RXQUEUEMAX];//���ն���֡���ζ���
u16 ActionRxQueueHead;//��ʼλ��
u16 ActionRxQueueEnd;//����λ��
	
u32 ActionQueue[ACTION_QUEUEMAX]; //����ִ�л��ζ���
u16 ActionQueueHead;//��ʼλ��
u16 ActionQueueEnd;//����λ��

u32 ActionWord = 0x00000000; //Һѹ������Чֵ
u32 LastAction = 0x00000000; //��¼�ϴεĶ���������
static u32 u32newAct;
static u32 u32Act;
extern u16 ActionTimer;
/*****************************************�������ر�������*********************************************/
u32 CheckData = 0x00000000;   //���ĵ�ŷ������
u32 CheckWord = 0x00000000;   //��¼����⶯����Чֵ
u32 CheckNewData = 0x00000000; 	//�ӽ��շ����������»�ȡ�������
	
static u32 	FaultWord 		= 0x00000000;		 //SPI����ֵ����MC33996����������
static u32 	FaultWord1 		= 0x00000000;		//
static u32 	FaultWord2 		= 0x00000000;	   //

static u8   CommandByte1	= 0;
static u8	CommandByte2	= 0;
static u8	CommandByte3	= 0;
static u8	CommandByte4	= 0;

u32 VoltValue = 0x00000000;	//ADC1�˲����������
	
u32 CheckQueue[CHECK_QUEUEMAX];//���շ���⶯����������
u16 CheckQueueHead;//��ʼλ��
u16 CheckQueueEnd;//����λ��	

u32 CheckActionQueue[CHECK_ACTION_QUEUEMAX]; //�������������
u16 CheckActionQueueHead;//��ʼλ��
u16 CheckActionQueueEnd;//����λ��
		
extern sCanComProtocolFrm  s_EmvdCanRxQueue[EMVD_RXCAN_SIZE]; 
extern CanTxMsg   		   s_EmvdCanTxQueue[EMVD_TXCAN_SIZE];

/***�洢CAN��������***/
extern u16  s_u16EmvdCanRxQueueHeadPtr;
extern u16  s_u16EmvdCanRxQueueEndPtr;
extern u16  s_u16EmvdCanRxQueueCnt;

/***�洢CAN��������***/
extern u16  s_u16EmvdCanTxQueueHeadPtr;
extern u16  s_u16EmvdCanTxQueueEndPtr;
extern u16  s_u16EmvdCanTxQueueCnt;

/***Ӳ��Id***/
extern u32 Hare_Id;

/**********************************���嶯������************************************************/
const u16 OutPoint_Action_GpioPinBuf[]=
{
	
	OUTPOINT_24_GPIO_PIN,OUTPOINT_19_GPIO_PIN,OUTPOINT_26_GPIO_PIN,OUTPOINT_21_GPIO_PIN,
	OUTPOINT_25_GPIO_PIN,OUTPOINT_23_GPIO_PIN,OUTPOINT_29_GPIO_PIN,OUTPOINT_30_GPIO_PIN,
	OUTPOINT_11_GPIO_PIN,OUTPOINT_12_GPIO_PIN,OUTPOINT_16_GPIO_PIN,OUTPOINT_15_GPIO_PIN,
	OUTPOINT_18_GPIO_PIN,OUTPOINT_20_GPIO_PIN,OUTPOINT_22_GPIO_PIN,OUTPOINT_17_GPIO_PIN,
	
	OUTPOINT_2_GPIO_PIN,OUTPOINT_32_GPIO_PIN,OUTPOINT_1_GPIO_PIN,OUTPOINT_31_GPIO_PIN,
	OUTPOINT_6_GPIO_PIN,OUTPOINT_28_GPIO_PIN,OUTPOINT_4_GPIO_PIN,OUTPOINT_27_GPIO_PIN,
	OUTPOINT_10_GPIO_PIN,OUTPOINT_5_GPIO_PIN,OUTPOINT_8_GPIO_PIN,OUTPOINT_3_GPIO_PIN,
	OUTPOINT_9_GPIO_PIN,OUTPOINT_14_GPIO_PIN,OUTPOINT_7_GPIO_PIN,OUTPOINT_13_GPIO_PIN,

};
GPIO_TypeDef * const OutPoint_Action_GpioPortBuf[]=
{
	
	OUTPOINT_24_GPIO_PORT,OUTPOINT_19_GPIO_PORT,OUTPOINT_26_GPIO_PORT,OUTPOINT_21_GPIO_PORT,
	OUTPOINT_25_GPIO_PORT,OUTPOINT_23_GPIO_PORT,OUTPOINT_29_GPIO_PORT,OUTPOINT_30_GPIO_PORT,
	OUTPOINT_11_GPIO_PORT,OUTPOINT_12_GPIO_PORT,OUTPOINT_16_GPIO_PORT,OUTPOINT_15_GPIO_PORT,
	OUTPOINT_18_GPIO_PORT,OUTPOINT_20_GPIO_PORT,OUTPOINT_22_GPIO_PORT,OUTPOINT_17_GPIO_PORT,
	
	OUTPOINT_2_GPIO_PORT,OUTPOINT_32_GPIO_PORT,OUTPOINT_1_GPIO_PORT,OUTPOINT_31_GPIO_PORT,
	OUTPOINT_6_GPIO_PORT,OUTPOINT_28_GPIO_PORT,OUTPOINT_4_GPIO_PORT,OUTPOINT_27_GPIO_PORT,
	OUTPOINT_10_GPIO_PORT,OUTPOINT_5_GPIO_PORT,OUTPOINT_8_GPIO_PORT,OUTPOINT_3_GPIO_PORT,
	OUTPOINT_9_GPIO_PORT,OUTPOINT_14_GPIO_PORT,OUTPOINT_7_GPIO_PORT,OUTPOINT_13_GPIO_PORT,	
};

/********************************************************************************************
** ������	��	 	s_Can1RxQueueProc
** �䡡��	��  	
** �䡡��	�� 		��
** ��������	��		��CAN���ն����е����ݽ��д���
** ������	��	 	����
** �ա���	��  	2014.10.22s
** �汾		��    	V1.0A
** ���¼�¼	��
** 					��    ��      ��    ��                    ��      ��
** 					==========  =============  ========================================
**
*********************************************************************************************/
void s_Can1RxQueueProc(void)
{
	sCanComProtocolFrm *sRcvCan;
	u16 i;
	i = s_u16EmvdCanRxQueueCnt;	
	sRcvCan = &s_EmvdCanRxQueue[s_u16EmvdCanRxQueueHeadPtr];
	/*
	 * �жϽ��ն����Ƿ������ݣ�����������д���
	*/
	while(i)
	{
		/*
		 * ����֡ID������Ӧ�Ĵ�����
		 */
		if(Hare_Id == DEVICE_MOS)
			MosValue_PowerOn; //�յ�����ʱ�����鹩�翪��
		//add���ճ������֡
		if((s_EmvdCanRxQueue[s_u16EmvdCanRxQueueHeadPtr].u32ID.ID.RD))
		{
			/*
			 * �յ�����ĳ���֡ʱ�Ĵ���
			 */
			CanRecvProgProc((CXB_CAN_FRAME_TYPE *)sRcvCan);
		}
		else
		{
			switch(s_EmvdCanRxQueue[s_u16EmvdCanRxQueueHeadPtr].u32ID.ID.FT)
			{
				case ACTION_CMD:
					CanCmdDataProc(ACTION_CMD);				//�����������
				break;
				
				case CHECK_CMD:
					CanCmdDataProc(CHECK_CMD);				//����������
				break;
				default:
				break;
			}
		}
		/*
		 * ����������֡����-1����ָ��+1����ʱӦ�ùر��жϣ���ֹ�ı���ʱ���ж�Ҳ�޸ĸñ���
		 */
		__disable_irq();
		s_u16EmvdCanRxQueueCnt--;
		s_u16EmvdCanRxQueueHeadPtr++;
		s_u16EmvdCanRxQueueHeadPtr %= EMVD_RXCAN_SIZE;
		i = s_u16EmvdCanRxQueueCnt;
		__enable_irq();
		
	}
}
/********************************************************************************************
** ������	��	 	CanCmdDataProc
** �䡡��	��  	
** �䡡��	�� 		��
** ��������	��		CAN�������ݽ��д���
** ������	��	 	����
** �ա���	��  	2014.10.22
** �汾		��    	V1.0A
** ���¼�¼	��
** 					��    ��      ��    ��                    ��      ��
** 					==========  =============  ========================================
**
*********************************************************************************************/
#include "string.h"
void CanCmdDataProc(u32 u32Cmd)
{
	u8 		u8TmpDLC,u8i,u8SaveCrc;
	u32		u32Data = 0;
	//sCanComProtocolFrm 	RxCan;
	u8		Tmp[9];
	/*
	 * ȡ��CAN����֡�е����ݣ������Ч���ݣ����ݶ������ͣ�������صĶ���
	 */
	if(s_u16EmvdCanRxQueueCnt)
	{
		u8TmpDLC = s_EmvdCanRxQueue[s_u16EmvdCanRxQueueHeadPtr].u16DLC;
		u8SaveCrc = s_EmvdCanRxQueue[s_u16EmvdCanRxQueueHeadPtr].u8Data[u8TmpDLC - 1];//����������ݵ�У����
				
		Tmp[0] = s_EmvdCanRxQueue[s_u16EmvdCanRxQueueHeadPtr].u32ID.u32Id & 0xff;
		Tmp[1] = (s_EmvdCanRxQueue[s_u16EmvdCanRxQueueHeadPtr].u32ID.u32Id>>8) & 0xff;
		Tmp[2] = (s_EmvdCanRxQueue[s_u16EmvdCanRxQueueHeadPtr].u32ID.u32Id>>16) & 0xff;
		Tmp[3] = (s_EmvdCanRxQueue[s_u16EmvdCanRxQueueHeadPtr].u32ID.u32Id>>24) & 0xff;
		Tmp[4] = s_EmvdCanRxQueue[s_u16EmvdCanRxQueueHeadPtr].u16DLC;
		Tmp[5] = s_EmvdCanRxQueue[s_u16EmvdCanRxQueueHeadPtr].u8Data[0];
		Tmp[6] = s_EmvdCanRxQueue[s_u16EmvdCanRxQueueHeadPtr].u8Data[1];
		Tmp[7] = s_EmvdCanRxQueue[s_u16EmvdCanRxQueueHeadPtr].u8Data[2];
		Tmp[8] = s_EmvdCanRxQueue[s_u16EmvdCanRxQueueHeadPtr].u8Data[3];
		
		if(u8SaveCrc != CRC_8(Tmp, 9))
			return;		
		for (u8i = 0; u8i < u8TmpDLC; u8i++)
			u32Data |= (s_EmvdCanRxQueue[s_u16EmvdCanRxQueueHeadPtr].u8Data[u8i] << (u8i << 3));
		if(u32Cmd == ACTION_CMD)
			InsertActionRxQueue(trans_26To32(u32Data));					//�������ݼ������
		else if(u32Cmd == CHECK_CMD)
        {
            CheckData = s_EmvdCanRxQueue[s_u16EmvdCanRxQueueHeadPtr].u8Data[0];
            CheckData = CheckData|s_EmvdCanRxQueue[s_u16EmvdCanRxQueueHeadPtr].u8Data[1] << 8;
            CheckData = CheckData| s_EmvdCanRxQueue[s_u16EmvdCanRxQueueHeadPtr].u8Data[2] << 16;
            CheckData = CheckData|s_EmvdCanRxQueue[s_u16EmvdCanRxQueueHeadPtr].u8Data[3] << 24;                                             
			  InsertCheckQueue(trans_26To32(u32Data));						//�������������
        }
	}
}
 /*********************************************************************************************************************
  **function name:	   InsertActionRxQueue()
  **description:	   �����������Ķ������ݷ��붯�����ն���
  **input value:	   ��
  **output value:��	�� ��
  **return value:	�� ��
**********************************************************************************************************************/
void InsertActionRxQueue(u32 Action)
{
	ActionRxQueue[ActionRxQueueEnd++] =  Action;
	ActionRxQueueEnd %=  ACTION_RXQUEUEMAX;
}

u16 NewActionRxQueue(void)
{
	if(ActionQueueHead != ActionQueueEnd)
		return 0;
	return (ActionRxQueueHead != ActionRxQueueEnd);
}

void InsertActionQueue(u32 Action)
{
	u32 j;
	u16	i;

	if (!Action)
	{
		ActionQueue[ActionQueueEnd++] = 0;
		ActionQueueEnd %= ACTION_QUEUEMAX;
		return;
	}
	for (i = 0, j = 1; i < 32; i++)
	{
		if (Action & j)
		{
			ActionQueue[ActionQueueEnd++] = j;			//���϶�����ִ��붯��������
			ActionQueueEnd %= ACTION_QUEUEMAX;
		}
		j <<= 1;
	}
}
/*********************************************************************************************************************
  **function name:	   s_CanRecvDataPro()
  **description:	   CAN�������ݴ���
  **input value:	   ��
  **output value:��	�� ��
  **return value:	�� ��
**********************************************************************************************************************/
void s_CanRecvDataPro(void)
{
	if(Hare_Id == DEVICE_MOS)
	{
	   if(Mos_CheckCycle(MOS_TIME) || OverCurrent_Checkcycle(OCP_TIME))
	   {
		   return;
	   }
		/*
		*CAN���յĶ������ݴ���
		*/
		s_ActionDataPro();
		/*
		*ִ�ж��������еĶ�������
		*/
		s_ActionCmdExec(Hare_Id);
		
		/*
		*CAN���յķ�������ݴ���
		*/
		s_CheckDataProc();
		
		/*
		*ִ�з�������������
		*/
		s_CheckCmdExec(Hare_Id);
	}
	else
	{
		/*
		*CAN���յĶ������ݴ���
		*/
		s_ActionDataPro();
		/*
		*ִ�ж��������еĶ�������
		*/
		s_ActionCmdExec(Hare_Id);
		
		/*
		*CAN���յķ�������ݴ���
		*/
		s_CheckDataProc();
		
		/*
		*ִ�з�������������
		*/
		s_CheckCmdExec(Hare_Id);
	}
}
void s_ActionDataPro(void)
{
	/*
	*�ж϶����������Ƿ�������
	*/
	if(NewActionRxQueue())
	{
		u32newAct = ActionRxQueue[ActionRxQueueHead++];
		ActionRxQueueHead %= ACTION_RXQUEUEMAX;
		/*
		*�½��յ����ݴ������붯���������
		*/
		u32Act = ActionWord;
		ActionWord &= u32newAct;
		 
		if((ActionWord & LastAction) ==0)
			ActionTimer = TIMER_EXPIRED;
	  
		 u32newAct |= u32Act;
		 u32newAct ^= u32Act; //ȡ���¶���
		 if(u32newAct || (u32Act != ActionWord))
			 InsertActionQueue(u32newAct);
	}	
}

void s_ActionCmdExec(u32 u32Id)
{
	/*
	*�ж϶�����ʱ��ʱ�䵽��񣬼��ж���һ����ŷ������ȶ��񣬼�������ŷ��ȵ�������Ҫ��һ��ʱ����TIMOUT
	*/
	if(ActionTimer == TIMER_EXPIRED)
	{
		while(ActionQueueHead != ActionQueueEnd)
		{
			u32Act = ActionQueue[ActionQueueHead++];
			ActionQueueHead %= ACTION_QUEUEMAX;			
			ActionWord |= u32Act;
			LastAction = u32Act;
			if(u32Id == DEVICE_MOS)
			{
				s_MosSet_Action(ActionWord);
				/*
				 * �����������ʱ��
				 */
				if (u32Act)
				{
					ActionTimer = CHIPACTION_TIMER_VAL;
					break;
				}						
			}
			else 
			{
				s_ChipSetAction(ActionWord);						
				if(u32Act)
				{
					ActionTimer = MOSACTION_TIMER_VAL;
					break;				
				}	
			}
		}
	}	
}
/*********************************************************************************************************************
  **function name:	   s_ChipSetAction()
  **description:	   оƬ�津������
  **input value:	 ��u32 Action ����ID
  **output value:��	������
  **return value:	������
**********************************************************************************************************************/
void s_ChipSetAction(u32 Action)
{
	/*
	 * ���÷��͸�MC33996оƬ������
	 */
	//CommandByte1 = (u8)(ActionWord & 0x000000ff);				//8��
	//CommandByte2 = (u8)((ActionWord & 0x0000f300) >> 8);		//6��
	//CommandByte3 = (u8)((ActionWord & 0x00ff0000) >> 16);		//8��
	//CommandByte4 = (u8)((ActionWord & 0x0f000000) >> 24);		//4��
	/*
	 * �����Ӧ��������Ϊȫ32���ܣ����͸�MC33996оƬ������
	 */
	CommandByte1 = (u8)(Action & 0x000000ff);				//8��
	CommandByte2 = (u8)((Action & 0x0000ff00) >> 8);		//8��
	CommandByte3 = (u8)((Action & 0x00ff0000) >> 16);		//8��
	CommandByte4 = (u8)((Action & 0xff000000) >> 24);		//8��

	/*
	 * Ƭѡ1����������
	 */
	CS1_DISABLE;
	Delay(1);
	FaultWord1 = TakeAction(0x00,CommandByte2,CommandByte1);
	CS1_ENABLE;				
	/*
	 * Ƭѡ2����������
	 */
	CS2_DISABLE;
	Delay(1);
	FaultWord2 = TakeAction(0x00,CommandByte4,CommandByte3);
	CS2_ENABLE;

	/*
	 * FaultWord��ʾMC33996����������⣬������ϸ��Ϣ���Բμ�MC33996оƬ�ֲ�
	 */
	FaultWord = FaultWord1 + (FaultWord2 << 16);
					/*
	* ���Ͷ��������ӦMC33996����ڵĴ���״̬
	 */
	if (FaultWord)
	{
		s_TransmitCheckOut(FaultWord, 0x11);
		FaultWord = 0;
	}
}

/*********************************************************************************************************************
  **function name:	   TakeAction()
  **description:	   ���Ͷ������л��߼�⶯�������е�����
  **input value:	   u8 CtlWord��MC33996оƬ�п�����, u8 data1, u8 data2
  **output value:
  **return value:	   t,MC33996��������
**********************************************************************************************************************/
u32 TakeAction(u8 CtlWord, u8 data1, u8 data2)
{
	u32 t1,t2,t3,t;

	t1 = SPI1_SendByte(CtlWord);					//��MC33996оƬ�е�24λ��λ�Ĵ�����д���ݿ�����
	t2 = SPI1_SendByte(data1);
	t3 = SPI1_SendByte(data2);
	t = t1<<16 | t2<<8 | t3;
	return t;
}
/*******************************************************************************************
  **function name: SPI1_SendByte()
  **description:   to process the data of Usart2RxBuf[], to verdict the legal frame, and to get
  **               action value.
  **input value:   u8 byte
  **output value:
  **return value:  SPI_I2S_ReceiveData(SPI1)
*******************************************************************************************/
u8 SPI1_SendByte(u8 byte)
{
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

  SPI_I2S_SendData(SPI1, byte);

  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

  return SPI_I2S_ReceiveData(SPI1);
}
/*********************************************************************************************************************
  **function name:	   s_MosSet_Action()
  **description:	   ����MOS�汾IO����
  **input value:	 ��u32 Action ����ID
  **output value:��	������
  **return value:	������
**********************************************************************************************************************/
void s_MosSet_Action(u32 Action)
{
	u32 u32i = 0,u32tmp = 0,u32j = 0;
	for(u32i=0;u32i< 32;u32i++)
	{
		if((Action & (1<<u32i)))
		{		
			GPIO_SetBits(OutPoint_Action_GpioPortBuf[u32i],OutPoint_Action_GpioPinBuf[u32i]);
			continue;
		}
		GPIO_ResetBits(OutPoint_Action_GpioPortBuf[u32i],OutPoint_Action_GpioPinBuf[u32i]);
	}	
}
/*********************************************************************************************************************
  **function name:	   Stop_Action()
  **description:	   ֹͣ��������
  **input value:	 ��u32 u32Id   Ӳ��IDʶ��
  **output value:��	������
  **return value:	������
**********************************************************************************************************************/
void Stop_Action(u32 u32Id)
{
	u32 u32i;
	if(u32Id == DEVICE_MOS)
	{
		for(u32i=0;u32i< 32;u32i++)
		{
			GPIO_ResetBits(OutPoint_Action_GpioPortBuf[u32i],OutPoint_Action_GpioPinBuf[u32i]);
		}
	}
	else
	{
		CS1_DISABLE;
		TakeAction(0x00, 0x00, 0x00);
		CS1_ENABLE;

		CS2_DISABLE;
		TakeAction(0x00, 0x00, 0x00);
		CS2_ENABLE;
	}
	ActionQueueHead = ActionQueueEnd;
	ActionRxQueueHead = ActionRxQueueEnd;
	ActionWord = 0;
	CheckQueueHead = CheckQueueEnd;
	CheckActionQueueHead = CheckActionQueueEnd;
	CheckWord = 0;
	ActionTimer = TIMER_EXPIRED;  //�رն��������ʱ��
}

/*****************************************����Ϊ��������ݴ�������**************************************************************/


/*******************************************************************************************
  **function name: InsertCheckQueue()
  **description:   ����һ���������ƣ�CAN���յ������ݷ�����Ϊ�������ݺͼ�����ݣ���������ݴ�
  **               �뷧������ݻ��ζ��� 
  **input value:   u32 CheckCommand
  **output value:
  **return value:
*******************************************************************************************/
void InsertCheckQueue(u32 CheckCommand)
{
    CheckQueue[CheckQueueEnd++] = CheckCommand;
	CheckQueueEnd %= CHECK_QUEUEMAX;
}

/*******************************************************************************************
  **function name: NewCheckActionRx()
  **description:   �ж�CAN���ջ��λ��������Ƿ������ݣ������ʱ��⶯�������������ݾ�Ҫ��ִ��ʣ��ļ�⶯��
  **               ���û�м�⶯���ˣ��ͷ��ؼ�⶯���������Ƿ������� 
  **input value:   
  **output value:
  **return value:  1����⶯���������Ƿ������ݣ�0����ʾû��
*******************************************************************************************/
u16	NewCheckActionRx(void)
{
	if (CheckActionQueueHead != CheckActionQueueEnd)
	    return(0);	 //��ִ��ʣ����Ч��⶯��
	return (CheckQueueHead != CheckQueueEnd);
}
/*******************************************************************************************
  **function name: InsertCheckActionQueue()
  **description:   �жϴӼ�⶯��������ȡ�����¶������������ִ�м�⶯�������� 
  **input value:   u32 u32Checktmp1
  **output value:
  **return value:  
*******************************************************************************************/
void InsertCheckActionQueue(u32 u32Checktmp1)
{
	CheckActionQueue[CheckActionQueueEnd++] = u32Checktmp1;
	CheckActionQueueEnd %= CHECK_ACTION_QUEUEMAX;
}
/*******************************************************************************************
  **function name: s_CheckDataProc()
  **description:   ����⶯���������ݴ���
  **input value:   ��
  **output value:
  **return value:  
*******************************************************************************************/
void s_CheckDataProc(void)
{
	/*
	*�жϼ��������Ƿ�������
	*/
	if(NewCheckActionRx())
	{
		CheckNewData = CheckQueue[CheckQueueHead++];
		CheckQueueHead %= CHECK_QUEUEMAX;		
		/*
		*�½��յ������Ƿ����ϴ���ͬ������ͬ������������������
		*/
		if(CheckWord != CheckNewData)
		{
			InsertCheckActionQueue(CheckNewData);
		}
		else 
		{
			CheckWord = 0;
		}		
	}	
}
/*******************************************************************************************
  **function name: Mos_CheckQueueSend()
  **description:   Mos�ܷ����ʱʹ�ܼ��ܽ�
  **input value:   ��
  **output value:
  **return value:  
*******************************************************************************************/
void Mos_CheckQueueSend(u32 checkword)
{
	u8 u8i,u8j;
	if(checkword)
	{
		for(u8i=0;u8i< 32;u8i++)
		{
			if((checkword & (1<<u8i)))
			{
				GPIO_SetBits(OutPoint_Action_GpioPortBuf[u8i],OutPoint_Action_GpioPinBuf[u8i]);
			}
		}
	}
	else
	{		
		for(u8j=0;u8j< 32;u8j++)
		{		
			GPIO_ResetBits(OutPoint_Action_GpioPortBuf[u8j],OutPoint_Action_GpioPinBuf[u8j]);
		}
	}
}
/*********************************************************************************************************************
  **function name:	   TakeCheckAction()
  **description:	   ���Ͷ������л��߼�⶯�������е�����
  **input value:	   u8 CtlWord��MC33996оƬ�п�����, 
						u8 data1,16������ڵĸ�8λ
						u8 data2,16������ڵĵ�8λ
  **output value:
  **return value:	   t,MC33996��������
**********************************************************************************************************************/
void TakeCheckAction(u8 CtlWord, u8 data1, u8 data2)
{
	SPI1_SendByte(CtlWord);
	SPI1_SendByte(data1);
	SPI1_SendByte(data2);
}
/*********************************************************************************************************************
  **function name:	   Chip_CheckQueueSend(u32 CheckWord)
  **description:	     �����ʱʹ�ܼ��ܽ�
  **input value:	   
  **output value:
  **return value:      1���գ�0���ǿ�
**********************************************************************************************************************/
void Chip_CheckQueueSend(u32 checkword)  
{
	u8 CheckByte1,CheckByte2,CheckByte3,CheckByte4; 
	//�õ�Ҫ���͵�ָ��ֵ
	// 	CheckByte1 = (u8)(checkword & 0x000000ff);				//8
	// 	CheckByte2 = (u8)((checkword & 0x0000f300) >> 8);		//6
	// 	CheckByte3 = (u8)((checkword & 0x00ff0000) >> 16);		//8
	// 	CheckByte4 = (u8)((checkword & 0x0f000000) >> 24);		//4
	/*
	 * �����Ӧ��������Ϊȫ32���ܣ����͸�MC33996оƬ������
	 */
	CheckByte1 = (u8)(checkword & 0x000000ff);				//8
	CheckByte2 = (u8)((checkword & 0x0000ff00) >> 8);		//8
	CheckByte3 = (u8)((checkword & 0x00ff0000) >> 16);		//8
	CheckByte4 = (u8)((checkword & 0xff000000) >> 24);		//8
	
	//ѡ�е�һ��MC33996оƬ�����͵��ֵ���������
	CS1_DISABLE;
	Delay(1);
	TakeCheckAction(0x00,CheckByte2,CheckByte1);
	CS1_ENABLE;
	//ѡ�еڶ���MC33996оƬ�����͸��ֵ���������
	CS2_DISABLE;
	Delay(1);
	TakeCheckAction(0x00,CheckByte4,CheckByte3);
	CS2_ENABLE;
}
void s_TransmitCheckOut(u32 u32Word,u8 u8Result)
{
	if(EVMDCAN_ENABLED == 1)
		s_CANxCheckValueTxProc(u32Word,u8Result);		//ͨ��CAN����
	VoltValue = 0;	
}
/****************************************************
  **function name:	   s_MosCheckCmdExec()
  **description:	   Mos�淧��⴦����
  **input value:	   ��
  **output value:��	�� ��
  **return value:	�� ��
******************************************************/
void  s_MosCheckCmdExec(void)
{
	 /*
	 * �жϼ�⶯���������Ƿ��м������������ִ��
	 */
	if(CheckActionQueueHead != CheckActionQueueEnd)
	{
		/*
		 * �жϷ���ⲿ�ֵ�Դ��
		 */
		MosValue_PowerOff;
		Delay(2);
		/*
		 * ȡ�¼�⶯������
		 */
		CheckNewData = CheckActionQueue[CheckActionQueueHead++];
		CheckActionQueueHead %= CHECK_ACTION_QUEUEMAX;
		CheckWord |= CheckNewData;
		CheckWord &= CheckNewData;
		
		/*
		 * ������¼�⶯�������ִ��
		 */
		if(CheckWord)
		{
				Mos_CheckQueueSend(CheckWord);					//ѡ��Ҫ���ĵ�ŷ�
				Delay(5);
				Get_Samples_Val();							//���ʮ�β���ֵ
				VoltValue = SampVal_Proc();					//�˲�����
			
			/*
			 * ѡ��CAN����ʹ��CAN-Tx���з��͡�����ѡ���жϷ��͡�
			*/
				/*
				 * ���ݲ���ֵ�жϵ���ȵ����Ƿ����ߣ�0x08����·��0x04����·��0x01������
				 * ��·��ʾ�ȵ�����·����·��ʾ�ȵ���δ�ӣ�������ʾ�ȵ������Ӳ�������ʹ��
				 */
				if(VoltValue < ShortValueMax)
				{
					s_TransmitCheckOut(CheckData,0x08);			//���ͼ����
				}
				if((VoltValue >= FormalValueMin) && (VoltValue <= FormalValueMax))
				{
					s_TransmitCheckOut(CheckData,0x01);			//���ͼ����
				}
				if((VoltValue >= OpenValueMin) && (VoltValue <= OpenValueMax))
				{
					s_TransmitCheckOut(CheckData,0x04);			//���ͼ����
				}
		}
		Mos_CheckQueueSend(0x00000000);
		Delay(2);
		/*
		 * �ָ�������·�ϵĵ�Դ����
		 */
		MosValue_PowerOn;		
	}	
}
/****************************************************
  **function name:	   s_ChipCheckCmdExec()
  **description:	   оƬ�淧��⴦����
  **input value:	   ��
  **output value:��	�� ��
  **return value:	�� ��
******************************************************/
void s_ChipCheckCmdExec(void)
{
	/*
	 * �жϼ�⶯���������Ƿ��м������������ִ��
	 */
	if(CheckActionQueueHead != CheckActionQueueEnd)
	{
		/*
		 * �ж�MC33996оƬ�ĵ�Դ��Ҫ���е�ŷ��������ж�MC33996оƬ�ĵ�Դ
		 */
		ChipVale_PowerOff;
		Delay(2);
		/*
		 * ȡ�¼�⶯������
		 */
		CheckNewData = CheckActionQueue[CheckActionQueueHead++];
		CheckActionQueueHead %= CHECK_ACTION_QUEUEMAX;
		CheckWord |= CheckNewData;
		CheckWord &= CheckNewData;		
		/*
		 * ������¼�⶯�������ִ��
		 */
		if(CheckWord)
		{
			Chip_CheckQueueSend(CheckWord);					//ѡ��Ҫ���ĵ�ŷ�
			Delay(5);
			Get_Samples_Val();							//���ʮ�β���ֵ
			VoltValue = SampVal_Proc();					//�˲�����			
			/*
			 * �����Ľ�����͸�֧�ܿ����������ѡ��ʹ��UART����ʹ��UART-Tx���ͣ�
			 * ���ѡ��CAN����ʹ��CAN-Tx���з��͡�����ѡ���жϷ��͡�
			 */
			// if (u8i < 2)
			// {
			// 		u8i++;
			// }
			// else
			// {
				/*
				 * ���ݲ���ֵ�жϵ���ȵ����Ƿ����ߣ�0x08����·��0x04����·��0x01������
				 * ��·��ʾ�ȵ�����·����·��ʾ�ȵ���δ�ӣ�������ʾ�ȵ������Ӳ�������ʹ��
				 */
				if(VoltValue < ShortValueMax)
				{
					s_TransmitCheckOut(CheckData,0x08);			//���ͼ����
				}
				if((VoltValue >= FormalValueMin) && (VoltValue <= FormalValueMax))
				{
					s_TransmitCheckOut(CheckData,0x01);			//���ͼ����
				}
				if((VoltValue >= OpenValueMin) && (VoltValue <= OpenValueMax))
				{
					s_TransmitCheckOut(CheckData,0x04);			//���ͼ����
				}
// 			}
		}
		Chip_CheckQueueSend(0x00000000);
		Delay(2);
		/*
		 * �ָ�MC33996оƬ�ĵ�Դ
		 */
		ChipVale_PowerOn;
	}
}
/********************************************************************************************
** ������	��	 	s_CheckCmdExec
** �䡡��	��  	��
** �䡡��	�� 		��       
** ��������	��		��������ݴ���ʽ
** ������	��	 	����
** �ա���	��  	2014.10.22
** �汾		��    	V1.0A
** ���¼�¼	��
** 					��    ��      ��    ��                    ��      ��
** 					==========  =============  ========================================
**
*********************************************************************************************/
void s_CheckCmdExec(u32 u32Id)
{
	if(Hare_Id == DEVICE_MOS)
	{
		s_MosCheckCmdExec();
	}
	else 
	{
		s_ChipCheckCmdExec();
	}
}
/*******************************************************************************************
  **function name: Var_Init()
  **description:   ��ʵ���еĶ�ʱ�������ݻ��������г�ʼ���� 
  **input value:
  **output value:
  **return value:
*******************************************************************************************/
void Var_Init(void)
{
	ActionTimer = TIMER_EXPIRED;  //���������ʱ
	ActionRxQueueHead =0;
	ActionRxQueueEnd =0;
	ActionQueueHead =0;
	ActionQueueEnd =0;	
}
/********************************************************************************************
** ������	��	 	CRC_8
** �䡡��	��  	unsigned char *PData,unsigned char Len
** �䡡��	�� 		CRCУ����
** ��������	��		8λCRCУ�麯��
** ������	��	 	����
** �ա���	��  	2014.10.22
** �汾		��    	V1.0A
** ���¼�¼	��
** 					��    ��      ��    ��                    ��      ��
** 					==========  =============  ========================================
**
*********************************************************************************************/
/********************************************************************************************
** ������	��	 	CanTxDataProc
** �䡡��	��  	
** �䡡��	�� 		��
** ��������	��		CAN�������ݽ��д���
** ������	��	 	����
** �ա���	��  	2014.10.22
** �汾		��    	V1.0A
** ���¼�¼	��
** 					��    ��      ��    ��                    ��      ��
** 					==========  =============  ========================================
**
*********************************************************************************************/
void InsCanTxQueue(CanTxMsg *TxCan)
{
	if(s_u16EmvdCanTxQueueCnt <  EMVD_TXCAN_SIZE)
	{
		s_EmvdCanTxQueue[s_u16EmvdCanTxQueueEndPtr] = *TxCan;
				
		s_u16EmvdCanTxQueueEndPtr++;
		s_u16EmvdCanTxQueueEndPtr %= EMVD_TXCAN_SIZE;
		s_u16EmvdCanTxQueueCnt++;
				
		/*
		 * ���ݼ��뷢�Ͷ��к�ʹ�ܷ����ж�
		 */
		CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE);			//CAN1����������ж�ʹ��
	}
}

/********************************************************************************************
** ������	��	 	CanTxDataProc
** �䡡��	��  	
** �䡡��	�� 		��
** ��������	��		CAN�������ݽ��д���
** ������	��	 	����
** �ա���	��  	2014.10.22
** �汾		��    	V1.0A
** ���¼�¼	��
** 					��    ��      ��    ��                    ��      ��
** 					==========  =============  ========================================
**
*********************************************************************************************/     
void CanTxDataProc(u8 *u8DataPtr, u8 u8DLC,u32 u32Cmd)
{
	u8 u8i;
	CanTxMsg TxCan = {0};
	sCanComProtocolFrm TxCanTmp;
	
	TxCanTmp.u32ID.ID.RD = 0;
	TxCanTmp.u32ID.ID.ACK = 0;
	TxCanTmp.u32ID.ID.SUB = 0;
	TxCanTmp.u32ID.ID.SUM = 0;
	TxCanTmp.u32ID.ID.SN = 0;
	TxCanTmp.u32ID.ID.FT = u32Cmd;
	TxCanTmp.u32ID.ID.TID = 0x01;
	TxCanTmp.u32ID.ID.RID = 0x00;
	
	TxCan.ExtId = TxCanTmp.u32ID.u32Id;
	TxCan.IDE = (u8)1 << 2;				//��չ֡
	TxCan.RTR = (u8)0;					//����֡
	
	if(u32Cmd == HEART_BEAT_CMD)
		TxCan.DLC = u8DLC + 4;
	else
		TxCan.DLC = u8DLC;
	
	if(u32Cmd == HEART_BEAT_CMD)
	{
		for(u8i = 0; u8i < TxCan.DLC; u8i++)
		{
			TxCan.Data[u8i] = *u8DataPtr;							//��������
			u8DataPtr++;
			if(u8i > 3)
			{
				TxCan.Data[u8i] -= 0x30;							//����汾��Ϣ
			}
		}
	}
	else
	{
		for(u8i = 0; u8i < u8DLC; u8i++)
		{
			TxCan.Data[u8i] = *u8DataPtr;
			u8DataPtr++;
		}
	}
	if(u32Cmd == HEART_BEAT_CMD)
	{
		while(u8i < 8)
			TxCan.Data[u8i++] = 0;
	}
	InsCanTxQueue(&TxCan);
}
/********************************************************************************************
** ������	��	 	s_CANxCheckValueTxProc
** �䡡��	��  	
** �䡡��	�� 		��
** ��������	��		Ҫͨ��CAN���ͼ�������ݴ���
** ������	��	 	����
** �ա���	��  	2014.10.22
** �汾		��    	V1.0A
** ���¼�¼	��
** 					��    ��      ��    ��                    ��      ��
** 					==========  =============  ========================================
**
*********************************************************************************************/
void s_CANxCheckValueTxProc(u32 u32Word,u8 u8Result)
{
	u8 u8TmpArray[6];	
	/*
	 * ������ֵ
	 */
	u8TmpArray[0] = (u8)(u32Word & 0x000000FF);
	u8TmpArray[1] = (u8)((u32Word & 0x0000FF00) >> 8);	
	u8TmpArray[2] = (u8)((u32Word & 0x00FF0000) >> 16);
	u8TmpArray[3] = (u8)((u32Word & 0xFF000000) >> 24);
	/*
	 * �������ֵ
	 */
	u8TmpArray[4] = u8Result;
	
	/*
	 * ����CAN���Ͷ���
	 */
	CanTxDataProc(u8TmpArray, 5,CHECK_RPT_CMD);
}
/********************************************************************************************
** ������	��	 	s_CANxCheckValueTxProc
** �䡡��	��  	
** �䡡��	�� 		��
** ��������	��		Ҫͨ��CAN���ͼ�������ݴ���
** ������	��	 	����
** �ա���	��  	2014.10.22
** �汾		��    	V1.0A
** ���¼�¼	��
** 					��    ��      ��    ��                    ��      ��
** 					==========  =============  ========================================
**
*********************************************************************************************/
void s_CANxCheckFaultTxProc(u32 u32Word,u8 u8Fault)
{
	
	u8 u8TmpArray[6];	
	/*
	 * ������ֵ
	 */
	u8TmpArray[0] = (u8)(u32Word & 0x000000FF);
	u8TmpArray[1] = (u8)((u32Word & 0x0000FF00) >> 8);	
	u8TmpArray[2] = (u8)((u32Word & 0x00FF0000) >> 16);
	u8TmpArray[3] = (u8)((u32Word & 0xFF000000) >> 24);
	/*
	 * �������ֵ
	 */
	u8TmpArray[4] = u8Fault;
	
	/*
	 * ����CAN���Ͷ���
	 */
	CanTxDataProc(u8TmpArray, 5,CHECK_FAULT_CMD);
}
/********************************************************************************************
*********************************************************************************************/ 

u32 trans_26To32(u32 actionWord)
{
	u32 actionWord_result = 0;
	u8  i = 0;
	u8  j = 0;

	for(j = 0;j < 32;j++)
    {
		if(actionWord&(1<<j))
        {
            for(i = 0;i < 32;i++)
            {
                if(EMV_32_Out_Table[i] == EMV_26_Out_Table[j])
                {
                    actionWord_result = actionWord_result|(1<<i);
                }
            }   
		}
	}	
			
	return actionWord_result;
}


u8	CRC_8(u8 *PData, u8 Len)
{
	u8	CRC_Temp=0;
	u8	Temp, i;
	u8	PData_H=0;
	u8	PData_L=0;

	for (i = 0; i < Len; i++)
	{
		PData_L=PData[i];
		if (i < (Len-1))
		{
			PData_H=PData[i+1];

			Temp=CRC_Temp>>4;
			CRC_Temp<<=4;
			CRC_Temp^=s_u8CRC_TABLE[Temp^(PData_H>>4)];
			Temp=CRC_Temp>>4;
			CRC_Temp<<=4;
			CRC_Temp^=s_u8CRC_TABLE[Temp^(PData_H&0x0F)];
			i++;
		}

		Temp=CRC_Temp>>4;
		CRC_Temp<<=4;
		CRC_Temp^=s_u8CRC_TABLE[Temp^(PData_L>>4)];
		Temp=CRC_Temp>>4;
		CRC_Temp<<=4;
		CRC_Temp^=s_u8CRC_TABLE[Temp^(PData_L&0x0F)];
	}
	
	return (CRC_Temp);
}




