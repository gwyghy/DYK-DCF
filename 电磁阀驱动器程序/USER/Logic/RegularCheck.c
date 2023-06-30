#include "includes.h"
extern u16  Mos_CheckTimer;			   //MOS�ܶ�ʱ����ʱ��
extern u16  Rcop_CheckTimer;		  //����������ʱ����ʱ��

extern u32 ActionWord;//Һѹ������Чֵ
extern u32 CheckWord;//��¼����⶯����Чֵ

extern u16  Mos_FaultTimer;	 //MOS�ܹ����ϱ�ʱ����ʱ��
extern u16  Rcop_FaultTimer; //������������ʱ����ʱ��

u16 Mos_Volt_Val[MOS_ADC_SampNum];
u32 Mos_VoltValue = 0x00000000;	//Value_Check�˲����������
/*********************************************************************************************************************
  **function name:	   Mos_CheckCycle()
  **description:	   Mos�Լ�⴦����
  **input value:	   u16 Time �������Լ��ʱ��
  **output value:��	�� ��
  **return value:	�� TRUE������쳣  FALSE: �������
**********************************************************************************************************************/
u8 Mos_CheckCycle(u16 Time)
{
	if(Mos_CheckTimer == TIMER_EXPIRED)
	{
		if((!ActionWord) &&(!CheckWord))
		{
			MosValue_PowerOff; //δ�յ�����ʱ�����鹩��ر�
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
  **description:	   ���������Լ�⴦����
  **input value:	   u16 Time �������Լ��ʱ��
  **output value:��	�� ��
  **return value:	�� TRUE������쳣  FALSE: �������
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
  **description:	   ����ʮ�Σ� ��������Mos_Volt_Val[i]
  **input value:
  **output value:
  **return value:      
**********************************************************************************************************************/
void Get_MosSamples_Val(void)
{
	u8 i;
	for(i=0;i<CHECK_ADC_SampNum;i++)
	{
		Mos_Volt_Val[i] = Get_Adc_Channel_Val(ADC_Channel_8);	                        //volt sensor��AD1��ͨ��15������ʮ��
	}
}

/*********************************************************************************************************************
  **function name:	   Mos_SampVal_Proc()
  **description:	   �����ݽ��д���,����ʮ�Σ�ȥ����Сֵ��ȡʣ��ֵ��ƽ��ֵ
  **input value:	   Mos_Volt_Val[]�����ʮ�β������������
  **output value:
  **return value:      sum/8���յĲ������ֵ
**********************************************************************************************************************/
u32 Mos_SampVal_Proc(void)
{
  u8  i,j;
	u16 tmp;
	u32 sum=0;
	//�򵥵�ð������
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
	//ȥ�����ֵ����Сֵ��ȡ��ƽ��ֵ
	for(i=1;i<MOS_ADC_SampNum-2;i++)
	{
		sum+=Mos_Volt_Val[i];
	}
	return sum / (MOS_ADC_SampNum - 3);
// 	return  (Volt_Val[SampNum - 1]);
}