#include "includes.h"

#define EVMDCAN_HEARTBEAT_VAL  50

u32 Hare_Id; //定义硬件ID识别

extern u16 CAN1HeartbeatTimer;
extern u16 CANRxTimer;
/***存储CAN接收数据***/
extern u16  s_u16EmvdCanRxQueueHeadPtr;
extern u16  s_u16EmvdCanRxQueueEndPtr;

extern u16  s_u16EmvdCanRxQueueCnt;
extern u16	s_u16EmvdCanTxQueueCnt;
const u8 cu8HeartBeatData[8] = {'E','M','V','D','0','2','6',0x4f};//硬密封电磁阀版本从2.6.30开始(0x4e -0x30)
u32 u32i = 0xffff;
/********************************************************************************************
** 函数名	：		s_PowerUpTransThreeFrame
** 输　入	：		无
** 输　出	：		无
** 功能描述	：		初始上电后发送三帧心跳数据
** 作　者	：		沈万江
** 日　期	：		2014.10.22
** 版本		：		V1.0A
** 更新记录	：
** 					日    期      姓    名                    描      述
** 					==========  =============  ========================================
**
*********************************************************************************************/
static void s_PowerUpTransThreeFrame(void)
{
	u8 u8i = 0;
	while(u8i < 3)
	{
		if (CAN1HeartbeatTimer == TIMER_EXPIRED)
		{
			CanTxDataProc((u8 *)&cu8HeartBeatData , 4 , HEART_BEAT_CMD);
			CAN1HeartbeatTimer = EVMDCAN_HEARTBEAT_VAL;
			u8i++;
		}
	}
}
int main(void)
{
	RCC_Configuration();
	SysTick_Configuration();
	ADC_Configuration();
	CanConfig();
	HareId_Init(); //ghy 硬件ID初始化
	if(Hare_Id == DEVICE_MOS)
	{
		MosGPIO_Configuration();
		MosValue_PowerOn;
		NMOS_CON_On;
		OutPoint_Init();		
	}
	else
	{
		ChipGPIO_Configuration();
		MC_RST;										   		//MC非复位
		MC_NO_RST;
		ChipVale_PowerOn;
		SPI1_Init();
		ChipVale_PowerOn;
	}
	#ifdef IWDG_ENABLED
		IWDG_Configuration();	
	#endif
	CanVarInit();
	CanIapInint();
	Var_Init();
	s_PowerUpTransThreeFrame(); 
	while(1)
	{	
		/*
		*CAN通讯指示灯
		*/
		s_LEDStatusProc(Hare_Id);		
		/*
		CAN接收队列处理
		*/		
		s_Can1RxQueueProc();		   		
		/*
		CAN接收队列中的数据处理
		*/
		 s_CanRecvDataPro();		
		/*
		定时500ms发送EMVD心跳
		*/
		if(CAN1HeartbeatTimer == TIMER_EXPIRED)
		{
			 if(!s_u16EmvdCanRxQueueCnt && !s_u16EmvdCanTxQueueCnt)
				 CanTxDataProc((u8 *)&cu8HeartBeatData , 4 , HEART_BEAT_CMD);			 
			 CAN1HeartbeatTimer = EVMDCAN_HEARTBEAT_VAL;
		}
		/*
		*CAN接收定时器是否到时，如果到时停止当前正在执行的动作
		*/
		if((CANRxTimer == TIMER_EXPIRED)&&(EVMDCAN_ENABLED == 1))
		{
				CANRxTimer = TIMER_CLOSED;
				Stop_Action(Hare_Id);
				s_u16EmvdCanRxQueueHeadPtr = s_u16EmvdCanRxQueueEndPtr;
				s_u16EmvdCanRxQueueCnt = 0;
				if(Hare_Id == DEVICE_MOS)
					 MosValue_PowerOff;//空闲状态时，切断电源
		}
		#ifdef	IWDG_ENABLED
			IWDG_Feed();
		#endif
	}
}



