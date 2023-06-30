#ifndef __CAN_DRIVER_H
#define __CAN_DRIVER_H

/******************************************************************************************
* �ļ�����		Can_Driver.h
* ���ߣ�     	����
* �汾��   		V1.0
* ���ڣ�      	2014.10.22
* ��������:  	Can_Driver.cģ��ͷ�ļ�����
* Ӳ����Ϣ��		
* ʹ��ע�⣺	
* �޸�˵����   
*
*       >>>>  �ڹ����е�λ��  <<<<
*               3-Ӧ�ò�
*               2-Э���
*          ��   1-Ӳ��������
*********************************************************************************************
* @copy
* <h2><center>&copy; COPYRIGHT ������������޹�˾ �з����� �����</center></h2>
*********************************************************************************************/

/****************************************ͷ�ļ�����******************************************/
#include "stm32f10x.h"
#include "includes.h"

/****************************************�궨��**********************************************/
/*
 * CAN1ͨѶ�ӿ�ʹ�ܣ�1����ʾʹ��CAN��֧�ܿ���������ͨѶ��0����ʾʹ��UART��֧�ܿ���������ͨѶ
 */
#define	EVMDCAN_ENABLED					1	//EVMDCANͨѶ�ӿ�ʹ��

#define CAN_RX_TIMER_VAL		 		10 	//CAN��������֡�����ʱ��		

#define LIGHT_NUM						2   //ָʾ������

/****MCU���õ��Ķ˿ڶ���****/
#define EMVD_CAN         	 CAN1
#define EMVD_CAN_PORT  		 GPIOA
#define EMVD_CAN_RX_PIN  	 GPIO_Pin_11
#define EMVD_CAN_TX_PIN 	 GPIO_Pin_12
#define EMVD_CAN_GPIO_RCC 	 RCC_APB2Periph_GPIOA
#define EMVD_CAN_RCC 		 RCC_APB1Periph_CAN1
#define EMVD_REMAP1_CAN    	 GPIO_Remap1_CAN1
#define EMVD_REMAP2_CAN		 GPIO_Remap2_CAN1
#define EMVD_CAN_RX_IRQN   	 USB_LP_CAN1_RX0_IRQn
#define EMVD_CAN_TX_IRQN   	 USB_HP_CAN1_TX_IRQn


/**	CAN���������� **/
#define	SC_RID								0
#define EMVD_RID							1
#define HUB_RID								2
#define WL_RID								3
#define CXB_RID								4
#define RID_MASK							0x00000007
#define RID_BITS							0x00000007
#define CAN1_FILTER_ID_HIGH					(u16)(((EMVD_RID << 3) >> 16) & 0xffff)	//�������б�ʶ��ID
#define CAN1_FILTER_ID_LOW					(u16)(((EMVD_RID << 3) & 0xffff) | CAN_ID_EXT | CAN_RTR_DATA)
#define CAN1_FILTER_MASK_ID_HIGH			(u16)(((RID_BITS << 3) >> 16) & 0xffff)	//������������λ����ID
#define CAN1_FILTER_MASK_ID_LOW				(u16)(((RID_BITS << 3) & 0xffff) | CAN_ID_EXT | CAN_RTR_DATA)
/****************************************��������********************************************/
#define EMVD_RXCAN_SIZE 		200   //CANx���ջ���Ĵ�С
#define EMVD_TXCAN_SIZE 		200	  //CANx���ͻ���Ĵ�С


/**CANͨ��Э������֡�ṹ�嶨��**/

typedef struct{
	union {
		struct{
			u32 RID:3;
			u32 TID:3;
			u32 FT:10;
			u32 SN:4;
			u32 SUM:5;
			u32 SUB:1;
			u32 ACK:1;
			u32	RD:2;				//����λ	
		}ID;
		u32 u32Id;
	}u32ID;
	u8 u8Data[8];
	u16 u16DLC;
}sCanComProtocolFrm;

/****************************************�������� ******************************************/
static void CanTrsDummy(CAN_TypeDef* CANx);
void SetCANSpeed(CAN_TypeDef *CANx, u16 u16CANxSpeed);
void CanVarInit(void);
void CanConfig(void);


#endif
