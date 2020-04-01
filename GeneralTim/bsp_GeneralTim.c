
#include "bsp_GeneralTim.h" 
#include  "stm32f10x.h"

#define KSpeed                25 //Mecanum_WheelSpeed*KSpeed 最大值为1000
#define R           1      //Wheel diameter
#define L           2     //Wheelbase
#define W           1     //Tread

 static void GENERAL_TIM_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // 输出比较通道1 GPIO 初始化
	RCC_APB2PeriphClockCmd(GENERAL_TIM_CH1_GPIO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  GENERAL_TIM_CH1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GENERAL_TIM_CH1_PORT, &GPIO_InitStructure);
	
	// 输出比较通道2 GPIO 初始化
	RCC_APB2PeriphClockCmd(GENERAL_TIM_CH2_GPIO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  GENERAL_TIM_CH2_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GENERAL_TIM_CH2_PORT, &GPIO_InitStructure);
	
	// 输出比较通道3 GPIO 初始化
	RCC_APB2PeriphClockCmd(GENERAL_TIM_CH3_GPIO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  GENERAL_TIM_CH3_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GENERAL_TIM_CH3_PORT, &GPIO_InitStructure);
	
	// 输出比较通道4 GPIO 初始化
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
// * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
// * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
// * 另外三个成员是通用定时器和高级定时器才有.
// *-----------------------------------------------------------------------------
// *typedef struct
// *{ TIM_Prescaler            都有
// *	TIM_CounterMode			     TIMx,x[6,7]没有，其他都有
// *  TIM_Period               都有
// *  TIM_ClockDivision        TIMx,x[6,7]没有，其他都有
// *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]才有
// *}TIM_TimeBaseInitTypeDef; 
// *-----------------------------------------------------------------------------
// */

/* ----------------   PWM信号 周期和占空比的计算--------------- */
// ARR ：自动重装载寄存器的值
// CLK_cnt：计数器的时钟，等于 Fck_int / (psc+1) = 72M/(psc+1)
// PWM 信号的周期 T = ARR * (1/CLK_cnt) = ARR*(PSC+1) / 72M
// 占空比P=CCR/(ARR+1)

static void Mecanum_PWMConfig(void)
{
  // 开启定时器时钟,即内部时钟CK_INT=72M
	GENERAL_TIM_APBxClock_FUN(GENERAL_TIM_CLK,ENABLE);

/*--------------------时基结构体初始化-------------------------*/
	// 配置周期，这里配置为100K
	
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	// 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
	TIM_TimeBaseStructure.TIM_Period=GENERAL_TIM_Period;	
	// 驱动CNT计数器的时钟 = Fck_int/(psc+1)
	TIM_TimeBaseStructure.TIM_Prescaler= GENERAL_TIM_Prescaler;	
	// 时钟分频因子 ，配置死区时间时需要用到
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
	// 计数器计数模式，设置为向上计数
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;		
	// 重复计数器的值，没用到不用管
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	// 初始化定时器
	TIM_TimeBaseInit(GENERAL_TIM, &TIM_TimeBaseStructure);
	
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	// 配置为PWM模式1
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	// 输出使能
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	// 输出通道电平极性配置	
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
