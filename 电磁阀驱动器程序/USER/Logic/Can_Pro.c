/******************************************************************************************
* 文件名：		Can_Pro.c
* 作者：    		沈万江
* 版本：   		V1.0
* 日期：      	2014.10.22
* 功能描述:  	负责电磁阀驱动器与支架控制器之间的CAN数据通信。该模块包含底层驱动和应用层程序。
* 硬件信息：		
* 使用注意：	
* 修改说明：   
*
*       >>>>  在工程中的位置  <<<<
*          √   3-应用层
*          √   2-协议层
*          √   1-硬件驱动层
*********************************************************************************************
* @copy
* <h2><center>&copy; COPYRIGHT 天津华宁电子有限公司 研发中心 软件部</center></h2>
*********************************************************************************************/
/****************************************头文件包含******************************************/
#include "Can_Pro.h"
#include "includes.h"
/****************************************常量定义******************************************/
static u8	s_u8CRC_TABLE[]= {0x00,0x31,0x62,0x53,0xC4,0xF5,0xA6,0x97,0xB9,0x88,0xDB,0xEA,0x7D,0x4C,0x1F,0x2E};	 //CRC校验表

//32接口板对应26的接口顺序 
static const int EMV_26_Out_Table[32] ={22,21,20,19,18,17,16,15,14,13,27,28,26,25,24,23, 7, 8, 5, 6, 3, 4, 1, 2,11,12,10, 9,29,30,31,32};
      
//32功能33996驱动输出顺序     
static const int EMV_32_Out_Table[32] ={24,19,26,21,25,23,29,30,11,12,16,15,18,20,22,17, 2,32, 1,31, 6,28, 4,27,10, 5, 8, 3, 9,14, 7,13};
/****************************************动作变量定义******************************************/
u32 ActionRxQueue[ACTION_RXQUEUEMAX];//接收动作帧环形队列
u16 ActionRxQueueHead;//起始位置
u16 ActionRxQueueEnd;//结束位置
	
u32 ActionQueue[ACTION_QUEUEMAX]; //动作执行环形队列
u16 ActionQueueHead;//起始位置
u16 ActionQueueEnd;//结束位置

u32 ActionWord = 0x00000000; //液压动作有效值
u32 LastAction = 0x00000000; //记录上次的动作命令字
static u32 u32newAct;
static u32 u32Act;
extern u16 ActionTimer;
/*****************************************阀检测相关变量定义*********************************************/
u32 CheckData = 0x00000000;   //检测的电磁阀输出口
u32 CheckWord = 0x00000000;   //记录阀检测动作有效值
u32 CheckNewData = 0x00000000; 	//从接收阀检测队列中新获取的输出口
	
static u32 	FaultWord 		= 0x00000000;		 //SPI返回值，即MC33996反馈的数据
static u32 	FaultWord1 		= 0x00000000;		//
static u32 	FaultWord2 		= 0x00000000;	   //

static u8   CommandByte1	= 0;
static u8	CommandByte2	= 0;
static u8	CommandByte3	= 0;
static u8	CommandByte4	= 0;

u32 VoltValue = 0x00000000;	//ADC1滤波过后的数据
	
u32 CheckQueue[CHECK_QUEUEMAX];//接收阀检测动作命令缓存队列
u16 CheckQueueHead;//起始位置
u16 CheckQueueEnd;//结束位置	

u32 CheckActionQueue[CHECK_ACTION_QUEUEMAX]; //阀检测命令缓存队列
u16 CheckActionQueueHead;//起始位置
u16 CheckActionQueueEnd;//结束位置
		
extern sCanComProtocolFrm  s_EmvdCanRxQueue[EMVD_RXCAN_SIZE]; 
extern CanTxMsg   		   s_EmvdCanTxQueue[EMVD_TXCAN_SIZE];

/***存储CAN接收数据***/
extern u16  s_u16EmvdCanRxQueueHeadPtr;
extern u16  s_u16EmvdCanRxQueueEndPtr;
extern u16  s_u16EmvdCanRxQueueCnt;

/***存储CAN发送数据***/
extern u16  s_u16EmvdCanTxQueueHeadPtr;
extern u16  s_u16EmvdCanTxQueueEndPtr;
extern u16  s_u16EmvdCanTxQueueCnt;

/***硬件Id***/
extern u32 Hare_Id;

