/**
  ******************************************************************************
  * @file    SPI/SPI_FLASH/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 
/****************************************************************************************************
** 	公司：华宁电子
**	功能：电磁阀驱动器。实现与支架控制器通过RS232串口交换数据，然后控制多个电磁阀动作。
**  版本：V1.0  
**	硬件信息：外部晶振：16MHz；CPU：STM32F103RBT6；SPI1（SCLK=PA5、MISO=PA6、MOSI=PA7）、PWM1=PC12、PWM2=PC10、
**  RST=PD2、CS1=PB5、CS2=PB6；USART3（USART3_TX=PB10、USART3_RX=PB11），电磁阀驱动芯片：MC33996
**	作者：Aaron 2013/1/13   
****************************************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "includes.h"

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup SPI_FLASH
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
#define CHECK_QUEUE_MAX		200
#define CHECK_ACTION_QUEUE_MAX	100

/* Private define ------------------------------------------------------------*/
u16   ActionTimer;				//动作间隔计时
u16	  CANRxTimer;					//CAN接收数据寄存器
u16	  CAN1HeartbeatTimer;			//CAN心跳计时器
u16	  g_Light1DurationTimer;		//指示灯1持续时长计时器 	CAN接收指示灯持续时长定时器
u16	  g_Light2DurationTimer;		//指示灯2持续时长计时器		CAN发送指示灯持续时长定时器
vu32  TimingDelay;				//延时时间计时
u16   Mos_CheckTimer;			   //MOS管定时检测计时器
u16   Rcop_CheckTimer;			   //过流保护定时检测计时器

u16     Mos_FaultTimer = 200 ;			//MOS管故障上报时长定时器
u16     Rcop_FaultTimer;	    //过流保护故障时长定时器
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */	
void SysTick_Handler(void)
{
	if (TimingDelay != 0)
	{
		TimingDelay--;
	}
	if ((ActionTimer != TIMER_EXPIRED) && (ActionTimer != TIMER_CLOSED))
	{
		ActionTimer--;
	}
	if ((CANRxTimer != TIMER_EXPIRED) && (CANRxTimer != TIMER_CLOSED))
	{
		CANRxTimer--;
	}
	if ((CAN1HeartbeatTimer != TIMER_EXPIRED) && (CAN1HeartbeatTimer != TIMER_CLOSED))
	{
		CAN1HeartbeatTimer--;
	}
	if ((g_Light1DurationTimer != TIMER_EXPIRED) && (g_Light1DurationTimer != TIMER_CLOSED))
	{
		g_Light1DurationTimer--;
	}
	if ((g_Light2DurationTimer != TIMER_EXPIRED) && (g_Light2DurationTimer != TIMER_CLOSED))
	{
		g_Light2DurationTimer--;
	}
	if ((Mos_CheckTimer != TIMER_EXPIRED) && (Mos_CheckTimer != TIMER_CLOSED))
	{
		Mos_CheckTimer--;
	}
	if ((Rcop_CheckTimer != TIMER_EXPIRED) && (Rcop_CheckTimer != TIMER_CLOSED))
	{
		Rcop_CheckTimer--;
	}
	if ((Mos_FaultTimer != TIMER_EXPIRED) && (Mos_FaultTimer != TIMER_CLOSED))
	{
		Mos_FaultTimer--;
	}
	if ((Rcop_FaultTimer != TIMER_EXPIRED) && (Rcop_FaultTimer != TIMER_CLOSED))
	{
		Rcop_FaultTimer--;
	}
}

void USART3_IRQHandler(void)
{

}
/*********************************************************************************
**********************************************************************************/
void SPI3_IRQHandler(void)
{

}
/*********************************************************************************
**********************************************************************************/

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
