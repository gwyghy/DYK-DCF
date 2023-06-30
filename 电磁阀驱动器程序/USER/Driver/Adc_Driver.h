#ifndef __ADC_DRIVER_H
#define __ADC_DRIVER_H
#include "includes.h"
#define CHECK_ADC_GPIO_RCC 		RCC_APB2Periph_GPIOB
#define CHECK_ADC_GPIO_PORT 	GPIOB
#define CHECK_ADC_GPIO_PIN		GPIO_Pin_0

#define CHECK_ADC_RCC 			RCC_APB2Periph_ADC1
#define CHECK_ADC				ADC1

#define CHECK_ADC_SampNum		10

#define ShortValueMax 	100
#define FormalValueMin  101
#define FormalValueMax  2500
#define OpenValueMin    2500
#define OpenValueMax    4096

void ADC_Configuration(void);
u16 Get_Adc_Channel_Val(u8 channel);
void Get_Samples_Val(void);
u32 SampVal_Proc(void);
#endif
