/********************************************************************************
* �ļ����ƣ�	CanIap.c
* ��	�ߣ�	������   
* ��ǰ�汾��   	V1.0
* ������ڣ�    2015.01.28
* ��������: 	��ɳ�����¹���(��������ΪCAN)�������ܸ��¡������ڼܡ�ȫ����¡����ļ�������CAN���ߴ�����̡�
* ��ʷ��Ϣ��   
*           	�汾��Ϣ     ���ʱ��      ԭ����        ע��
*
*       >>>>  �ڹ����е�λ��  <<<<
*          	  3-Ӧ�ò�
*           �� 2-Э���
*          	  1-Ӳ��������
*********************************************************************************
* Copyright (c) 2014,������������޹�˾ All rights reserved.
*********************************************************************************/
/********************************************************************************
* .hͷ�ļ�
*********************************************************************************/
#include "CanIap.h"
#include "iapupdate.h"
/********************************************************************************
* #define�궨��
*********************************************************************************/
#define CANIAP_OS_USE_FLAG	0//�Ƿ�ʹ��OSϵͳ��־

/********************************************************************************
* ��������
*********************************************************************************/
#define DOWNLOAD_PROGRAM_FILTER     		0x1FFFF					//�������ʱ�Ĺ����֣������⣬2013.07.22����
#define TRANS_PROG_TIMES					0x02//ÿһ֡���ݷ��͵Ĵ���
/********************************************************************************
* ��������
*********************************************************************************/
u32 u32RecvBaseAddr,u32RecvOffsetAddr;//����ַ,ƫ�Ƶ�ַ
u32 u32RecvTotalPacketNumb;//�ܰ���
u32 u32NowRecvPacketNumb;//��ǰ����
u32 u32NowRecvPacketNumbBackup;//��ǰ�����ı���
u8 	u8NowRecvPacketNumbTimes;//��ǰ�������յĴ���
u32 u32RecvProgLength;
u16 u16BackupScId;//���ݵı��ܼܺ�

u16 u16BackupScMinid;//���ݵ���С�ܺ�
u16 u16BackupScMaxid;//���ݵ���С�ܺ�
u8 u8BackupLeftErr;//���ݵ����ڼ�ͨѶ�쳣
u8 u8BackupRightErr;//���ݵ����ڼ�ͨѶ�쳣
u32 u32DevType;//u32DevType�ڴ������ʱΪ��������;

