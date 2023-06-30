/********************************************************************************
* �ļ����ƣ�	iapupdate.h
* ��	�ߣ�	������   
* ��ǰ�汾��   	V1.0
* ������ڣ�    2014.02.08
* ��������: 	����iapupdate.hͷ�ļ�			
* ��ʷ��Ϣ��   
*           	�汾��Ϣ     ���ʱ��      ԭ����        ע��
*
*       >>>>  �ڹ����е�λ��  <<<<
*          	  3-Ӧ�ò�
*           �� 2-Э���
*             1-Ӳ��������
*********************************************************************************
* Copyright (c) 2014,������������޹�˾ All rights reserved.
*********************************************************************************/
#ifndef __IAPUPDATE_H__
#define __IAPUPDATE_H__
/********************************************************************************
* .hͷ�ļ�
*********************************************************************************/
#include "stm32f10x.h"

/********************************************************************************
* #define�궨�弰��������
*********************************************************************************/
/**ʹ�õ�Ӳ��ƽ̨����****/
#define	BSP_PLATFORM_M3			0x01
#define	BSP_PLATFORM_M4			0x02
#define BSP_PLATFORM			BSP_PLATFORM_M3//��������֮һ��������Ч

/**�Ƿ�ʹ�ò���ϵͳ����***/
#define	BSP_OS_USE				0x01
#define BSP_OS_NO_USE			0x02
#define BSP_OS_FLAG				BSP_OS_NO_USE//��������֮һ��������Ч

/**�Ƿ�ʹ��LCD��ʾ***/
#define	BSP_LCD_USE				0x01
#define BSP_LCD_NO_USE			0x02
#define BSP_LCD_FLAG			BSP_LCD_NO_USE//��������֮һ��������Ч

/**�Ƿ�ʹ��BEEP***/
#define	BSP_BEEP_USE			0x01
#define BSP_BEEP_NO_USE			0x02
#define BSP_BEEP_FLAG			BSP_BEEP_NO_USE//��������֮һ��������Ч

/**�Ƿ�ʹ������FLASH��־***//**���Ƕȴ�����������FLASH****/
#define	BSP_PRG_STORAGE_EXFLASH_USE		0x01//ʹ������FLASH
#define	BSP_PRG_STORAGE_INFLASH_USE		0x02//ʹ��CPU�ڲ�FLASH
#define BSP_PRG_STORAGE_FLASH_FLAG		BSP_PRG_STORAGE_INFLASH_USE//��������֮һ��������Ч

/**���Ա�־****/
#define	DEBUG_MODEL_ENABLE		0x01
#define	DEBUG_MODEL_DISABLE		0x02
#define DEBUG_MODEL				DEBUG_MODEL_DISABLE//��������֮һ��������Ч

//ͨѶЭ���У��汾��Ϣ�Ľṹ�嶨�塣�ܹ�ռ��256���ֽڣ�64���֡����ֽ���ǰ�����ֽ��ں�
typedef struct
{
	u32	u32PrgDevType;//����汾���豸����
	u32 u32TargetBoardType;//Ŀ�������
	u32 u32PrgVer;//Ӧ�ó���İ汾��
	u32 u32PrgSize;//Ӧ�ó���Ĵ�С
	u32	u32PrgDate;//Ӧ�ó�����������
	u32	u32PrgEncryptType;//Ӧ�ó���ļ����㷨��0x00Ϊ�޼���	
	u32 u32PrgWriteBaseaddr;//����д��Ļ���ַ
	u32 u32PrgCrc16;//Ӧ�ó����CRCУ��ֵ
	u32	reserved[55];//Ԥ����Ϣ��52����
	u32 u32VerInfCrc16;//�����汾��Ϣ�����CRCУ�飬ΪCRC16��ֻռ�����е������ֽ�
}PROGRAM_VERSION_TYPE;

/**��Ӧ���豸��������Ӧ������FLASH����洢��ַ**/
typedef struct
{
	u32 u32DevType;
	u32 u32NorPrgBaseAddr;
}PRG_STORAGE_BASEADDR_TYPE;

