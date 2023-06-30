/********************************************************************************
* 文件名称：	CanIap.c
* 作	者：	马如意   
* 当前版本：   	V1.0
* 完成日期：    2015.01.28
* 功能描述: 	完成程序更新功能(传输总线为CAN)。含本架更新、左右邻架、全面更新。本文件隶属于CAN总线处理过程。
* 历史信息：   
*           	版本信息     完成时间      原作者        注释
*
*       >>>>  在工程中的位置  <<<<
*          	  3-应用层
*           √ 2-协议层
*          	  1-硬件驱动层
*********************************************************************************
* Copyright (c) 2014,天津华宁电子有限公司 All rights reserved.
*********************************************************************************/
/********************************************************************************
* .h头文件
*********************************************************************************/
#include "CanIap.h"
#include "iapupdate.h"
/********************************************************************************
* #define宏定义
*********************************************************************************/
#define CANIAP_OS_USE_FLAG	0//是否使用OS系统标志

/********************************************************************************
* 常量定义
*********************************************************************************/
#define DOWNLOAD_PROGRAM_FILTER     		0x1FFFF					//传输程序时的过滤字，马如意，2013.07.22增加
#define TRANS_PROG_TIMES					0x02//每一帧数据发送的次数
/********************************************************************************
* 变量定义
*********************************************************************************/
u32 u32RecvBaseAddr,u32RecvOffsetAddr;//基地址,偏移地址
u32 u32RecvTotalPacketNumb;//总包数
u32 u32NowRecvPacketNumb;//当前包数
u32 u32NowRecvPacketNumbBackup;//当前包数的备份
u8 	u8NowRecvPacketNumbTimes;//当前包数接收的次数
u32 u32RecvProgLength;
u16 u16BackupScId;//备份的本架架号

u16 u16BackupScMinid;//备份的最小架号
u16 u16BackupScMaxid;//备份的最小架号
u8 u8BackupLeftErr;//备份的左邻架通讯异常
u8 u8BackupRightErr;//备份的右邻架通讯异常
u32 u32DevType;//u32DevType在传输程序时为程序类型;

/********************************************************************************
* 函数定义
*********************************************************************************/
/*******************************************************************************************
**函数名称：CanRecvProgProc
**函数作用：收到传输的程序帧时的处理
**函数参数：1)RxFrame，所接收的数据帧；2)u16From,16位整型，收到数据帧的方向(左/右)
**函数输出：无
**注意事项：无
** 作　者:	马如意
** 日　期： 2013.07.18
*******************************************************************************************/
void CanIapInint(void)
{
	//传输程序相关变量赋初值
	u32RecvBaseAddr = 0x00;
	u32RecvOffsetAddr = 0x00;//基地址,偏移地址
	u32RecvTotalPacketNumb = 0x00;//总包数
	u32NowRecvPacketNumb = 0x00;//当前包数
	u32NowRecvPacketNumbBackup = 0x01;//总是从第一包开始收
	u32RecvProgLength = 0x00;
	u32DevType = NONE_DEV_TYPE;;	
	u16BackupScId = 0x00;//备份的本架架号
	u16BackupScMinid = 0x00;//备份的最小架号
	u16BackupScMaxid = 0x00;//备份的最小架号
	u8BackupLeftErr = 0x00;//备份的左邻架通讯异常
	u8BackupRightErr = 0x00;//备份的右邻架通讯异常
}
/*******************************************************************************************
**函数名称：CanIapTimeDlyUs
**函数作用：延时函数(非精确延时，取决于系统时钟)
**函数参数：u32us:延时的时间
**函数输出：无
**注意事项：无
** 作　者:	马如意
** 日　期： 2013.07.18
*******************************************************************************************/
#if (CANIAP_OS_USE_FLAG == 0)//是否使用了操作系统
	u32 CanIapTimeDlyUs(u32 u32us)
	{
		u32 u32i,u32j;

		for (u32i = 0; u32i < u32us; u32i++)
		{
			for(u32j = 0; u32j < 10; u32j++)
			/* Nothing to do */;
		}
		
		return 0x01;
	}