/********************************************************************************
* ��������
*********************************************************************************/
/*******************************************************************************************
**�������ƣ�CanRecvProgProc
**�������ã��յ�����ĳ���֡ʱ�Ĵ���
**����������1)RxFrame�������յ�����֡��2)u16From,16λ���ͣ��յ�����֡�ķ���(��/��)
**�����������
**ע�������
** ������:	������
** �ա��ڣ� 2013.07.18
*******************************************************************************************/
void CanIapInint(void)
{
	//���������ر�������ֵ
	u32RecvBaseAddr = 0x00;
	u32RecvOffsetAddr = 0x00;//����ַ,ƫ�Ƶ�ַ
	u32RecvTotalPacketNumb = 0x00;//�ܰ���
	u32NowRecvPacketNumb = 0x00;//��ǰ����
	u32NowRecvPacketNumbBackup = 0x01;//���Ǵӵ�һ����ʼ��
	u32RecvProgLength = 0x00;
	u32DevType = NONE_DEV_TYPE;;	
	u16BackupScId = 0x00;//���ݵı��ܼܺ�
	u16BackupScMinid = 0x00;//���ݵ���С�ܺ�
	u16BackupScMaxid = 0x00;//���ݵ���С�ܺ�
	u8BackupLeftErr = 0x00;//���ݵ����ڼ�ͨѶ�쳣
	u8BackupRightErr = 0x00;//���ݵ����ڼ�ͨѶ�쳣
}
/*******************************************************************************************
**�������ƣ�CanIapTimeDlyUs
**�������ã���ʱ����(�Ǿ�ȷ��ʱ��ȡ����ϵͳʱ��)
**����������u32us:��ʱ��ʱ��
**�����������
**ע�������
** ������:	������
** �ա��ڣ� 2013.07.18
*******************************************************************************************/
#if (CANIAP_OS_USE_FLAG == 0)//�Ƿ�ʹ���˲���ϵͳ
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
**�������ƣ�CanRecvProgProc
**�������ã��յ�����ĳ���֡ʱ�Ĵ���
**����������1)RxFrame�������յ�����֡��2)u16From,16λ���ͣ��յ�����֡�ķ���(��/��)
**�����������
**ע�������
** ������:	������
** �ա��ڣ� 2013.07.18
*******************************************************************************************/
//�����豸��ΪΪ֧�ܿ�����ʱ���������´���
void CanRecvProgProc(CXB_CAN_FRAME_TYPE *ExCan)
{
	u16 u16DstAddr;
	static u16 u16CrcCalculate=0x00,u16Crc=0x00;
	u32 u32Addr=0x00,u32Temp;
	#if CANIAP_OS_USE_FLAG//�Ƿ�ʹ���˲���ϵͳ
		OS_TCB pdata;
	#endif
	
	u16DstAddr = ExCan->u32ID.ID.RID;
	u32NowRecvPacketNumb = ExCan->u32ID.ID.PACKET_NUMB;//20λ
	
	if((ExCan->u32ID.ID.TD_FTYPE != CXB_FRAME_DOWNLOAD_PRG_VERSION)&&(u32NowRecvPacketNumb != 0x01))
	{
		if((u32NowRecvPacketNumb - u32NowRecvPacketNumbBackup) > 1)
		{
			u32Temp = 0x01;
			return;		
		}
	}
	
	if(u32NowRecvPacketNumb)//���ǵ�0֡ʱ���ݵĴ���
	{
		if(u32NowRecvPacketNumb == u32NowRecvPacketNumbBackup)//���յ���һ֡�ĵڶ�֡����
			u8NowRecvPacketNumbTimes++;
		/**���յ��汾��Ϣ���һ֡����ʱ���ձ���Ϊ��һ֡��������������账��**/
		else if((u8NowRecvPacketNumbTimes == 0x01) && (u32NowRecvPacketNumbBackup == 0x01) && (u32NowRecvPacketNumb == 0x20))
			u8NowRecvPacketNumbTimes++;
		else//���յ��µ�һ֡ʱ�����ж���һ֡�Ƿ������n��
		{
			u8NowRecvPacketNumbTimes = 0x01;//�µ�һ֡�յ���һ��					

		}
	}	
	
	u32NowRecvPacketNumbBackup = u32NowRecvPacketNumb;
	
	if(u16DstAddr == (THE_DEV_TYPE&0xFF))//Ϊȫѡ�ܺŻ�������ܺ����
	{
		if(ExCan->u32ID.ID.TD_FTYPE == CXB_FRAME_DOWNLOAD_PRG_VERSION)//�汾��Ϣ
		{
			if((u32NowRecvPacketNumb == 0x01)&&(u8NowRecvPacketNumbTimes == 0x01))//��һ֡�յ���һ��ʱʱ��ȡ�豸���Ͳ��������еĿ�
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
				/**�����˲��ֵ����п�**/				
				for(u32Addr = u32RecvBaseAddr; u32Addr <  (u32RecvBaseAddr+u32RecvOffsetAddr); u32Addr += THE_DEV_PRG_SECTOR_SIZE)
					u8IapEraserSector(u32Addr);//������	

				__enable_fault_irq();		
				
				u32RecvOffsetAddr = 0x00;
				u8NowRecvPacketNumbTimes = 0x01;
				if( sizeof(PROGRAM_VERSION_TYPE)%0x08)
					u32Temp = sizeof(PROGRAM_VERSION_TYPE)/0x08+0x01;
				else
					u32Temp = sizeof(PROGRAM_VERSION_TYPE)/0x08;
				u32RecvTotalPacketNumb = u32Temp;
				
			}
			if(u32NowRecvPacketNumb == 0x02)//�ڶ�֡ʱ��ȡ�����С�������ܰ���
			{
				u32RecvProgLength =  (ExCan->u8DT[0x04]|(ExCan->u8DT[0x05]<<8));
				u32RecvProgLength |= ((ExCan->u8DT[0x06]<<16)|(ExCan->u8DT[0x07]<<24));
			}
			//���洢1~4��
			if((u32NowRecvPacketNumb <= 0x04)&&(u8NowRecvPacketNumbTimes == 0x01))
			{
				if(u32NowRecvPacketNumb == 0X04)
					u32NowRecvPacketNumb = 0x04;
				/*�ж�д���ַ�Ƿ��ڳ���洢��Χ��*/
				__disable_fault_irq();
				FLASH_SetLatency(THE_DEV_FLASH_Latency);
				FLASH_Unlock();		
			
				u32Addr = u32RecvBaseAddr+(u32NowRecvPacketNumb-0x01)*0x08;			
				if((u32Addr >= THE_DEV_PRG_STORAGE_BASEADDR) && (u32Addr < MY_DEV_TYPE_ADDRESS))
					u8IapWriteBuf(&(ExCan->u8DT[0x00]), u32Addr,(u8)(ExCan->u16DLC));//����		

				__enable_fault_irq();
			}
			if(u32NowRecvPacketNumb == u32RecvTotalPacketNumb)//���еİ汾��Ϣ�������
			{
				u32RecvOffsetAddr = PROG_CODE_OFFSET_ADDRESS;//ƫ�Ƶ�ַ
				u32NowRecvPacketNumb = 0x01;//�����յ��İ���
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
			/**���յ���������ʱ��***/
			if(u32NowRecvPacketNumb && u32RecvTotalPacketNumb && (u32NowRecvPacketNumb < u32RecvTotalPacketNumb))
			{
				u32Addr = (u32NowRecvPacketNumb-0x01)*0x08;
				u32Addr += u32RecvBaseAddr+PROG_CODE_OFFSET_ADDRESS;
				if((u32Addr >= THE_DEV_PRG_STORAGE_BASEADDR) && (u32Addr < MY_DEV_TYPE_ADDRESS) && (u8NowRecvPacketNumbTimes == 0x01))
				{
					__disable_fault_irq();
					FLASH_SetLatency(THE_DEV_FLASH_Latency);
					FLASH_Unlock();	
					
					u8IapWriteBuf(&(ExCan->u8DT[0x00]), u32Addr, (u8)(ExCan->u16DLC));//����
		
					__enable_fault_irq();		
				}
			}		
			else//���һ֡���ݵĴ���
			{			
				/*�ų����������������*/					
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
					
					u8IapWriteBuf(&(ExCan->u8DT[0x00]), u32Addr, (u8)(ExCan->u16DLC));//����
	
					__enable_fault_irq();		
				}
				
				#if CANIAP_OS_USE_FLAG//�Ƿ�ʹ���˲���ϵͳ
					OSTimeDly(100/TICK_TIME);//��ʱ100ms
				#else
					CanIapTimeDlyUs(1000);
				#endif
				
				u16CrcCalculate = 0x00;
				for(u32Addr = u32RecvBaseAddr+PROG_CODE_OFFSET_ADDRESS; u32Addr < (u32RecvBaseAddr+PROG_CODE_OFFSET_ADDRESS+(u32NowRecvPacketNumb-0x01)*0x08+(u8)(ExCan->u16DLC));u32Addr+= 0x20000)
				{
					if(((u32RecvBaseAddr+PROG_CODE_OFFSET_ADDRESS+(u32NowRecvPacketNumb-0x01)*0x08+(u8)(ExCan->u16DLC))-u32Addr) >= 0x20000)
						u16CrcCalculate = u16IapExFlashCrc(u32Addr,0x20000,&u16CrcCalculate);//��ʱ����	
					else
						u16CrcCalculate = u16IapExFlashCrc(u32Addr,(u32RecvBaseAddr+PROG_CODE_OFFSET_ADDRESS+(u32NowRecvPacketNumb-0x01)*0x08+(u8)(ExCan->u16DLC)-u32Addr),&u16CrcCalculate);//��ʱ����
					#if CANIAP_OS_USE_FLAG//�Ƿ�ʹ���˲���ϵͳ
						OSTimeDly(100/TICK_TIME);//��ʱ1500ms���Ա���ȿ��Ź�����
					#else
						CanIapTimeDlyUs(1000);
					#endif
				}

				u16Crc = 0x00;
				u8IapReadBuf((u8 *)&u16Crc,u32RecvBaseAddr+PROG_CRCL_OFFSET_ADDRESS,0x02);
				if((u16CrcCalculate == u16Crc))//У��ɹ�
				{
					u16CrcCalculate = 0x00;
					u16CrcCalculate = u16IapExFlashCrc(u32RecvBaseAddr+PROG_DEVTYPE_OFFSET_ADDRESS,DEV_PROGRAM_VERSION_SIZE-0x04,&u16CrcCalculate);//����CRCУ��
					u32Temp = (u32)u16CrcCalculate;
					/*�ж�д���ַ�Ƿ��ڳ���洢��Χ��*/
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
						if(u32DevType ==THE_DEV_TYPE)//�豸���������д����ر�ʶ����������
					#endif				
					{
						__disable_fault_irq();
						FLASH_SetLatency(THE_DEV_FLASH_Latency);
						FLASH_Unlock();
						
						//д���豸������
						u8IapEraserSector(MY_DEV_TYPE_ADDRESS);//����һ������
						u8IapWriteBuf((u8 *)&u32DevType, MY_DEV_TYPE_ADDRESS, 0x04);
						/* ���������׵�ַ*/
						u8IapReadBuf((u8 *)&u32Temp,  u32RecvBaseAddr+PROG_WRITE_BASE_ADDRESS, 0x04);
						u8IapWriteBuf((u8 *)&u32Temp, APP_WRITE_BASEADDRESS, 0x04);//д�������׵�ַ	

						u32Temp = RROG_DOWNLOAD_FLAG;//д��������ɱ�־
						/*�ж�д���ַ�Ƿ��ڳ���洢��Χ��*/
						u32Addr = APP_DOWNLOAD_OFFSET_ADDRESS;			
						if((u32Addr >= MY_DEV_TYPE_ADDRESS) && (u32Addr < (APP_NOT_DWNL_UPDATE_ADDRESS+0x04)))					
							u8IapWriteBuf((u8 *)&u32Temp, u32Addr,0x04);

						u32Temp = RROG_UPDATE_FLAG;//д���������±�־
						/*�ж�д���ַ�Ƿ��ڳ���洢��Χ��*/
						u32Addr = APP_UPDATE_OFFSET_ADDRESS;			
						if((u32Addr >= MY_DEV_TYPE_ADDRESS) && (u32Addr < (APP_NOT_DWNL_UPDATE_ADDRESS+0x04)))						
							u8IapWriteBuf((u8 *)&u32Temp, u32Addr,0x04);
						
						FLASH_Lock();	
						__enable_fault_irq();
						
						GPIO_ResetBits(GPIOC,GPIO_Pin_6);   //������ƣ�ָʾ����������
						
						#if CANIAP_OS_USE_FLAG//�Ƿ�ʹ���˲���ϵͳ
							OSTimeDly(100/TICK_TIME);//��ʱ100ms���Ա㽫����˳������
						#else
							CanIapTimeDlyUs(1000);						
						#endif
						
						//ִ����ת
						vIapJumpToBoot(IN_FLASH_BOOTLOADER_ADDR);
					}
				}
				else //У�鲻��ȷ
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
				
				#if CANIAP_OS_USE_FLAG//�Ƿ�ʹ���˲���ϵͳ
					/**�ָ�����Ҫ������**/
					OSTaskQuery(ANGLE_SENSOR_TASK_PRIO, &pdata);			//��ѯ���������Ƿ����
					if(pdata.OSTCBStat == OS_STAT_SUSPEND)
					{
						OSTaskResume(ANGLE_SENSOR_TASK_PRIO);				//��������
					}	
					
					/**�ָ�����Ҫ������**/
					OSTaskQuery(ANGLE_SENSOR_TASK_PRIO, &pdata);			//��ѯ���������Ƿ����
					if(pdata.OSTCBStat == OS_STAT_SUSPEND)
					{
						OSTaskResume(ANGLE_SENSOR_TASK_PRIO);				//��������
					}	
					
					/**�ָ�����Ҫ������**/
					OSTaskQuery(CANLEFT_TASK_PRIO, &pdata);			//��ѯ���������Ƿ����
					if(pdata.OSTCBStat == OS_STAT_SUSPEND)
					{
						OSTaskResume(CANLEFT_TASK_PRIO);				//��������
					}	

					/**�ָ�����Ҫ������**/
					OSTaskQuery(CANRIGHT_TASK_PRIO, &pdata);			//��ѯ���������Ƿ����
					if(pdata.OSTCBStat == OS_STAT_SUSPEND)
					{
						OSTaskResume(CANRIGHT_TASK_PRIO);				//��������
					}
				#endif
			 }
		}
	}			
}