/********************************************************************************
* ��������
*********************************************************************************/
/*���´���ǰ��Ҫ�趨�õ�ַ*/
/*�ݶ�bootloaderռ��16k�ڴ棬Ӧ�ó����0x08004000��ʼ��0x0804000*/
#define IN_FLASH_BOOTLOADER_ADDR		0x08000000//STM32������������ʼ��ַ
#define IN_FLASH_APP_ADDR				0x08004000//STM32��Ӧ�ó�����ʼ��ַ


#if ((BSP_PRG_STORAGE_FLASH_FLAG	==	BSP_PRG_STORAGE_INFLASH_USE))
	#define IN_FLASH_ADDRESS_MAX			0x080117FF//STM32��Ӧ�ó�����flash����ߵ�ַ
#elif (BSP_PRG_STORAGE_FLASH_FLAG	==	BSP_PRG_STORAGE_EXFLASH_USE)
	#error "The EMVD Module has no exflash"
#endif

//ƫ������ַ
#ifdef	BOOTLOADER_ENABLED
#define FLASH_BASE            ((uint32_t)0x08004000) /*!< FLASH base address in the alias region */
#else
#define FLASH_BASE            ((uint32_t)0x08000000) /*!< FLASH base address in the alias region */
#endif

#define IN_CODE_FLASH_SIZE_MAX			(IN_FLASH_ADDRESS_MAX - IN_FLASH_APP_ADDR+0x01)//STM32�п������ɵ���������																									
#define VTOR_OFFSET						(IN_FLASH_APP_ADDR - 0x08000000)//Ӧ�ó����ж�������ƫ�Ƶ�ַ
//iapLoadCode�ڻ�������С													
#define LOAD_CODE_TEMPBUF_MAX_LEN		32//��ֵ���Ϊ65535�����ұ�����4�ı���

//SPI FLASH�еĵ�ַ��ƫ�Ƶ�ַ����
#if ((BSP_PRG_STORAGE_FLASH_FLAG	==	BSP_PRG_STORAGE_INFLASH_USE))
	#define EMVD_DEV_PROGRAM_BASE_ADDRESS	0x08011800//emvdģ��ĳ����Ż���ַ
	
	#define MY_DEV_TYPE_ADDRESS          	0x0801F000//�����豸���ʹ�ŵĻ���ַ
	#define APP_WRITE_BASEADDRESS        	0x0801F004//Ӧ�ó���д��Ļ���ַ//�����⣬2013.08.26����	
	#define APP_DOWNLOAD_OFFSET_ADDRESS  	0x0801F008//�汾��Ϣ�г���������ɱ�־�ı����ַ
	#define APP_UPDATE_OFFSET_ADDRESS	   	0x0801F00C//�汾��Ϣ�г����������±�־�ı����ַ
	#define	APP_PRGUPDATE_SUCCEED_ADDRESS	0x0801F010//�汾��Ϣ�г�����³ɹ���־�Ĵ���	
	#define APP_DWNL_NOT_UPDATE_ADDRESS		0x0801F014//���س�����ɣ�δ���и��±�־�����˱�־λΪ��ʱ������������Խ��г���ı��ݡ�01H��δ���£�00H(FF������)���Ѿ������˸���
	#define APP_NOT_DWNL_UPDATE_ADDRESS		0x0801F018//δ���س��򣬵�ѡ���������±��ء�01H��δ���أ�00H(FF������)���Ѿ������˸���
#elif (BSP_PRG_STORAGE_FLASH_FLAG	== BSP_PRG_STORAGE_EXFLASH_USE)
	#error "The EMVD Module has no exflash"
#endif

/**SPI FLAS�г����С����***/
#if ((BSP_PRG_STORAGE_FLASH_FLAG	==	BSP_PRG_STORAGE_INFLASH_USE))
	#define EMVD_DEV_PROGRAM_SIZE			0xD700//emvdģ��ĳ����С(���汾��Ϣ)��53.75K
#elif (BSP_PRG_STORAGE_FLASH_FLAG	== BSP_PRG_STORAGE_EXFLASH_USE)
	#error "The EMVD Module has no exflash"
