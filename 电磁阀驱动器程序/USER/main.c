#include "includes.h"

#define EVMDCAN_HEARTBEAT_VAL  50

u32 Hare_Id; //����Ӳ��IDʶ��

extern u16 CAN1HeartbeatTimer;
extern u16 CANRxTimer;
/***�洢CAN��������***/
extern u16  s_u16EmvdCanRxQueueHeadPtr;
extern u16  s_u16EmvdCanRxQueueEndPtr;

extern u16  s_u16EmvdCanRxQueueCnt;
extern u16	s_u16EmvdCanTxQueueCnt;
const u8 cu8HeartBeatData[8] = {'E','M','V','D','0','2','6',0x4f};//Ӳ�ܷ��ŷ��汾��2.6.30��ʼ(0x4e -0x30)
u32 u32i = 0xffff;
/********************************************************************************************
** ������	��		s_PowerUpTransThreeFrame
** �䡡��	��		��
** �䡡��	��		��
** ��������	��		��ʼ�ϵ������֡��������
** ������	��		����
** �ա���	��		2014.10.22
** �汾		��		V1.0A
** ���¼�¼	��
** 					��    ��      ��    ��                    ��      ��
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
	HareId_Init(); //ghy Ӳ��ID��ʼ��
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
		MC_RST;										   		//MC�Ǹ�λ
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
		*CANͨѶָʾ��
		*/
		s_LEDStatusProc(Hare_Id);		
		/*
		CAN���ն��д���
		*/		
		s_Can1RxQueueProc();		   		
		/*
		CAN���ն����е����ݴ���
		*/
		 s_CanRecvDataPro();		
		/*
		��ʱ500ms����EMVD����
		*/
		if(CAN1HeartbeatTimer == TIMER_EXPIRED)
		{
			 if(!s_u16EmvdCanRxQueueCnt && !s_u16EmvdCanTxQueueCnt)
				 CanTxDataProc((u8 *)&cu8HeartBeatData , 4 , HEART_BEAT_CMD);			 
			 CAN1HeartbeatTimer = EVMDCAN_HEARTBEAT_VAL;
		}
		/*
		*CAN���ն�ʱ���Ƿ�ʱ�������ʱֹͣ��ǰ����ִ�еĶ���
		*/
		if((CANRxTimer == TIMER_EXPIRED)&&(EVMDCAN_ENABLED == 1))
		{
				CANRxTimer = TIMER_CLOSED;
				Stop_Action(Hare_Id);
				s_u16EmvdCanRxQueueHeadPtr = s_u16EmvdCanRxQueueEndPtr;
				s_u16EmvdCanRxQueueCnt = 0;
				if(Hare_Id == DEVICE_MOS)
					 MosValue_PowerOff;//����״̬ʱ���жϵ�Դ
		}
		#ifdef	IWDG_ENABLED
			IWDG_Feed();
		#endif
	}
}



