#include "includes.h"
extern u32 Hare_Id;
u16 Volt_Val[CHECK_ADC_SampNum];
/*******************************************************************************************
  **function name:	   ADC_Configuration()
  **description:	   初始化并启动ADC转换
  **input value:
  **output value:
*******************************************************************************************/
void ADC_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure; 	
	//结构类型初始化
	ADC_DeInit(CHECK_ADC); //将外设 ADC1 的全部寄存器重设为默认值	
	
	RCC_APB2PeriphClockCmd(CHECK_ADC_GPIO_RCC, ENABLE);// 使能复用时钟和ADC1的时钟	
	
	/* Configure ADC1 pin */
	GPIO_InitStructure.GPIO_Pin = CHECK_ADC_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;         //模拟输入
	GPIO_Init(CHECK_ADC_GPIO_PORT, &GPIO_InitStructure);   
	
		/* 功能复用IO时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|CHECK_ADC_RCC, ENABLE);// 使能复用时钟和ADC1的时钟	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);                                  //72M/8=9M,ADC最大时间不能超过14M

	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	               //ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	                   //模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	               //模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	           //ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	                           //顺序进行规则转换的ADC通道的数目
	ADC_Init(CHECK_ADC, &ADC_InitStructure);	                               //根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

	ADC_Cmd(CHECK_ADC, ENABLE);	                                           //使能指定的ADC1
  
	ADC_ResetCalibration(CHECK_ADC);	                                       //重置指定的ADC1的校准寄存器
	while(ADC_GetResetCalibrationStatus(CHECK_ADC));	                       //获取ADC1重置校准寄存器的状态,设置状态则等待
	ADC_StartCalibration(CHECK_ADC);		                                   //开始指定ADC1的校准状态
	while(ADC_GetCalibrationStatus(CHECK_ADC));		                       //获取指定ADC1的校准程序,设置状态则等待
	ADC_SoftwareStartConvCmd(CHECK_ADC, ENABLE);	                           //使能指定的ADC1的软件转换启动功能
}

/*********************************************************************************************************************
  **function name:	   Get_Adc_Channel_Val()
  **description:	   配置采样通道等，并获取采样值
  **input value:	   通道值channel
  **output value:
  **return value:	   通道转换结果 ADC_GetConversionValue(ADC1)
**********************************************************************************************************************/
u16 Get_Adc_Channel_Val(u8 channel)   										//获取一次采样值
{
	ADC_RegularChannelConfig(CHECK_ADC, channel, 1, ADC_SampleTime_55Cycles5 );	    //ADC1,ADC通道3,规则采样顺序值为1,采样时间为55.5周期
	
	ADC_SoftwareStartConvCmd(CHECK_ADC, ENABLE);		                            //使能指定的ADC1的软件转换启动功能
	while(!ADC_GetFlagStatus(CHECK_ADC, ADC_FLAG_EOC ))break;                         //等待转换结束
	return ADC_GetConversionValue(CHECK_ADC);	                                //返回最近一次ADC1规则组的转换结果
}
/*********************************************************************************************************************
  **function name:	   Get_Samples_Proc()
  **description:	   采样十次， 存入数组Volt_Val[i]
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
			Volt_Val[i] = Get_Adc_Channel_Val(ADC_Channel_8);	                        //volt sensor，AD1的通道15，采样十次
		else 
			Volt_Val[i] = Get_Adc_Channel_Val(ADC_Channel_15);	
		Delay(5);
	}
}
/*********************************************************************************************************************
  **function name:	   SampVal_Proc()
  **description:	   对数据进行处理,采样十次，去掉大小值，取剩余值的平均值
  **input value:	   Volt_Val[]，存放十次采样结果的数组
  **output value:
  **return value:      sum/8最终的采样结果值
**********************************************************************************************************************/
u32 SampVal_Proc(void)
{
    u8  i,j;
	u16 tmp;
	u32 sum=0;
	//简单的冒泡排序法
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
	//去掉最大值，最小值，取其平均值
	for(i=1;i<CHECK_ADC_SampNum-2;i++)
	{
		sum+=Volt_Val[i];
	}
	return sum / (CHECK_ADC_SampNum - 3);
// 	return  (Volt_Val[SampNum - 1]);
}


