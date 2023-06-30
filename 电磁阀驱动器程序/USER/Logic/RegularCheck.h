#ifndef __REGULAR_CHECK_H
#define __REGULAR_CHECK_H
#include "includes.h"

#define MOS_TIME  5
#define OCP_TIME  5
#define	MOS_ADC_SampNum  10

#define Mos_ValueMin   101
#define Mos_ValueMax   4096

void Get_MosSamples_Val(void);
u32 Mos_SampVal_Proc(void);
u8 Mos_CheckCycle(u16 Time);
u8 OverCurrent_Checkcycle(u16 Time);
#endif 