#endif

/**SPI FLAS�汾��Ϣ��С����***/
#define DEV_PROGRAM_VERSION_SIZE		0x00100//���������豸���汾��Ϣ���ܳ��ȣ�256�ֽ�

//��ų���ʱ��ص�ƫ�Ƶ�ַ����
#define PROG_DEVTYPE_OFFSET_ADDRESS		0x0000//�汾��Ϣ���豸���͵�ƫ�Ƶ�ַ
#define PROG_TARGETYPE_OFFSET_ADDRESS	0x0004//�汾��Ϣ��Ŀ������͵�ƫ�Ƶ�ַ
#define PROG_VERSION_OFFSET_ADDRESS		0x0008//�汾��Ϣ�а汾�ŵ�ƫ�Ƶ�ַ
#define PROG_LENGTH_OFFSET_ADDRESS		0x000C//�汾��Ϣ�г����С��ƫ�Ƶ�ַ

#define PROG_DATE_OFFSET_ADDRESS		0x0010//�汾��Ϣ�г���������ڵ�ƫ�Ƶ�ַ

#define PROG_WRITE_BASE_ADDRESS	   		0x0018//�汾��Ϣ�г���д���׵�ַ��ƫ��
#define PROG_CRCL_OFFSET_ADDRESS	   	0x001C//�汾��Ϣ�г���CRCУ��ĵ��ֽ�
#define PROG_CRCH_OFFSET_ADDRESS	   	0x001D//�汾��Ϣ�г���CRCУУ��ĸ��ֽ�

#define PROG_MESS_OFFSET_ADDRESS	   	0x0020//�汾��Ϣ�г����޶����ݵ�ƫ�Ƶ�ַ

#define PROG_VER_CRCL_OFFSET_ADDRESS	0x00FC//�汾��Ϣ�����ֶ�CRCУ����ֽڵı����ַ
#define PROG_VER_CRCH_OFFSET_ADDRESS	0x00FD//�汾��Ϣ�����ֶ�CRCУ����ֽڵı����ַ

#define PROG_CODE_OFFSET_ADDRESS	  	0x00100//��������ƫ�Ƶ�ַ

//һЩ��־
#define RROG_DOWNLOAD_FLAG				(0x01)//�����Ѿ�������ɱ�־
#define RROG_UPDATE_FLAG				(0x01)//������Ҫ�������±�־
#define PROG_NOT_DOWNLOAD_UPDATE_FLAG	(0x01)//δ�������س�����Ҫ�������±�־

#define PROG_UPDATE_SUCCEED_FLAG		(0x01)//������³ɹ���־
#define PROG_UPDATE_FAIL_FLAG			(0x02)//�������ʧ�ܱ�־

/**ͨѶЭ������������豸����****/
#define NONE_DEV_TYPE					(0x00000000)//�豸������Ч
#define SC_DEV_TYPE						(0x00100A00)//֧�ܿ�����
#define EMVD_DEV_TYPE					(0x00100A01)//��ŷ�
#define HUB_DEV_TYPE					(0x00100A02)//HUB
#define WL_DEV_TYPE 					(0x00100A03)//����ģ��
#define CXB_DEV_TYPE					(0x00100A04)//�����
#define ANGLE_DEV_TYPE					(0x00100A05)//�Ƕ�
#define SS_DEV_TYPE						(0x00100A06)//֧�ܷ�����
#define OUHEQI_DEV_TYPE 				(0x00100A07)//�����
#define DYK_DEV_TYPE_MAX				(0x00100A1F)//�豸���͵����ֵ


/**Ŀ����豸���Ͷ���****/
#define TAGET_51_MCU					(0x00000001)
#define TAGET_PIC_MCU					(0x00000002)
#define TAGET_STM32F1_MCU				(0x00000003)
#define TAGET_STM32F2_MCU				(0x00000004)
#define TAGET_STM32F4_MCU				(0x00000005)

