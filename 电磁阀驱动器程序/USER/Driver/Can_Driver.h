#ifndef __CAN_DRIVER_H
#define __CAN_DRIVER_H

/******************************************************************************************
* 文件名：		Can_Driver.h
* 作者：     	沈万江
* 版本：   		V1.0
* 日期：      	2014.10.22
* 功能描述:  	Can_Driver.c模块头文件定义
* 硬件信息：		
* 使用注意：	
* 修改说明：   
*
*       >>>>  在工程中的位置  <<<<
*               3-应用层
*               2-协议层
*          √   1-硬件驱动层
*********************************************************************************************
* @copy
* <h2><center>&copy; COPYRIGHT 天津华宁电子有限公司 研发中心 软件部</center></h2>
*********************************************************************************************/

/****************************************头文件包含******************************************/
#include "stm32f10x.h"
#include "includes.h"

/****************************************宏定义**********************************************/
/*
 * CAN1通讯接口使能，1，表示使用CAN与支架控制器进行通讯；0，表示使用UART与支架控制器进行通讯
 */
#define	EVMDCAN_ENABLED					1	//EVMDCAN通讯接口使能

#define CAN_RX_TIMER_VAL		 		10 	//CAN接收数据帧间隔计时器		

#define LIGHT_NUM						2   //指示灯数量

/****MCU所用到的端口定义****/
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


/**	CAN过滤器设置 **/
#define	SC_RID								0
#define EMVD_RID							1
#define HUB_RID								2
#define WL_RID								3
#define CXB_RID								4
#define RID_MASK							0x00000007
#define RID_BITS							0x00000007
#define CAN1_FILTER_ID_HIGH					(u16)(((EMVD_RID << 3) >> 16) & 0xffff)	//过滤器中标识符ID
#define CAN1_FILTER_ID_LOW					(u16)(((EMVD_RID << 3) & 0xffff) | CAN_ID_EXT | CAN_RTR_DATA)
#define CAN1_FILTER_MASK_ID_HIGH			(u16)(((RID_BITS << 3) >> 16) & 0xffff)	//过滤器中屏蔽位掩码ID
#define CAN1_FILTER_MASK_ID_LOW				(u16)(((RID_BITS << 3) & 0xffff) | CAN_ID_EXT | CAN_RTR_DATA)
/****************************************常量定义********************************************/
#define EMVD_RXCAN_SIZE 		200   //CANx接收缓存的大小
#define EMVD_TXCAN_SIZE 		200	  //CANx发送缓存的大小


/**CAN通信协议数据帧结构体定义**/

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
			u32	RD:2;				//保留位	
		}ID;
		u32 u32Id;
	}u32ID;
	u8 u8Data[8];
	u16 u16DLC;
}sCanComProtocolFrm;

/****************************************函数声明 ******************************************/
static void CanTrsDummy(CAN_TypeDef* CANx);
void SetCANSpeed(CAN_TypeDef *CANx, u16 u16CANxSpeed);
void CanVarInit(void);
void CanConfig(void);


#endif
