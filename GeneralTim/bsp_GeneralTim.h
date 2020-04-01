#ifndef __BSP_GENERALTIME_H
#define __BSP_GENERALTIME_H


#include "stm32f10x.h"


/********* ***通用定时器TIM参数定义，只限TIM2、3、4、5************/
// 当使用不同的定时器的时候，对应的GPIO是不一样的，这点要注意
// 我们这里默认使用TIM3
//  wheel0 C6  B14 
//  wheel1 C7  B15
//  wheel2 C8  D2
//  wheel3 C9  C12
#define            GENERAL_TIM                   TIM8
#define            GENERAL_TIM_APBxClock_FUN     RCC_APB2PeriphClockCmd
#define            GENERAL_TIM_CLK               RCC_APB2Periph_TIM8
#define            GENERAL_TIM_Period            1024
#define            GENERAL_TIM_Prescaler         6
// TIM3 输出比较通道1
#define            GENERAL_TIM_CH1_GPIO_CLK      RCC_APB2Periph_GPIOC
#define            GENERAL_TIM_CH1_PORT          GPIOC
#define            GENERAL_TIM_CH1_PIN           GPIO_Pin_6

// TIM3 输出比较通道2
#define            GENERAL_TIM_CH2_GPIO_CLK      RCC_APB2Periph_GPIOC
#define            GENERAL_TIM_CH2_PORT          GPIOC
#define            GENERAL_TIM_CH2_PIN           GPIO_Pin_7

// TIM3 输出比较通道3
#define            GENERAL_TIM_CH3_GPIO_CLK      RCC_APB2Periph_GPIOC
#define            GENERAL_TIM_CH3_PORT          GPIOC
#define            GENERAL_TIM_CH3_PIN           GPIO_Pin_8

// TIM3 输出比较通道4
#define            GENERAL_TIM_CH4_GPIO_CLK      RCC_APB2Periph_GPIOC
#define            GENERAL_TIM_CH4_PORT          GPIOC
#define            GENERAL_TIM_CH4_PIN           GPIO_Pin_9

/**************************函数声明********************************/
void Mecanum_WheelSpeedUpdate(int32_t SpeedX,int32_t SpeedY,int32_t VelocityZ, int* Target);
void Mecanum_SteelSpeedApply( int wheel0,int wheel1,int wheel2,int wheel3);
void Mecanum_PWM_Init(void);


#endif	/* __BSP_GENERALTIME_H */