#endif
/*******************************************************************************************
**函数名称：CanRecvProgProc
**函数作用：收到传输的程序帧时的处理
**函数参数：1)RxFrame，所接收的数据帧；2)u16From,16位整型，收到数据帧的方向(左/右)
**函数输出：无
**注意事项：无
** 作　者:	马如意
** 日　期： 2013.07.18
*******************************************************************************************/
//当本设备不为为支架控制器时，进行如下处理
void CanRecvProgProc(CXB_CAN_FRAME_TYPE *ExCan)
{
	u16 u16DstAddr;
	static u16 u16CrcCalculate=0x00,u16Crc=0x00;
	u32 u32Addr=0x00,u32Temp;
	#if CANIAP_OS_USE_FLAG//是否使用了操作系统
		OS_TCB pdata;
	#endif
	
	u16DstAddr = ExCan->u32ID.ID.RID;
	u32NowRecvPacketNumb = ExCan->u32ID.ID.PACKET_NUMB;//20位
	
	if((ExCan->u32ID.ID.TD_FTYPE != CXB_FRAME_DOWNLOAD_PRG_VERSION)&&(u32NowRecvPacketNumb != 0x01))
	{
		if((u32NowRecvPacketNumb - u32NowRecvPacketNumbBackup) > 1)
		{
			u32Temp = 0x01;
			return;		
		}
	}
	
	if(u32NowRecvPacketNumb)//不是第0帧时数据的处理
	{
		if(u32NowRecvPacketNumb == u32NowRecvPacketNumbBackup)//接收到上一帧的第二帧数据
			u8NowRecvPacketNumbTimes++;
		/**当收到版本信息最后一帧，此时接收备份为第一帧，此种情况下无需处理**/
		else if((u8NowRecvPacketNumbTimes == 0x01) && (u32NowRecvPacketNumbBackup == 0x01) && (u32NowRecvPacketNumb == 0x20))
			u8NowRecvPacketNumbTimes++;
		else//接收到新的一帧时，需判断上一帧是否接收了n次
		{
			u8NowRecvPacketNumbTimes = 0x01;//新的一帧收到了一次					

		}
	}	
	
	u32NowRecvPacketNumbBackup = u32NowRecvPacketNumb;
	
	if(u16DstAddr == (THE_DEV_TYPE&0xFF))//为全选架号或与自身架号相等
	{
		if(ExCan->u32ID.ID.TD_FTYPE == CXB_FRAME_DOWNLOAD_PRG_VERSION)//版本信息
		{
			if((u32NowRecvPacketNumb == 0x01)&&(u8NowRecvPacketNumbTimes == 0x01))//第一帧收到第一次时时读取设备类型并擦除所有的块
			{
				u32DevType = (ExCan->u8DT[0x00]|(ExCan->u8DT[0x01]<<8)|(ExCan->u8DT[0x02]<<16)|(ExCan->u8DT[0x03]<<24));
				if((u8IapGetPrgStorageAddr(u32DevType,&u32RecvBaseAddr)  == 0) || (u8IapGetPrgSize(u32DevType,&u32RecvOffsetAddr) == 0))
				{
					u32DevType = 0x00;
					return ;
				}
				__disable_fault_irq();
				FLASH_SetLatency(THE_DEV_FLASH_Latency);
				FLASH_Unlock();	
				/**擦除此部分的所有块**/				
				for(u32Addr = u32RecvBaseAddr; u32Addr <  (u32RecvBaseAddr+u32RecvOffsetAddr); u32Addr += THE_DEV_PRG_SECTOR_SIZE)
					u8IapEraserSector(u32Addr);//擦除块	

				__enable_fault_irq();		
				
				u32RecvOffsetAddr = 0x00;
				u8NowRecvPacketNumbTimes = 0x01;
				if( sizeof(PROGRAM_VERSION_TYPE)%0x08)
					u32Temp = sizeof(PROGRAM_VERSION_TYPE)/0x08+0x01;
				else
					u32Temp = sizeof(PROGRAM_VERSION_TYPE)/0x08;
				u32RecvTotalPacketNumb = u32Temp;
				
			}
			if(u32NowRecvPacketNumb == 0x02)//第二帧时读取程序大小并计算总包数
			{
				u32RecvProgLength =  (ExCan->u8DT[0x04]|(ExCan->u8DT[0x05]<<8));
				u32RecvProgLength |= ((ExCan->u8DT[0x06]<<16)|(ExCan->u8DT[0x07]<<24));
			}
			//仅存储1~4包
			if((u32NowRecvPacketNumb <= 0x04)&&(u8NowRecvPacketNumbTimes == 0x01))
			{
				if(u32NowRecvPacketNumb == 0X04)
					u32NowRecvPacketNumb = 0x04;
				/*判断写入地址是否在程序存储范围内*/
				__disable_fault_irq();
				FLASH_SetLatency(THE_DEV_FLASH_Latency);
				FLASH_Unlock();		
			
				u32Addr = u32RecvBaseAddr+(u32NowRecvPacketNumb-0x01)*0x08;			
				if((u32Addr >= THE_DEV_PRG_STORAGE_BASEADDR) && (u32Addr < MY_DEV_TYPE_ADDRESS))
					u8IapWriteBuf(&(ExCan->u8DT[0x00]), u32Addr,(u8)(ExCan->u16DLC));//数据		

				__enable_fault_irq();
			}
			if(u32NowRecvPacketNumb == u32RecvTotalPacketNumb)//所有的版本信息发送完毕
			{
				u32RecvOffsetAddr = PROG_CODE_OFFSET_ADDRESS;//偏移地址
				u32NowRecvPacketNumb = 0x01;//即将收到的包数
				u32NowRecvPacketNumbBackup = u32NowRecvPacketNumb;
				if( u32RecvProgLength%0x08)
					u32Temp = u32RecvProgLength/0x08+0x01;
				else
					u32Temp = u32RecvProgLength/0x08;
				u32RecvTotalPacketNumb = u32Temp;	
			}			
		}
		else
		{
			/**当收到程序代码的时候***/
			if(u32NowRecvPacketNumb && u32RecvTotalPacketNumb && (u32NowRecvPacketNumb < u32RecvTotalPacketNumb))
			{
				u32Addr = (u32NowRecvPacketNumb-0x01)*0x08;
				u32Addr += u32RecvBaseAddr+PROG_CODE_OFFSET_ADDRESS;
				if((u32Addr >= THE_DEV_PRG_STORAGE_BASEADDR) && (u32Addr < MY_DEV_TYPE_ADDRESS) && (u8NowRecvPacketNumbTimes == 0x01))
				{
					__disable_fault_irq();
					FLASH_SetLatency(THE_DEV_FLASH_Latency);
					FLASH_Unlock();	
					
					u8IapWriteBuf(&(ExCan->u8DT[0x00]), u32Addr, (u8)(ExCan->u16DLC));//程序
		
					__enable_fault_irq();		
				}
			}		
			else//最后一帧数据的处理
			{			
				/*排除非正常进入的条件*/					
				if(!(u32NowRecvPacketNumb&& u32RecvTotalPacketNumb && (u32NowRecvPacketNumb == u32RecvTotalPacketNumb)))
				{			
					return;
				}

				u32Addr = (u32NowRecvPacketNumb-0x01)*0x08;
				u32Addr += u32RecvBaseAddr+PROG_CODE_OFFSET_ADDRESS;
				if((u32Addr >= THE_DEV_PRG_STORAGE_BASEADDR) && (u32Addr < MY_DEV_TYPE_ADDRESS) && (u8NowRecvPacketNumbTimes == 0x01))
				{
					__disable_fault_irq();
					FLASH_SetLatency(THE_DEV_FLASH_Latency);
					FLASH_Unlock();		
					
					u8IapWriteBuf(&(ExCan->u8DT[0x00]), u32Addr, (u8)(ExCan->u16DLC));//程序
	
					__enable_fault_irq();		
				}
				
				#if CANIAP_OS_USE_FLAG//是否使用了操作系统
					OSTimeDly(100/TICK_TIME);//延时100ms
				#else
					CanIapTimeDlyUs(1000);
				#endif
				
				u16CrcCalculate = 0x00;
				for(u32Addr = u32RecvBaseAddr+PROG_CODE_OFFSET_ADDRESS; u32Addr < (u32RecvBaseAddr+PROG_CODE_OFFSET_ADDRESS+(u32NowRecvPacketNumb-0x01)*0x08+(u8)(ExCan->u16DLC));u32Addr+= 0x20000)
				{
					if(((u32RecvBaseAddr+PROG_CODE_OFFSET_ADDRESS+(u32NowRecvPacketNumb-0x01)*0x08+(u8)(ExCan->u16DLC))-u32Addr) >= 0x20000)
						u16CrcCalculate = u16IapExFlashCrc(u32Addr,0x20000,&u16CrcCalculate);//临时屏蔽	
					else
						u16CrcCalculate = u16IapExFlashCrc(u32Addr,(u32RecvBaseAddr+PROG_CODE_OFFSET_ADDRESS+(u32NowRecvPacketNumb-0x01)*0x08+(u8)(ExCan->u16DLC)-u32Addr),&u16CrcCalculate);//临时屏蔽
					#if CANIAP_OS_USE_FLAG//是否使用了操作系统
						OSTimeDly(100/TICK_TIME);//延时1500ms，以便调度看门狗任务
					#else
						CanIapTimeDlyUs(1000);
					#endif
				}

				u16Crc = 0x00;
				u8IapReadBuf((u8 *)&u16Crc,u32RecvBaseAddr+PROG_CRCL_OFFSET_ADDRESS,0x02);
				if((u16CrcCalculate == u16Crc))//校验成功
				{
					u16CrcCalculate = 0x00;
					u16CrcCalculate = u16IapExFlashCrc(u32RecvBaseAddr+PROG_DEVTYPE_OFFSET_ADDRESS,DEV_PROGRAM_VERSION_SIZE-0x04,&u16CrcCalculate);//计算CRC校验
					u32Temp = (u32)u16CrcCalculate;
					/*判断写入地址是否在程序存储范围内*/
					u32Addr = u32RecvBaseAddr+PROG_VER_CRCL_OFFSET_ADDRESS;			
					if((u32Addr >= THE_DEV_PRG_STORAGE_BASEADDR) && (u32Addr < MY_DEV_TYPE_ADDRESS) && (u8NowRecvPacketNumbTimes == 0x01))	
					{		
						__disable_fault_irq();
						FLASH_SetLatency(THE_DEV_FLASH_Latency);
						FLASH_Unlock();	
						u8IapWriteBuf((u8 *)&u32Temp, u32RecvBaseAddr+PROG_VER_CRCL_OFFSET_ADDRESS,0x04);
						FLASH_Lock();	
						__enable_fault_irq();	
					}
					#if (THE_DEV_TYPE == SC_DEV_TYPE)
						#ifndef DYK_SS
							if (u32DevType == SC_DEV_TYPE)
						#else
							if(u32DevType == SS_DEV_TYPE)
						#endif
					#else
						if(u32DevType ==THE_DEV_TYPE)//设备类型相符，写入相关标识，立即更新
					#endif				
					{
						__disable_fault_irq();
						FLASH_SetLatency(THE_DEV_FLASH_Latency);
						FLASH_Unlock();
						
						//写入设备类型字
						u8IapEraserSector(MY_DEV_TYPE_ADDRESS);//擦除一个扇区
						u8IapWriteBuf((u8 *)&u32DevType, MY_DEV_TYPE_ADDRESS, 0x04);
						/* 程序启动首地址*/
						u8IapReadBuf((u8 *)&u32Temp,  u32RecvBaseAddr+PROG_WRITE_BASE_ADDRESS, 0x04);
						u8IapWriteBuf((u8 *)&u32Temp, APP_WRITE_BASEADDRESS, 0x04);//写入启动首地址	

						u32Temp = RROG_DOWNLOAD_FLAG;//写入下载完成标志
						/*判断写入地址是否在程序存储范围内*/
						u32Addr = APP_DOWNLOAD_OFFSET_ADDRESS;			
						if((u32Addr >= MY_DEV_TYPE_ADDRESS) && (u32Addr < (APP_NOT_DWNL_UPDATE_ADDRESS+0x04)))					
							u8IapWriteBuf((u8 *)&u32Temp, u32Addr,0x04);

						u32Temp = RROG_UPDATE_FLAG;//写入立即更新标志
						/*判断写入地址是否在程序存储范围内*/
						u32Addr = APP_UPDATE_OFFSET_ADDRESS;			
						if((u32Addr >= MY_DEV_TYPE_ADDRESS) && (u32Addr < (APP_NOT_DWNL_UPDATE_ADDRESS+0x04)))						
							u8IapWriteBuf((u8 *)&u32Temp, u32Addr,0x04);
						
						FLASH_Lock();	
						__enable_fault_irq();
						
						GPIO_ResetBits(GPIOC,GPIO_Pin_6);   //点亮红灯，指示程序接收完成
						
						#if CANIAP_OS_USE_FLAG//是否使用了操作系统
							OSTimeDly(100/TICK_TIME);//延时100ms，以便将数据顺利发出
						#else
							CanIapTimeDlyUs(1000);						
						#endif
						
						//执行跳转
						vIapJumpToBoot(IN_FLASH_BOOTLOADER_ADDR);
					}
				}
				else //校验不正确
				{
					__disable_fault_irq();
					FLASH_Lock();	
					__enable_fault_irq();
					u32DevType = NONE_DEV_TYPE;
					u32RecvProgLength = 0x00;
					u32NowRecvPacketNumb = 0x00;
					u32RecvTotalPacketNumb = 0x00;
					u32RecvOffsetAddr = 0x00;
					u32RecvBaseAddr = 0x00;
				}
				
				#if CANIAP_OS_USE_FLAG//是否使用了操作系统
					/**恢复不必要的任务**/
					OSTaskQuery(ANGLE_SENSOR_TASK_PRIO, &pdata);			//查询处理任务是否挂起
					if(pdata.OSTCBStat == OS_STAT_SUSPEND)
					{
						OSTaskResume(ANGLE_SENSOR_TASK_PRIO);				//唤醒任务
					}	
					
					/**恢复不必要的任务**/
					OSTaskQuery(ANGLE_SENSOR_TASK_PRIO, &pdata);			//查询处理任务是否挂起
					if(pdata.OSTCBStat == OS_STAT_SUSPEND)
					{
						OSTaskResume(ANGLE_SENSOR_TASK_PRIO);				//唤醒任务
					}	
					
					/**恢复不必要的任务**/
					OSTaskQuery(CANLEFT_TASK_PRIO, &pdata);			//查询处理任务是否挂起
					if(pdata.OSTCBStat == OS_STAT_SUSPEND)
					{
						OSTaskResume(CANLEFT_TASK_PRIO);				//唤醒任务
					}	

					/**恢复不必要的任务**/
					OSTaskQuery(CANRIGHT_TASK_PRIO, &pdata);			//查询处理任务是否挂起
					if(pdata.OSTCBStat == OS_STAT_SUSPEND)
					{
						OSTaskResume(CANRIGHT_TASK_PRIO);				//唤醒任务
					}
				#endif
			 }
		}
	}			
}


