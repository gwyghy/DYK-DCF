#include "includes.h"
extern u16  Mos_CheckTimer;			   //MOS管定时检测计时器
extern u16  Rcop_CheckTimer;		  //过流保护定时检测计时器

extern u32 ActionWord;//液压动作有效值
extern u32 CheckWord;//记录阀检测动作有效值

extern u16  Mos_FaultTimer;	 //MOS管故障上报时长定时器
extern u16  Rcop_FaultTimer; //过流保护故障时长定时器

u16 Mos_Volt_Val[MOS_ADC_SampNum];
u32 Mos_VoltValue = 0x00000000;	//Value_Check滤波过后的数据
/*********************************************************************************************************************
  **function name:	   Mos_CheckCycle()
  **description:	   Mos自检测处理函数
  **input value:	   u16 Time 周期性自检测时间
  **output value:　	　 无
  **return value:	　 TRUE：检测异常  FALSE: 检测正常
**********************************************************************************************************************/
u8 Mos_CheckCycle(u16 Time)
{
	if(Mos_CheckTimer == TIMER_EXPIRED)
	{
		if((!ActionWord) &&(!CheckWord))
		{
			MosValue_PowerOff; //未收到数据时，阀组供电关闭
			Mos_VoltValue = 0x00000000;
			Get_MosSamples_Val();
			Mos_VoltValue = Mos_SampVal_Proc();	
		}
		Mos_CheckTimer = Time;			
	}
	if((Mos_VoltValue >= Mos_ValueMin) &&(Mos_VoltValue <= Mos_ValueMax))
	{
		return FALSE;
	}
	else 
	{
		if(Mos_FaultTimer == TIMER_EXPIRED)
		{
			s_CANxCheckFaultTxProc(0x00,0x01);
			Mos_FaultTimer = 200;
		}
		return TRUE;
	}	
}
 /*********************************************************************************************************************
  **function name:	   OverCurrent_Checkcycle()
  **description:	   过流保护自检测处理函数
  **input value:	   u16 Time 周期性自检测时间
  **output value:　	　 无
  **return value:	　 TRUE：检测异常  FALSE: 检测正常
**********************************************************************************************************************/
u8 OverCurrent_Checkcycle(u16 Time)
{
	static u8 u8i = TRUE;
	if(Rcop_CheckTimer == TIMER_EXPIRED)
	{
		u8i = GPIO_ReadInputDataBit(OVERCURRENT_PORT,OVERCURRENT_PIN);
		Rcop_CheckTimer = Time;
	}
	if(!u8i)
	{
		if(Rcop_FaultTimer ==TIMER_EXPIRED)
		{
			s_CANxCheckFaultTxProc(0x00,0x02);
			Rcop_FaultTimer =200;
		}
		return TRUE;
	}	
	else
	{
		return FALSE;
	}	
}
/*********************************************************************************************************************
  **function name:	   Get_MosSamples_Val()
  **description:	   采样十次， 存入数组Mos_Volt_Val[i]
  **input value:
  **output value:
  **return value:      
**********************************************************************************************************************/
void Get_MosSamples_Val(void)
{
	u8 i;
	for(i=0;i<CHECK_ADC_SampNum;i++)
	{
		Mos_Volt_Val[i] = Get_Adc_Channel_Val(ADC_Channel_8);	                        //volt sensor，AD1的通道15，采样十次
	}
}

/*********************************************************************************************************************
  **function name:	   Mos_SampVal_Proc()
  **description:	   对数据进行处理,采样十次，去掉大小值，取剩余值的平均值
  **input value:	   Mos_Volt_Val[]，存放十次采样结果的数组
  **output value:
  **return value:      sum/8最终的采样结果值
**********************************************************************************************************************/
u32 Mos_SampVal_Proc(void)
{
  u8  i,j;
	u16 tmp;
	u32 sum=0;
	//简单的冒泡排序法
	for(i=0;i<MOS_ADC_SampNum;i++)
	{
	    for(j=0;j<MOS_ADC_SampNum-i;j++)
		{
		    if(Mos_Volt_Val[j]>Mos_Volt_Val[j+1])
			{
			    tmp=Mos_Volt_Val[j];
				Mos_Volt_Val[j]=Mos_Volt_Val[j+1];
				Mos_Volt_Val[j+1]=tmp;
			}
		}
	}
	//去掉最大值，最小值，取其平均值
	for(i=1;i<MOS_ADC_SampNum-2;i++)
	{
		sum+=Mos_Volt_Val[i];
	}
	return sum / (MOS_ADC_SampNum - 3);
// 	return  (Volt_Val[SampNum - 1]);
}