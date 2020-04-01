#ifndef _CONTROL_H
#define _CONTROL_H

#include "stm32f10x.h"
#include "system_stm32f10x.h"
#include "core_cm3.h"
#include "bsp_GeneralTim.h" 
#include "encoder.h"
#include "bsp_usart.h"

#define  KP  35
#define  KI  0.02
void control(void);
void SysTick_Init(void);
extern char stop_motor;
//void Delay_us(__IO uint32_t nTime);
#endif  
