

//wheel0 ����  C6  B14   ����A6  A7
//wheel1 ����  C7  B15   ����B6  B7
//wheel2 ����  C8  D2    ����A8  A9
//wheel3 ����  C9  C12   ����B0  B1



// TIM��ͨ�ö�ʱ��-����������
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
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	/* ��ʱ����ʼ�� */
	USART_Config();
	Mecanum_PWM_Init();
	Encoder_Init();
	SysTick_Init();
//	void Delay(__IO uint32_t nCount);   ����
	char stop_motor=0;
	control();
}//////////66666666 
/*********************************************END OF FILE**********************/
void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����  ms
{ 
	int j;
	for(; nCount != 0; nCount--)
	for(j=10260;j!=0;j--);
}