/**********************************定义动作常量************************************************/
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
** 函数名	：	 	s_Can1RxQueueProc
** 输　入	：  	
** 输　出	： 		无
** 功能描述	：		对CAN接收队列中的数据进行处理。
** 作　者	：	 	沈万江
** 日　期	：  	2014.10.22s
** 版本		：    	V1.0A
** 更新记录	：
** 					日    期      姓    名                    描      述
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
	 * 判断接收队列是否有数据，有数据则进行处理
	*/
	while(i)
	{
		/*
		 * 根据帧ID调用相应的处理函数
		 */
		if(Hare_Id == DEVICE_MOS)
			MosValue_PowerOn; //收到数据时，阀组供电开启
		//add接收程序更新帧
		if((s_EmvdCanRxQueue[s_u16EmvdCanRxQueueHeadPtr].u32ID.ID.RD))
		{
			/*
			 * 收到传输的程序帧时的处理
			 */
			CanRecvProgProc((CXB_CAN_FRAME_TYPE *)sRcvCan);
		}
		else
		{
			switch(s_EmvdCanRxQueue[s_u16EmvdCanRxQueueHeadPtr].u32ID.ID.FT)
			{
				case ACTION_CMD:
					CanCmdDataProc(ACTION_CMD);				//动作命令处理函数
				break;
				
				case CHECK_CMD:
					CanCmdDataProc(CHECK_CMD);				//检测命令处理函数
				break;
				default:
				break;
			}
		}
		/*
		 * 队列中数据帧个数-1，读指针+1，此时应该关闭中断，防止改变量时，中断也修改该变量
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
** 函数名	：	 	CanCmdDataProc
** 输　入	：  	
** 输　出	： 		无
** 功能描述	：		CAN接收数据进行处理。
** 作　者	：	 	沈万江
** 日　期	：  	2014.10.22
** 版本		：    	V1.0A
** 更新记录	：
** 					日    期      姓    名                    描      述
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
	 * 取出CAN数据帧中的数据，组成有效数据，根据动作类型，插入相关的队列
	 */
	if(s_u16EmvdCanRxQueueCnt)
	{
		u8TmpDLC = s_EmvdCanRxQueue[s_u16EmvdCanRxQueueHeadPtr].u16DLC;
		u8SaveCrc = s_EmvdCanRxQueue[s_u16EmvdCanRxQueueHeadPtr].u8Data[u8TmpDLC - 1];//保存接收数据的校验结果
				
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
			InsertActionRxQueue(trans_26To32(u32Data));					//动作数据加入队列
		else if(u32Cmd == CHECK_CMD)
        {
            CheckData = s_EmvdCanRxQueue[s_u16EmvdCanRxQueueHeadPtr].u8Data[0];
            CheckData = CheckData|s_EmvdCanRxQueue[s_u16EmvdCanRxQueueHeadPtr].u8Data[1] << 8;
            CheckData = CheckData| s_EmvdCanRxQueue[s_u16EmvdCanRxQueueHeadPtr].u8Data[2] << 16;
            CheckData = CheckData|s_EmvdCanRxQueue[s_u16EmvdCanRxQueueHeadPtr].u8Data[3] << 24;                                             
			  InsertCheckQueue(trans_26To32(u32Data));						//检测命令加入队列
        }
	}
}
 /*********************************************************************************************************************
  **function name:	   InsertActionRxQueue()
  **description:	   将解析出来的动作数据放入动作接收队列
  **input value:	   无
  **output value:　	　 无
  **return value:	　 无
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
			ActionQueue[ActionQueueEnd++] = j;			//将合动作拆分存入动作队列中
			ActionQueueEnd %= ACTION_QUEUEMAX;
		}
		j <<= 1;
	}
}
/*********************************************************************************************************************
  **function name:	   s_CanRecvDataPro()
  **description:	   CAN接收数据处理
  **input value:	   无
  **output value:　	　 无
  **return value:	　 无
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
		*CAN接收的动作数据处理
		*/
		s_ActionDataPro();
		/*
		*执行动作队列中的动作处理
		*/
		s_ActionCmdExec(Hare_Id);
		
		/*
		*CAN接收的阀检测数据处理
		*/
		s_CheckDataProc();
		
		/*
		*执行阀检测操作队列中
		*/
		s_CheckCmdExec(Hare_Id);
	}
	else
	{
		/*
		*CAN接收的动作数据处理
		*/
		s_ActionDataPro();
		/*
		*执行动作队列中的动作处理
		*/
		s_ActionCmdExec(Hare_Id);
		
		/*
		*CAN接收的阀检测数据处理
		*/
		s_CheckDataProc();
		
		/*
		*执行阀检测操作队列中
		*/
		s_CheckCmdExec(Hare_Id);
	}
}
void s_ActionDataPro(void)
{
	/*
	*判断动作队列中是否有数据
	*/
	if(NewActionRxQueue())
	{
		u32newAct = ActionRxQueue[ActionRxQueueHead++];
		ActionRxQueueHead %= ACTION_RXQUEUEMAX;
		/*
		*新接收的数据处理，插入动作命令队列
		*/
		u32Act = ActionWord;
		ActionWord &= u32newAct;
		 
		if((ActionWord & LastAction) ==0)
			ActionTimer = TIMER_EXPIRED;
	  
		 u32newAct |= u32Act;
		 u32newAct ^= u32Act; //取得新动作
		 if(u32newAct || (u32Act != ActionWord))
			 InsertActionQueue(u32newAct);
	}	
}

