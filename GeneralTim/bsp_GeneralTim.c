
#include "bsp_GeneralTim.h" 
#include  "stm32f10x.h"

#define KSpeed                25 //Mecanum_WheelSpeed*KSpeed ���ֵΪ1000
#define R           1      //Wheel diameter
#define L           2     //Wheelbase
#define W           1     //Tread

 static void GENERAL_TIM_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // ����Ƚ�ͨ��1 GPIO ��ʼ��
	RCC_APB2PeriphClockCmd(GENERAL_TIM_CH1_GPIO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  GENERAL_TIM_CH1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GENERAL_TIM_CH1_PORT, &GPIO_InitStructure);
	
	// ����Ƚ�ͨ��2 GPIO ��ʼ��
	RCC_APB2PeriphClockCmd(GENERAL_TIM_CH2_GPIO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  GENERAL_TIM_CH2_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GENERAL_TIM_CH2_PORT, &GPIO_InitStructure);
	
	// ����Ƚ�ͨ��3 GPIO ��ʼ��
	RCC_APB2PeriphClockCmd(GENERAL_TIM_CH3_GPIO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  GENERAL_TIM_CH3_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GENERAL_TIM_CH3_PORT, &GPIO_InitStructure);
	
	// ����Ƚ�ͨ��4 GPIO ��ʼ��
	RCC_APB2PeriphClockCmd(GENERAL_TIM_CH4_GPIO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  GENERAL_TIM_CH4_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GENERAL_TIM_CH4_PORT, &GPIO_InitStructure);
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( GPIOB, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( GPIOB, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( GPIOC, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( GPIOD, &GPIO_InitStructure);
}


///*
// * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
// * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
// * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
// *-----------------------------------------------------------------------------
// *typedef struct
// *{ TIM_Prescaler            ����
// *	TIM_CounterMode			     TIMx,x[6,7]û�У���������
// *  TIM_Period               ����
// *  TIM_ClockDivision        TIMx,x[6,7]û�У���������
// *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]����
// *}TIM_TimeBaseInitTypeDef; 
// *-----------------------------------------------------------------------------
// */

/* ----------------   PWM�ź� ���ں�ռ�ձȵļ���--------------- */
// ARR ���Զ���װ�ؼĴ�����ֵ
// CLK_cnt����������ʱ�ӣ����� Fck_int / (psc+1) = 72M/(psc+1)
// PWM �źŵ����� T = ARR * (1/CLK_cnt) = ARR*(PSC+1) / 72M
// ռ�ձ�P=CCR/(ARR+1)

static void Mecanum_PWMConfig(void)
{
  // ������ʱ��ʱ��,���ڲ�ʱ��CK_INT=72M
	GENERAL_TIM_APBxClock_FUN(GENERAL_TIM_CLK,ENABLE);

/*--------------------ʱ���ṹ���ʼ��-------------------------*/
	// �������ڣ���������Ϊ100K
	
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	// �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
	TIM_TimeBaseStructure.TIM_Period=GENERAL_TIM_Period;	
	// ����CNT��������ʱ�� = Fck_int/(psc+1)
	TIM_TimeBaseStructure.TIM_Prescaler= GENERAL_TIM_Prescaler;	
	// ʱ�ӷ�Ƶ���� ����������ʱ��ʱ��Ҫ�õ�
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
	// ����������ģʽ������Ϊ���ϼ���
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;		
	// �ظ���������ֵ��û�õ����ù�
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	// ��ʼ����ʱ��
	TIM_TimeBaseInit(GENERAL_TIM, &TIM_TimeBaseStructure);
	
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	// ����ΪPWMģʽ1
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	// ���ʹ��
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	// ���ͨ����ƽ��������	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OC1Init(GENERAL_TIM, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);
  TIM_OC2Init(GENERAL_TIM, &TIM_OCInitStructure);
  TIM_OC2PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);
	TIM_OC3Init(GENERAL_TIM, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);
	TIM_OC4Init(GENERAL_TIM, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);
	TIM_Cmd(GENERAL_TIM, ENABLE);
  TIM_CtrlPWMOutputs(GENERAL_TIM, ENABLE);
}   

//static void Mecanum_PWMSet(uint8_t Wheel,uint8_t Direction,int32_t TIM_CCR)
//{
//switch(Wheel)
//    {
//        case 0:{
//           if(Direction)
//						 {
//							 TIM8->CCR1 = TIM_CCR;
//	       
//						  GPIO_ResetBits(GPIOB,GPIO_Pin_14); 
//							 break;
//							 }
//						 TIM8->CCR1  = 1000-TIM_CCR;
//	           GPIO_SetBits(GPIOB,GPIO_Pin_14);
//							 break;
//               }
//        case 1:{
//            if(Direction)
//						 {
//							 TIM8->CCR2  = TIM_CCR;
//							 GPIO_ResetBits(GPIOB,GPIO_Pin_15); 
//							 break;
//							 }
//						 TIM8->CCR2 = 1000-TIM_CCR;
//	           GPIO_SetBits(GPIOB,GPIO_Pin_15); 	 
//							 break;
//	
//              }
//        case 2:
//					{
//						if(Direction)
//						 {
//							 TIM8->CCR3 = TIM_CCR;
//							 GPIO_ResetBits(GPIOD,GPIO_Pin_2);
//							 break;
//							 }
//						  TIM8->CCR3 = 1000-TIM_CCR;
//							GPIO_SetBits(GPIOD,GPIO_Pin_2);
//							 break;
//				 }
//           
//				case 3:
//					{
//            if(Direction)
//						 {
//							  TIM8->CCR4 = TIM_CCR;
//							 GPIO_ResetBits(GPIOC,GPIO_Pin_12);
//							 break;
//							 }
//						 TIM8->CCR4 = 1000-TIM_CCR;
//						 GPIO_SetBits(GPIOC,GPIO_Pin_12);
//							 break;
//	
//           }
//         }
//}
//	








static void Mecanum_PWMSet(uint8_t Wheel,uint8_t Direction,int32_t TIM_CCR)
{
switch(Wheel)
    {
        case 0:{
           if(Direction)
						 {
							 TIM8->CCR1 = TIM_CCR;
	            
						  GPIO_ResetBits(GPIOB,GPIO_Pin_14); 
							 break;
							 }
						 TIM8->CCR1  = 1000-TIM_CCR;
	           GPIO_SetBits(GPIOB,GPIO_Pin_14);
							 break;
               }
        case 1:{
            if(Direction)
						 {
							 TIM8->CCR2  = TIM_CCR;
							 GPIO_ResetBits(GPIOB,GPIO_Pin_15); 
							 break;
							 }
						 TIM8->CCR2 = 1000-TIM_CCR;
	           GPIO_SetBits(GPIOB,GPIO_Pin_15); 	 
							 break;
	
              }
        case 2:
					{
						if(Direction)
						 {
							 TIM8->CCR3 = TIM_CCR;
							 GPIO_ResetBits(GPIOD,GPIO_Pin_2);
							 break;
							 }
						  TIM8->CCR3 = 1000-TIM_CCR;
							GPIO_SetBits(GPIOD,GPIO_Pin_2);
							 break;
				 }
           
				case 3:
					{
            if(Direction)
						 {
							  TIM8->CCR4 = TIM_CCR;
							 GPIO_ResetBits(GPIOC,GPIO_Pin_12);
							 break;
							 }
						 TIM8->CCR4 = 1000-TIM_CCR;
						 GPIO_SetBits(GPIOC,GPIO_Pin_12);
							 break;
	
           }
         }
}
	


 void  Mecanum_SteelSpeedApply( int wheel0,int wheel1,int wheel2,int wheel3)
 {
	   int Mecanum_WheelSpeed[4];
	    Mecanum_WheelSpeed[0]= wheel0;
	    Mecanum_WheelSpeed[1]= wheel1;
	    Mecanum_WheelSpeed[2]= wheel2;
	    Mecanum_WheelSpeed[3]= wheel3;
    for(uint8_t i = 0;i<4;i++)
   {
        if(Mecanum_WheelSpeed[i]>0)
        {
           if(Mecanum_WheelSpeed[i]<1000)  Mecanum_PWMSet(i,1,Mecanum_WheelSpeed[i]);
				   else Mecanum_PWMSet(i,1,1000);
        }
        else
        {
					if(-Mecanum_WheelSpeed[i]<1000) Mecanum_PWMSet(i,0,Mecanum_WheelSpeed[i]);
				   else Mecanum_PWMSet(i,0,1000);
        }
    }
 }

 void Mecanum_WheelSpeedUpdate(int32_t SpeedX,int32_t SpeedY,int32_t VelocityZ, int* Target)
 {
	 int i=0,j=0;
	  *(Target + 0) =-(L+W)*VelocityZ/2+SpeedX+SpeedY;
	  *(Target + 1) =-(L+W)*VelocityZ/2+SpeedX-SpeedY;
	  *(Target + 2) =-(L+W)*VelocityZ/2-SpeedX+SpeedY;
	  *(Target + 3) =-(L+W)*VelocityZ/2-SpeedX-SpeedY;
	 for(;i<4;i++)
	 {
		 if(*(Target + i)*KSpeed>1000) 
			 for(j=0;j<4;j++)
					*(Target + j)=(*(Target + j))*1000/(*(Target + i))/(KSpeed);
		 
		if((*(Target + i))*KSpeed<-1000) 
			 for(j=0;j<4;j++)
					(*(Target + j))=-(*(Target + j))*1000/(*(Target + i))/(KSpeed);			
		 
	 }
 }



void Mecanum_PWM_Init(void)
{
	GENERAL_TIM_GPIO_Config();
	Mecanum_PWMConfig();
  
	
}

/*********************************************END OF FILE**********************/