/**��Ӧ�����������豸����Ӧ��д���ַ�Լ��ռ��С�ȵĶ���****/
#define THE_DEV_TYPE					EMVD_DEV_TYPE
#define THE_TARGET_TYPE					TAGET_STM32F1_MCU//�豸��Ŀ�������
#define	THE_DEV_PRG_STORAGE_BASEADDR	EMVD_DEV_PROGRAM_BASE_ADDRESS
#define	THE_DEV_PRG_STORAGE_SIZE		EMVD_DEV_PROGRAM_SIZE

#if (BSP_PRG_STORAGE_FLASH_FLAG	== BSP_PRG_STORAGE_EXFLASH_USE)
	#error "The EMVD Module has no exflash"//�������塣����ʱ��==�ⲿ������С���ڲ�ʱ��==�ڲ�ҳ��С
#elif (BSP_PRG_STORAGE_FLASH_FLAG	==	BSP_PRG_STORAGE_INFLASH_USE)
	#define THE_DEV_PRG_SECTOR_SIZE		0x400//�������塣����ʱ��==�ⲿ������С���ڲ�ʱ��==�ڲ�ҳ��С
#endif

#define THE_DEV_PRG_BULK_SIZE			0x8000//�鶨�塣64K

#define THE_PRG_STORAGE_BASEADDRESS_MAX	MY_DEV_TYPE_ADDRESS/**�豸����洢������ַ��������ֵ���ж���Ч***/

/**��ʹ�õľ���Ƶ�ʶ�Ӧ��Latency***/
#define	THE_DEV_FLASH_Latency			FLASH_Latency_2//1�ȴ����ڣ��� 24MHz < SYSCLK �� 48MHz 
#define	THE_DEV_CPU_VOLTAGE				VoltageRange_3/*!<Device operating range: 2.7V to 3.6V */

/********************************************************************************
* ȫ�ֱ�������
*********************************************************************************/

/********************************************************************************
* ��������
*********************************************************************************/
u8 u8IapGetPrgStorageAddr(u32 u32DevType,u32 *pStorageAddr);
u8 u8IapGetPrgSize(u32 u32DevType,u32 *pSize);
u16 u16InFlashCrc(u32 u32SrcAddr, u32 u32Len);
void vCrc16Ccitt(const u8 *u8Buf, u32 u32Len, u16 *u16CheckOld);
void vDelay(u32 u32Count);
#if (BSP_PLATFORM	==	BSP_PLATFORM_M3)
	void vInFlashErasePage(u32 u32StartSectorAddr,u32 u32EndSectorAddr);
#elif (BSP_PLATFORM	==	BSP_PLATFORM_M4	)
	void vInFlashEraseSector(u32 u32StartSectorAddr ,u32 u32EndSectorAddr,uint8_t VoltageRange);
#endif
u16 u16IapExFlashCrc(u32 u32SrcAddr, u32 u32Len,u16 *pCrc);
u8 u8IapLoadCode(u32 u32DestAddr,const u32 u32SrcAddr, u32 u32Len);
void vIapJumpToApp(u32 u32AppAddr);
//��ת����
void vIapJumpToBoot(uint32_t u32bootAddr);
void vIapWriteCodeLen(u32 u32Addr, u32 u32Len);
u32 u32IapReadCodeLen(u32 u32Addr);

void vIapWriteCrc(u32 u32Addr, u32 u32Crc);
u32 u32IapReadCrc(u32 u32Addr);

u32 u32IapReadFlag(u32 u32Addr);
u32 u32IapFlagSet(u32 u32Addr);
u32 u32IapFlagClear(u32 u32Addr);

u8 u8IapReadBuf(u8 *pBuffer, u32 u32ReadAddr, u16 u16NumByteToRead);
u8 u8IapWriteBuf(u8 *pBuffer, u32 u32ReadAddr, u16 u16NumByteToRead);
u8 u8IapEraserBulk(u32 u32BulkAddr);
u8 u8IapEraserSector(u32 u32SectorAddr);

void vIapLcdClear(void);
void vIapShowStr(u16 u16Inv, const u8 *cu8Str, u16 u16Row, u16 u16Col);


#endif
