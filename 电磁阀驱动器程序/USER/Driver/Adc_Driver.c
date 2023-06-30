#include "includes.h"
extern u32 Hare_Id;
u16 Volt_Val[CHECK_ADC_SampNum];
/*******************************************************************************************
  **function name:	   ADC_Configuration()
  **description:	   ��ʼ��������ADCת��
  **input value:
  **output value:
*******************************************************************************************/
void ADC_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure; 	
	//�ṹ���ͳ�ʼ��
	ADC_DeInit(CHECK_ADC); //������ ADC1 ��ȫ���Ĵ�������ΪĬ��ֵ	
	
	RCC_APB2PeriphClockCmd(CHECK_ADC_GPIO_RCC, ENABLE);// ʹ�ܸ���ʱ�Ӻ�ADC1��ʱ��	
	
	/* Configure ADC1 pin */
	GPIO_InitStructure.GPIO_Pin = CHECK_ADC_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;         //ģ������
	GPIO_Init(CHECK_ADC_GPIO_PORT, &GPIO_InitStructure);   
	
		/* ���ܸ���IOʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|CHECK_ADC_RCC, ENABLE);// ʹ�ܸ���ʱ�Ӻ�ADC1��ʱ��	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);                                  //72M/8=9M,ADC���ʱ�䲻�ܳ���14M

	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	               //ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	                   //ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	               //ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	           //ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	                           //˳����й���ת����ADCͨ������Ŀ
	ADC_Init(CHECK_ADC, &ADC_InitStructure);	                               //����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   

	ADC_Cmd(CHECK_ADC, ENABLE);	                                           //ʹ��ָ����ADC1
  
	ADC_ResetCalibration(CHECK_ADC);	                                       //����ָ����ADC1��У׼�Ĵ���
	while(ADC_GetResetCalibrationStatus(CHECK_ADC));	                       //��ȡADC1����У׼�Ĵ�����״̬,����״̬��ȴ�
	ADC_StartCalibration(CHECK_ADC);		                                   //��ʼָ��ADC1��У׼״̬
	while(ADC_GetCalibrationStatus(CHECK_ADC));		                       //��ȡָ��ADC1��У׼����,����״̬��ȴ�
	ADC_SoftwareStartConvCmd(CHECK_ADC, ENABLE);	                           //ʹ��ָ����ADC1�����ת����������
}

/*********************************************************************************************************************
  **function name:	   Get_Adc_Channel_Val()
  **description:	   ���ò���ͨ���ȣ�����ȡ����ֵ
  **input value:	   ͨ��ֵchannel
  **output value:
  **return value:	   ͨ��ת����� ADC_GetConversionValue(ADC1)
**********************************************************************************************************************/
u16 Get_Adc_Channel_Val(u8 channel)   										//��ȡһ�β���ֵ
{
	ADC_RegularChannelConfig(CHECK_ADC, channel, 1, ADC_SampleTime_55Cycles5 );	    //ADC1,ADCͨ��3,�������˳��ֵΪ1,����ʱ��Ϊ55.5����
	
	ADC_SoftwareStartConvCmd(CHECK_ADC, ENABLE);		                            //ʹ��ָ����ADC1�����ת����������
	while(!ADC_GetFlagStatus(CHECK_ADC, ADC_FLAG_EOC ))break;                         //�ȴ�ת������
	return ADC_GetConversionValue(CHECK_ADC);	                                //�������һ��ADC1�������ת�����
}
/*********************************************************************************************************************
  **function name:	   Get_Samples_Proc()
  **description:	   ����ʮ�Σ� ��������Volt_Val[i]
  **input value:
  **output value:
  **return value:      
**********************************************************************************************************************/
void Get_Samples_Val(void)
{
	u8 i;
	for(i=0;i<CHECK_ADC_SampNum;i++)
	{
		if(Hare_Id == DEVICE_MOS)
			Volt_Val[i] = Get_Adc_Channel_Val(ADC_Channel_8);	                        //volt sensor��AD1��ͨ��15������ʮ��
		else 
			Volt_Val[i] = Get_Adc_Channel_Val(ADC_Channel_15);	
		Delay(5);
	}
}
/*********************************************************************************************************************
  **function name:	   SampVal_Proc()
  **description:	   �����ݽ��д���,����ʮ�Σ�ȥ����Сֵ��ȡʣ��ֵ��ƽ��ֵ
  **input value:	   Volt_Val[]�����ʮ�β������������
  **output value:
  **return value:      sum/8���յĲ������ֵ
**********************************************************************************************************************/
u32 SampVal_Proc(void)
{
    u8  i,j;
	u16 tmp;
	u32 sum=0;
	//�򵥵�ð������
	for(i=0;i<CHECK_ADC_SampNum;i++)
	{
	    for(j=0;j<CHECK_ADC_SampNum-i;j++)
		{
		    if(Volt_Val[j]>Volt_Val[j+1])
			{
			    tmp=Volt_Val[j];
				Volt_Val[j]=Volt_Val[j+1];
				Volt_Val[j+1]=tmp;
			}
		}
	}
	//ȥ�����ֵ����Сֵ��ȡ��ƽ��ֵ
	for(i=1;i<CHECK_ADC_SampNum-2;i++)
	{
		sum+=Volt_Val[i];
	}
	return sum / (CHECK_ADC_SampNum - 3);
// 	return  (Volt_Val[SampNum - 1]);
}


