#ifndef __CAN_PRO_H
#define __CAN_PRO_H
#include "includes.h"

#define ACTION_RXQUEUEMAX  100

#define ACTION_QUEUEMAX    200


#define CHECK_QUEUEMAX    200
#define CHECK_ACTION_QUEUEMAX    200


#define  CHIPACTION_TIMER_VAL  40
#define  MOSACTION_TIMER_VAL   0

/*
 * �������Ͷ���
 */
#define HEART_BEAT_CMD					0x00000000			//����������
#define ACTION_CMD							0x00000001			//����������
#define CHECK_CMD								0x00000002			//���������
#define CHECK_RPT_CMD						0x00000003			//����ϱ�������
#define CHECK_FAULT_CMD					0x00000004			//�����ϱ�������
/****************************************�������� ******************************************/
void s_CanRecvDataPro(void);    //CAN���ն����е����ݴ���
/***������ش�����**/
void CanCmdDataProc(u32 u32Cmd);
void s_Can1RxQueueProc(void);
void CanTxDataProc(u8 *u8DataPtr, u8 u8DLC,u32 u32Cmd);
void InsertActionRxQueue(u32 Action);
void s_ActionDataPro(void);
void s_ActionCmdExec(u32 u32Id);
void s_MosSet_Action(u32 Action);
void s_ChipSetAction(u32 Action);
void Stop_Action(u32 u32Id);
u8 SPI1_SendByte(u8 byte);
u32 TakeAction(u8 CtlWord, u8 data1, u8 data2);
/***�������ش�����***/
void InsertCheckQueue(u32 CheckCommand);
u16	NewCheckActionRx(void);
void s_CheckDataProc(void);
void Mos_CheckQueueSend(u32 checkword);
void Chip_CheckQueueSend(u32 checkword);
void s_TransmitCheckOut(u32 u32Word,u8 u8Result);
void s_CheckCmdExec(u32 u32Id);
void s_CANxCheckValueTxProc(u32 u32Word,u8 u8Result);
void s_MosCheckCmdExec(void);
/***�����ϴ�����***/
void s_CANxCheckFaultTxProc(u32 u32Word,u8 u8Fault);
/***��������������***/
u32 trans_26To32(u32 actionWord);
u8	CRC_8(u8 *PData, u8 Len);
void Var_Init(void);
#endif

