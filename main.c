

//wheel0 驱动  C6  B14   测速A6  A7
//wheel1 驱动  C7  B15   测速B6  B7
//wheel2 驱动  C8  D2    测速A8  A9
//wheel3 驱动  C9  C12   测速B0  B1



// TIM—通用定时器-编码器测速
<<<<<<< HEAD
#include "stm32f10x.h"
#include "bsp_GeneralTim.hhhhhhh"  
=======
#include "stm32f10x.hhhhhhhhh"
#include "bsp_GeneralTim.h"  
>>>>>>> test2
#include "encoder.h"
#include "bsp_usart.h"
#include "control.h"
/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	/* 定时器初始化 */
	USART_Config();
	Mecanum_PWM_Init();
	Encoder_Init();
	SysTick_Init();
//	void Delay(__IO uint32_t nCount);   声明
	char stop_motor=0;
	control();
}//////////66666666 
/*********************************************END OF FILE**********************/
void Delay(__IO uint32_t nCount)	 //简单的延时函数  ms
{ 
	int j;
	for(; nCount != 0; nCount--)
	for(j=10260;j!=0;j--);
}