void s_ActionCmdExec(u32 u32Id)
{
	/*
	*判断动作定时器时间到与否，即判断上一个电磁阀动作稳定否，即两个电磁阀先导阀动作要有一个时间间隔TIMOUT
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
				 * 开动作间隔计时器
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
  **description:	   芯片版触发动作
  **input value:	 　u32 Action 动作ID
  **output value:　	　　无
  **return value:	　　无
**********************************************************************************************************************/
void s_ChipSetAction(u32 Action)
{
	/*
	 * 配置发送给MC33996芯片的数据
	 */
	//CommandByte1 = (u8)(ActionWord & 0x000000ff);				//8个
	//CommandByte2 = (u8)((ActionWord & 0x0000f300) >> 8);		//6个
	//CommandByte3 = (u8)((ActionWord & 0x00ff0000) >> 16);		//8个
	//CommandByte4 = (u8)((ActionWord & 0x0f000000) >> 24);		//4个
	/*
	 * 下面对应控制命令为全32功能，发送给MC33996芯片的数据
	 */
	CommandByte1 = (u8)(Action & 0x000000ff);				//8个
	CommandByte2 = (u8)((Action & 0x0000ff00) >> 8);		//8个
	CommandByte3 = (u8)((Action & 0x00ff0000) >> 16);		//8个
	CommandByte4 = (u8)((Action & 0xff000000) >> 24);		//8个

	/*
	 * 片选1，发送数据
	 */
	CS1_DISABLE;
	Delay(1);
	FaultWord1 = TakeAction(0x00,CommandByte2,CommandByte1);
	CS1_ENABLE;				
	/*
	 * 片选2，发送数据
	 */
	CS2_DISABLE;
	Delay(1);
	FaultWord2 = TakeAction(0x00,CommandByte4,CommandByte3);
	CS2_ENABLE;

	/*
	 * FaultWord表示MC33996输出口有问题，具体详细信息可以参见MC33996芯片手册
	 */
	FaultWord = FaultWord1 + (FaultWord2 << 16);
					/*
	* 发送动作命令对应MC33996输出口的错误状态
	 */
	if (FaultWord)
	{
		s_TransmitCheckOut(FaultWord, 0x11);
		FaultWord = 0;
	}
}

/*********************************************************************************************************************
  **function name:	   TakeAction()
  **description:	   发送动作队列或者检测动作队列中的数据
  **input value:	   u8 CtlWord，MC33996芯片中控制字, u8 data1, u8 data2
  **output value:
  **return value:	   t,MC33996反馈数据
**********************************************************************************************************************/
u32 TakeAction(u8 CtlWord, u8 data1, u8 data2)
{
	u32 t1,t2,t3,t;

	t1 = SPI1_SendByte(CtlWord);					//往MC33996芯片中的24位移位寄存器中写数据控制字
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
  **description:	   触发MOS版本IO动作
  **input value:	 　u32 Action 动作ID
  **output value:　	　　无
  **return value:	　　无
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
  **description:	   停止动作命令
  **input value:	 　u32 u32Id   硬件ID识别
  **output value:　	　　无
  **return value:	　　无
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
	ActionTimer = TIMER_EXPIRED;  //关闭动作间隔计时器
}

/*****************************************以下为阀检测数据处理内容**************************************************************/


/*******************************************************************************************
  **function name: InsertCheckQueue()
  **description:   与上一个函数类似，CAN接收到的数据分析分为动作数据和检测数据，阀检测数据存
  **               入阀检测数据环形队列 
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
  **description:   判断CAN接收环形缓冲区中是否有数据，如果此时检测动作队列中有数据就要先执行剩余的检测动作
  **               如果没有检测动作了，就返回检测动作队列中是否有数据 
  **input value:   
  **output value:
  **return value:  1，检测动作队列中是否有数据，0，表示没有
*******************************************************************************************/
u16	NewCheckActionRx(void)
{
	if (CheckActionQueueHead != CheckActionQueueEnd)
	    return(0);	 //先执行剩余有效检测动作
	return (CheckQueueHead != CheckQueueEnd);
}
/*******************************************************************************************
  **function name: InsertCheckActionQueue()
  **description:   判断从检测动作队列中取出有新动作，将其存入执行检测动作队列中 
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
  **description:   阀检测动作队列数据处理
  **input value:   无
  **output value:
  **return value:  
*******************************************************************************************/
void s_CheckDataProc(void)
{
	/*
	*判断检测队列中是否有数据
	*/
	if(NewCheckActionRx())
	{
		CheckNewData = CheckQueue[CheckQueueHead++];
		CheckQueueHead %= CHECK_QUEUEMAX;		
		/*
		*新接收的数据是否与上次相同，不相同，则插入检测命令动作队列
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
  **description:   Mos管阀检测时使能检测管脚
  **input value:   无
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
  **description:	   发送动作队列或者检测动作队列中的数据
  **input value:	   u8 CtlWord，MC33996芯片中控制字, 
						u8 data1,16个输出口的高8位
						u8 data2,16个输出口的低8位
  **output value:
  **return value:	   t,MC33996反馈数据
**********************************************************************************************************************/
void TakeCheckAction(u8 CtlWord, u8 data1, u8 data2)
{
	SPI1_SendByte(CtlWord);
	SPI1_SendByte(data1);
	SPI1_SendByte(data2);
}
/*********************************************************************************************************************
  **function name:	   Chip_CheckQueueSend(u32 CheckWord)
  **description:	     阀检测时使能检测管脚
  **input value:	   
  **output value:
  **return value:      1，空；0，非空
**********************************************************************************************************************/
void Chip_CheckQueueSend(u32 checkword)  
{
	u8 CheckByte1,CheckByte2,CheckByte3,CheckByte4; 
	//得到要发送的指令值
	// 	CheckByte1 = (u8)(checkword & 0x000000ff);				//8
	// 	CheckByte2 = (u8)((checkword & 0x0000f300) >> 8);		//6
	// 	CheckByte3 = (u8)((checkword & 0x00ff0000) >> 16);		//8
	// 	CheckByte4 = (u8)((checkword & 0x0f000000) >> 24);		//4
	/*
	 * 下面对应控制命令为全32功能，发送给MC33996芯片的数据
	 */
	CheckByte1 = (u8)(checkword & 0x000000ff);				//8
	CheckByte2 = (u8)((checkword & 0x0000ff00) >> 8);		//8
	CheckByte3 = (u8)((checkword & 0x00ff0000) >> 16);		//8
	CheckByte4 = (u8)((checkword & 0xff000000) >> 24);		//8
	
	//选中第一个MC33996芯片，发送低字的数据命令
	CS1_DISABLE;
	Delay(1);
	TakeCheckAction(0x00,CheckByte2,CheckByte1);
	CS1_ENABLE;
	//选中第二个MC33996芯片，发送高字的数据命令
	CS2_DISABLE;
	Delay(1);
	TakeCheckAction(0x00,CheckByte4,CheckByte3);
	CS2_ENABLE;
}
void s_TransmitCheckOut(u32 u32Word,u8 u8Result)
{
	if(EVMDCAN_ENABLED == 1)
		s_CANxCheckValueTxProc(u32Word,u8Result);		//通过CAN发送
	VoltValue = 0;	
}
/****************************************************
  **function name:	   s_MosCheckCmdExec()
  **description:	   Mos版阀检测处理函数
  **input value:	   无
  **output value:　	　 无
  **return value:	　 无
******************************************************/
void  s_MosCheckCmdExec(void)
{
	 /*
	 * 判断检测动作队列中是否有检测命令，如果有则执行
	 */
	if(CheckActionQueueHead != CheckActionQueueEnd)
	{
		/*
		 * 切断阀检测部分电源，
		 */
		MosValue_PowerOff;
		Delay(2);
		/*
		 * 取新检测动作命令
		 */
		CheckNewData = CheckActionQueue[CheckActionQueueHead++];
		CheckActionQueueHead %= CHECK_ACTION_QUEUEMAX;
		CheckWord |= CheckNewData;
		CheckWord &= CheckNewData;
		
		/*
		 * 如果有新检测动作命令，则执行
		 */
		if(CheckWord)
		{
				Mos_CheckQueueSend(CheckWord);					//选中要检测的电磁阀
				Delay(5);
				Get_Samples_Val();							//获得十次采样值
				VoltValue = SampVal_Proc();					//滤波处理
			
			/*
			 * 选择CAN，则使用CAN-Tx进行发送。都是选择中断发送。
			*/
				/*
				 * 根据采样值判断电磁先导阀是否在线，0x08：短路；0x04：断路；0x01：正常
				 * 短路表示先导阀短路，断路表示先导阀未接，正常表示先导阀连接并能正常使用
				 */
				if(VoltValue < ShortValueMax)
				{
					s_TransmitCheckOut(CheckData,0x08);			//发送检测结果
				}
				if((VoltValue >= FormalValueMin) && (VoltValue <= FormalValueMax))
				{
					s_TransmitCheckOut(CheckData,0x01);			//发送检测结果
				}
				if((VoltValue >= OpenValueMin) && (VoltValue <= OpenValueMax))
				{
					s_TransmitCheckOut(CheckData,0x04);			//发送检测结果
				}
		}
		Mos_CheckQueueSend(0x00000000);
		Delay(2);
		/*
		 * 恢复阀检测电路上的电源供电
		 */
		MosValue_PowerOn;		
	}	
}
/****************************************************
  **function name:	   s_ChipCheckCmdExec()
  **description:	   芯片版阀检测处理函数
  **input value:	   无
  **output value:　	　 无
  **return value:	　 无
******************************************************/
void s_ChipCheckCmdExec(void)
{
	/*
	 * 判断检测动作队列中是否有检测命令，如果有则执行
	 */
	if(CheckActionQueueHead != CheckActionQueueEnd)
	{
		/*
		 * 切断MC33996芯片的电源；要进行电磁阀检测必须切断MC33996芯片的电源
		 */
		ChipVale_PowerOff;
		Delay(2);
		/*
		 * 取新检测动作命令
		 */
		CheckNewData = CheckActionQueue[CheckActionQueueHead++];
		CheckActionQueueHead %= CHECK_ACTION_QUEUEMAX;
		CheckWord |= CheckNewData;
		CheckWord &= CheckNewData;		
		/*
		 * 如果有新检测动作命令，则执行
		 */
		if(CheckWord)
		{
			Chip_CheckQueueSend(CheckWord);					//选中要检测的电磁阀
			Delay(5);
			Get_Samples_Val();							//获得十次采样值
			VoltValue = SampVal_Proc();					//滤波处理			
			/*
			 * 将检测的结果发送给支架控制器，如果选择使用UART，则使用UART-Tx发送；
			 * 如果选择CAN，则使用CAN-Tx进行发送。都是选择中断发送。
			 */
			// if (u8i < 2)
			// {
			// 		u8i++;
			// }
			// else
			// {
				/*
				 * 根据采样值判断电磁先导阀是否在线，0x08：短路；0x04：断路；0x01：正常
				 * 短路表示先导阀短路，断路表示先导阀未接，正常表示先导阀连接并能正常使用
				 */
				if(VoltValue < ShortValueMax)
				{
					s_TransmitCheckOut(CheckData,0x08);			//发送检测结果
				}
				if((VoltValue >= FormalValueMin) && (VoltValue <= FormalValueMax))
				{
					s_TransmitCheckOut(CheckData,0x01);			//发送检测结果
				}
				if((VoltValue >= OpenValueMin) && (VoltValue <= OpenValueMax))
				{
					s_TransmitCheckOut(CheckData,0x04);			//发送检测结果
				}
// 			}
		}
		Chip_CheckQueueSend(0x00000000);
		Delay(2);
		/*
		 * 恢复MC33996芯片的电源
		 */
		ChipVale_PowerOn;
	}
}
/********************************************************************************************
** 函数名	：	 	s_CheckCmdExec
** 输　入	：  	无
** 输　出	： 		无       
** 功能描述	：		阀检测数据处理方式
** 作　者	：	 	沈万江
** 日　期	：  	2014.10.22
** 版本		：    	V1.0A
** 更新记录	：
** 					日    期      姓    名                    描      述
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
  **description:   在实验中的定时器和数据缓存区进行初始化， 
  **input value:
  **output value:
  **return value:
*******************************************************************************************/
void Var_Init(void)
{
	ActionTimer = TIMER_EXPIRED;  //动作间隔计时
	ActionRxQueueHead =0;
	ActionRxQueueEnd =0;
	ActionQueueHead =0;
	ActionQueueEnd =0;	
}
/********************************************************************************************
** 函数名	：	 	CRC_8
** 输　入	：  	unsigned char *PData,unsigned char Len
** 输　出	： 		CRC校验结果
** 功能描述	：		8位CRC校验函数
** 作　者	：	 	沈万江
** 日　期	：  	2014.10.22
** 版本		：    	V1.0A
** 更新记录	：
** 					日    期      姓    名                    描      述
** 					==========  =============  ========================================
**
*********************************************************************************************/
/********************************************************************************************
** 函数名	：	 	CanTxDataProc
** 输　入	：  	
** 输　出	： 		无
** 功能描述	：		CAN发送数据进行处理。
** 作　者	：	 	沈万江
** 日　期	：  	2014.10.22
** 版本		：    	V1.0A
** 更新记录	：
** 					日    期      姓    名                    描      述
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
		 * 数据加入发送队列后，使能发送中断
		 */
		CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE);			//CAN1发送邮箱空中断使能
	}
}

/********************************************************************************************
** 函数名	：	 	CanTxDataProc
** 输　入	：  	
** 输　出	： 		无
** 功能描述	：		CAN发送数据进行处理。
** 作　者	：	 	沈万江
** 日　期	：  	2014.10.22
** 版本		：    	V1.0A
** 更新记录	：
** 					日    期      姓    名                    描      述
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
	TxCan.IDE = (u8)1 << 2;				//扩展帧
	TxCan.RTR = (u8)0;					//数据帧
	
	if(u32Cmd == HEART_BEAT_CMD)
		TxCan.DLC = u8DLC + 4;
	else
		TxCan.DLC = u8DLC;
	
	if(u32Cmd == HEART_BEAT_CMD)
	{
		for(u8i = 0; u8i < TxCan.DLC; u8i++)
		{
			TxCan.Data[u8i] = *u8DataPtr;							//心跳数据
			u8DataPtr++;
			if(u8i > 3)
			{
				TxCan.Data[u8i] -= 0x30;							//软件版本信息
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
** 函数名	：	 	s_CANxCheckValueTxProc
** 输　入	：  	
** 输　出	： 		无
** 功能描述	：		要通过CAN发送检测结果数据处理。
** 作　者	：	 	沈万江
** 日　期	：  	2014.10.22
** 版本		：    	V1.0A
** 更新记录	：
** 					日    期      姓    名                    描      述
** 					==========  =============  ========================================
**
*********************************************************************************************/
void s_CANxCheckValueTxProc(u32 u32Word,u8 u8Result)
{
	u8 u8TmpArray[6];	
	/*
	 * 检测命令赋值
	 */
	u8TmpArray[0] = (u8)(u32Word & 0x000000FF);
	u8TmpArray[1] = (u8)((u32Word & 0x0000FF00) >> 8);	
	u8TmpArray[2] = (u8)((u32Word & 0x00FF0000) >> 16);
	u8TmpArray[3] = (u8)((u32Word & 0xFF000000) >> 24);
	/*
	 * 检测结果赋值
	 */
	u8TmpArray[4] = u8Result;
	
	/*
	 * 加入CAN发送队列
	 */
	CanTxDataProc(u8TmpArray, 5,CHECK_RPT_CMD);
}
/********************************************************************************************
** 函数名	：	 	s_CANxCheckValueTxProc
** 输　入	：  	
** 输　出	： 		无
** 功能描述	：		要通过CAN发送检测结果数据处理。
** 作　者	：	 	沈万江
** 日　期	：  	2014.10.22
** 版本		：    	V1.0A
** 更新记录	：
** 					日    期      姓    名                    描      述
** 					==========  =============  ========================================
**
*********************************************************************************************/
void s_CANxCheckFaultTxProc(u32 u32Word,u8 u8Fault)
{
	
	u8 u8TmpArray[6];	
	/*
	 * 检测命令赋值
	 */
	u8TmpArray[0] = (u8)(u32Word & 0x000000FF);
	u8TmpArray[1] = (u8)((u32Word & 0x0000FF00) >> 8);	
	u8TmpArray[2] = (u8)((u32Word & 0x00FF0000) >> 16);
	u8TmpArray[3] = (u8)((u32Word & 0xFF000000) >> 24);
	/*
	 * 检测结果赋值
	 */
	u8TmpArray[4] = u8Fault;
	
	/*
	 * 加入CAN发送队列
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




